#include "stm32f10x.h"
#include "delay.h"
#include "stepper_motor.h"
#include "os.h"
void lightAndWatering(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					
    GPIO_ResetBits(GPIOC,GPIO_Pin_5);		
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);	    
}
 

void Light_On_Off(uint8_t Switch)
{
    if (Switch==0)
    {
         GPIO_ResetBits(GPIOC,GPIO_Pin_5);
    }else if(Switch==1)
    {
         GPIO_SetBits(GPIOC,GPIO_Pin_5);
    }
}


void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM2, DISABLE);  //ʹ��TIMx����
							 
}

void TIM8_PWM_Init(u16 arr,u16 psc)
{ 
    
    
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );
	   
 

    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
   //��ʼ��TIM8
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period =arr;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	
	
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);	
	
	
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); 
	TIM_SetCompare1(TIM8,arr/2);
}



volatile int step;                       //������
volatile int flag=0,hold=0;     


void TIM2_IRQHandler(void)   //TIM3�ж�
{  
    static int count=0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			count++;
            
			if (count>step)
			{
                count=0;
                step=0;
                flag=1;
                TIM_Cmd(TIM8, DISABLE);
                TIM_Cmd(TIM2, DISABLE);
                GPIO_ResetBits(GPIOC,GPIO_Pin_7);
            }
		}
}



void direction(int a)        //��Ϊ��ת����Ϊ��ת
{	 

    if(a>0)
	 {
			ln1(1);             //ʹ��
			ln2(1);
	 }
	if(a<0)
	 {

		 ln1(1);
		 ln2(0);
	 }
}



 void Stepper_motor(void)
{
    TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
    TIM_Cmd(TIM8, ENABLE);
}



void watering(uint32_t step_t)              		
{
    GPIO_SetBits(GPIOC,GPIO_Pin_7);   
    hold=0;
    //direction(1); 
    step=step_t;
    TIM2_Int_Init(999,30);
    TIM8_PWM_Init(999,30);   
    Stepper_motor();

}


