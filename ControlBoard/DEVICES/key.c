#include "key.h"
#include "delay.h"

volatile uint8_t KEY_Scan_STA=0;
//KEY_Scan_STA���壺
//Bit		  7		  6	  	5		  4		  3		  2	  	1		  0
//Key		���±�ʶ	NC		NC		KEY5	KEY4	KEY3	KEY2	KEY1
//1��ʾ��Ч��0��ʾ��Ч

volatile uint8_t M_Encoder_STA=0;
//M_Encoder_STA���壺
//Bit	   	7		  6		  5	  	4	  	3		  2	  	1	   	0
//Key		���±�ʶ	NC		NC		NC		NC		NC		CCW		CW
//1��ʾ��Ч��0��ʾ��Ч


/*********************************************
* ����IOɨ��
*********************************************/
void Key_Scan(void)
{
	static uint8_t keysta_old=0;
	uint8_t keysta_new=0x00;
    keysta_new = (((~GPIO_ReadInputData(GPIOF))>>1)&0x0F);

	
#if USE_PIT_KEYSCAN==0
	
	keysta_old=keysta_new;
	keysta_new=0x00;
	
	DelayMs(10);	//��������
	

    keysta_new = (((~GPIO_ReadInputData(GPIOF))>>1)&0x0F);
	
#endif
	

	KEY_Scan_STA=keysta_new&keysta_old;
	
	KEY_Scan_STA|=KEY_Scan_STA_Update_MASK;		//��ӱ�ʶλ
	
	keysta_old=keysta_new;
}


/*********************************************
* ����IO��ʼ��
*********************************************/
void Key_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	                    //ʹ��PF�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //KEY1234�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	 //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOF

	Key_Scan();	//��ʼ��KeyUp_old,KeyDown_old
	
#if USE_PIT_KEYSCAN!=0
	delay_ms(10);
	Key_Scan();
#endif
}




/******************************************************
*����״̬
*�����������ʵ�ֶఴ���������
******************************************************/
uint8_t Key_STA(void)
{
	static uint8_t keysta_mask=0xFF;
	static uint8_t keysta_mask_old=0xFF;
	static uint8_t keysta_cnt=0;
	
	uint8_t keysta;
//	uint8_t keysta_out;
	
#if USE_PIT_KEYSCAN==0
	
	Key_Scan();
	
#endif
	
	if(KEY_Scan_STA&0x80)
	{
		keysta=KEY_Scan_STA&0x3F;
		KEY_Scan_STA=0;
	}
	else 
		return 0;
	
	
	//���MASK
	keysta_mask|=~keysta;
	keysta_mask_old|=~keysta;
	
	//cnt����
 	if(keysta_mask&keysta)
	{
		keysta_cnt=0;
		keysta_mask_old=keysta_mask;
	}
	
	//���keysta
	if(keysta_cnt>=Key_KeepTime)
	{
		keysta&=keysta_mask_old;
		keysta_cnt-=Key_KeepStep;
	}
	else
	{
		keysta&=keysta_mask;
		keysta_cnt++;
	}
	
	//�趨MASK
	keysta_mask&=~keysta;
	
	return keysta;
}
