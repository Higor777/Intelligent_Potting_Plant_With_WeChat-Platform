#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h"

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
__align(8) CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);



//�������ȼ�
#define Move_TASK_PRIO		4
//�����ջ��С
#define Move_STK_SIZE		512
//������ƿ�
OS_TCB	MoveTaskTCB;
//�����ջ
__align(8) CPU_STK	Move_TASK_STK[Move_STK_SIZE];
//������
void Move_task(void *p_arg);

//�������ȼ�
#define RC_TASK_PRIO		4
//�����ջ��С
#define RC_STK_SIZE		512
//������ƿ�
OS_TCB	RCTaskTCB;
//�����ջ
__align(8) CPU_STK	RC_TASK_STK[RC_STK_SIZE];
//������
void RC_task(void *p_arg);


//�������ȼ�
#define Light_TASK_PRIO		4
//�����ջ��С
#define Light_STK_SIZE		128
//������ƿ�
OS_TCB	LightTaskTCB;
//�����ջ
__align(8) CPU_STK	Light_TASK_STK[Light_STK_SIZE];
//������
void Light_task(void *p_arg);


//�������ȼ�
#define Metal_Sensor_TASK_PRIO		4
//�����ջ��С
#define Metal_Sensor_STK_SIZE		512
//������ƿ�
OS_TCB	Metal_SensorTaskTCB;
//�����ջ
__align(8) CPU_STK	Metal_Sensor_TASK_STK[Metal_Sensor_STK_SIZE];
//������
void Metal_Sensor_task(void *p_arg);


//�������ȼ�
#define UI_TASK_PRIO		4
//�����ջ��С
#define UI_STK_SIZE		256
//������ƿ�
OS_TCB	UITaskTCB;
//�����ջ
__align(8) CPU_STK	UI_TASK_STK[UI_STK_SIZE];
//������
void UI_task(void *p_arg);


float system_time_100ms = 0.0;

//���ڲ��԰�������
float test_para1 = 1;
float test_para2 = 1;
uint16_t device_id_1=0;
uint16_t device_id_2=0;
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
    
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    USART_Module_Init();
    LCD_Init();
    ParameterList_Init();
    Key_Init();
    
    TIMER_PWM_Init(1000,6);
    temperature_soil_moisture_init();
    TSL2561_Init(TIMING_402MS);
    lightAndWatering();
    
    
    //mfrc522_Init();

    /*
    while(device_id_1 == 0){
        device_id_1 = LDC1614_1_Init(); 
    }
    while(device_id_2 == 0){
        device_id_2 = LDC1614_2_Init();    
    }
	*/
    
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*2=2ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���

			 
	//����Move����
	OSTaskCreate((OS_TCB 	* )&MoveTaskTCB,		
				 (CPU_CHAR	* )"Move task", 		
                 (OS_TASK_PTR )Move_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )Move_TASK_PRIO,     	
                 (CPU_STK   * )&Move_TASK_STK[0],	
                 (CPU_STK_SIZE)Move_STK_SIZE/10,	
                 (CPU_STK_SIZE)Move_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);		
	//����RC����
	OSTaskCreate((OS_TCB 	* )&RCTaskTCB,		
				 (CPU_CHAR	* )"RC task", 		
                 (OS_TASK_PTR )RC_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )RC_TASK_PRIO,     	
                 (CPU_STK   * )&RC_TASK_STK[0],	
                 (CPU_STK_SIZE)RC_STK_SIZE/10,	
                 (CPU_STK_SIZE)RC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
	//����Metal_Sensor����
                 /*
	OSTaskCreate((OS_TCB 	* )&Metal_SensorTaskTCB,		
				 (CPU_CHAR	* )"Metal_Sensor task", 		
                 (OS_TASK_PTR )Metal_Sensor_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )Metal_Sensor_TASK_PRIO,     	
                 (CPU_STK   * )&Metal_Sensor_TASK_STK[0],	
                 (CPU_STK_SIZE)Metal_Sensor_STK_SIZE/10,	
                 (CPU_STK_SIZE)Metal_Sensor_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
 */  
	//����Light����
	OSTaskCreate((OS_TCB 	* )&LightTaskTCB,		
				 (CPU_CHAR	* )"Light task", 		
                 (OS_TASK_PTR )Light_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )Light_TASK_PRIO,     	
                 (CPU_STK   * )&Light_TASK_STK[0],	
                 (CPU_STK_SIZE)Light_STK_SIZE/10,	
                 (CPU_STK_SIZE)Light_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
              

	//����UI����
	OSTaskCreate((OS_TCB 	* )&UITaskTCB,		
				 (CPU_CHAR	* )"UI task", 		
                 (OS_TASK_PTR )UI_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )UI_TASK_PRIO,     	
                 (CPU_STK   * )&UI_TASK_STK[0],	
                 (CPU_STK_SIZE)UI_STK_SIZE/10,	
                 (CPU_STK_SIZE)UI_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )1,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
    //IWDG_Configuration();
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}


//Move����
void Move_task(void *p_arg)
{
    OS_ERR err;
	while(1)
	{
        
        move_base_control();
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ5ms,��ʱ����������л�
        //OSSchedRoundRobinYield(&err);
	}
}

//RC����
void RC_task(void *p_arg)
{
    OS_ERR err;
	while(1)
	{
        //IWDG_ReloadCounter();
        system_time_100ms = OSTimeGet(&err)*0.02;//100ms
        //��ʱ��Ƭ�ڴ�����Ϣ���У������Ϣ����Ϊ�գ����ó�����ʱ��
        while(RCMsg_Hander())
        {
            ;
        }
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ5ms,��ʱ����������л�
		//OSSchedRoundRobinYield(&err);
	}
}

extern uint32_t L1,L2,L3,L4,R1,R2,R3,R4;
extern int16_t error;
//Metal_Sensor����
void Metal_Sensor_task(void *p_arg)
{
    OS_ERR err;
    uint8_t flag = 0;
    unsigned char str[100];
	while(1)
	{
        flag ++;
        getline_L(&flag);
        sprintf((char*)str,"L: %d %d %d %d",L1,L2,L3,L4);
        LCD_ShowString(0,150,350,24,24,str);
        sprintf((char*)str,"R: %d %d %d %d",R1,R2,R3,R4);
        LCD_ShowString(0,180,350,24,24,str);
        sprintf((char*)str,"Error: %-9d",error);
        LCD_ShowString(0,210,350,24,24,str);
        
        
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ5ms,��ʱ����������л�
		//OSSchedRoundRobinYield(&err);
	}
}


//Light����
void Light_task(void *p_arg)
{
    OS_ERR err;
    uint16_t illumination_data= 0;
    float temperature_data= 0;
    uint16_t soil_moisture_data= 0;
    unsigned char str[100];
	while(1)
	{
        
		illumination_data= get_illumination();
        sprintf((char*)str,"illumination: %d lux",illumination_data);
        LCD_ShowString(0,240,300,24,24,str);
        temperature_data = get_temperature();
        sprintf((char*)str,"temperature: %3.1f ",temperature_data);
        LCD_ShowString(0,270,300,24,24,str);
        soil_moisture_data = get_soil_moisture();
        sprintf((char*)str,"soil_moisture: %d %%",soil_moisture_data);
        LCD_ShowString(0,300,300,24,24,str);
        USART_printf(USART1,"|i:%d lux|t:%2.1f |s:%d%%|",illumination_data,temperature_data,soil_moisture_data);
        
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ500ms,��ʱ����������л�
        
		//OSSchedRoundRobinYield(&err);
	}
}


//UI����
void UI_task(void *p_arg)
{
    OS_ERR err;
	float system_time_s = 0;
    unsigned char str[100];
    //uint8_t status;
    //uint8_t g_ucTempbuf[20];  
	while(1)
	{
		

        system_time_s = system_time_100ms*0.1;
        sprintf((char*)str,"Time : %d h %d m %d s.",(int)system_time_s/3600,(int)system_time_s/60,(int)system_time_s%60);
		POINT_COLOR=RED;	  
        LCD_ShowString(0,0,300,24,24,str); 
        /*
        LCD_ShowString(30,70,200,16,16,"TFTLCD UI");
        LCD_ShowString(30,90,200,16,16,"zz");
        sprintf((char*)str,"UI1: %f    UI2: %f",test_para1,test_para2);
        LCD_ShowString(30,110,200,16,16,str);		//��ʾLCD ID
        LCD_ShowString(30,130,200,12,12,"2017/4/9");
        */
        
        Key_Scan();
        Parameter_Change();

        
        
        //status = PcdRequest(PICC_REQALL, g_ucTempbuf);//Ѱ��
        //sprintf((char*)str,"Status1:%d",status);
        //LCD_ShowString(0,30,300,24,24,str); 
        //sprintf((char*)str,"Card type:%x %x",g_ucTempbuf[0],g_ucTempbuf[1]);
        //LCD_ShowString(0,60,300,24,24,str); 
        //status = PcdAnticoll(g_ucTempbuf);//����ײ
        //sprintf((char*)str,"Card value:%x %x %x %x",g_ucTempbuf[0],g_ucTempbuf[1],g_ucTempbuf[2],g_ucTempbuf[3]);
        //LCD_ShowString(0,90,300,24,24,str); 
        //sprintf((char*)str,"Status2:%d",status);
        //LCD_ShowString(0,120,300,24,24,str);
    

                    
                    
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ5ms,��ʱ����������л�
	}
}



 

