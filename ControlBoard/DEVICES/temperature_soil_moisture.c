 #include "temperature_soil_moisture.h"
 #include "delay.h"
 #include "os.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
   
void temperature_soil_moisture_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    Adc_Init();
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE );
    //һ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOG, &GPIO_InitStructure);					
    GPIO_SetBits(GPIOG,GPIO_Pin_11);	


}
		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 


	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
    //PC0 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(1);
	}
	return temp_val/times;
} 	 



#define DQ_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=(u32)8<<(4*3);}
#define DQ_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=(u32)3<<(4*3);}

//IO��������	 

#define DQ_OUT_DATA    PGout(11) 
#define DQ_READ_DATA   PGin(11)  

#define TIME_OUT  100


void DQ_Delayus(uint32_t us){
  uint32_t n,m;
  for(n=0;n<us;n++) {
    for(m=0;m<10;m++)m=m;
  }
}

//�������ڷ���0�������ڷ���1
uint8_t DQ_reset(void)
{
    int read_time = 0;
    DQ_OUT();
    DQ_OUT_DATA = 1;
    DQ_Delayus(5);
    DQ_OUT_DATA = 0;
    DQ_Delayus(700);
    DQ_OUT_DATA = 1;
    DQ_Delayus(20);
    DQ_IN();
    //���������ڻ��DQ���ͣ�����100û�����жϴ�����δ����
    while(DQ_READ_DATA==1 && read_time<TIME_OUT)read_time++;
    if(read_time>=TIME_OUT)
        return 1;
    DQ_Delayus(250);
    return 0;
    
}


int WRITE1 = 10;
int WRITE2 = 50;

void DQ_write_1(void)
{
    DQ_OUT();
    DQ_OUT_DATA = 1;
    DQ_Delayus(5);
    DQ_OUT_DATA = 0;
    DQ_Delayus(15);
    DQ_OUT_DATA = 1;
    DQ_Delayus(45);
}

void DQ_write_0(void)
{
    DQ_OUT();
    DQ_OUT_DATA = 1;
    DQ_Delayus(5);
    DQ_OUT_DATA = 0;
    DQ_Delayus(60);
    DQ_OUT_DATA = 1;
}
int WRITE11 = 3;
int WRITE22 = 10;
uint8_t DQ_read(void)
{
    uint8_t data;
    DQ_OUT();
    DQ_OUT_DATA = 1;
    DQ_Delayus(5);
    DQ_OUT_DATA = 0;
    DQ_Delayus(20);
    DQ_OUT_DATA = 1;
    DQ_IN();
    data = DQ_READ_DATA;
    DQ_OUT();
    DQ_OUT_DATA = 1;
    return data;

}

void DQ_write_byte(uint8_t data)
{
    uint8_t i=0;
    for(i=0;i<8;i++)
    {
        if(data&0x01)
            DQ_write_1();
        else
            DQ_write_0();
        data = data>>1;
    }
}

uint16_t DQ_read_byte(void)
{
    uint16_t data=0,data_temp=0;
    uint8_t i=0;
    for(i=0;i<8;i++)
    {
        data_temp = data_temp>>1;
        if(DQ_read())
        {
            data_temp |= 0x80;
        }
        
        
        
    }
//    data |= data_temp>>8;
//    data |= data_temp<<8;
    data = data_temp;
    return data;
}

const char CRC8Table[]={
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};
 
unsigned char CRC8_Table( char *p, char counter)
{
    unsigned char crc8 = 0;
    for( ; counter > 0; counter--){
        crc8 = CRC8Table[crc8^*p];
        p++;
    }
    return(crc8);
}

float get_temperature(void)
{
    char temp[9],crc;
    float tempdata;
    static float last;
    uint8_t i = 0;
    OS_ERR err;
    CPU_SR_ALLOC();
    OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ1ms,��ʱ����������л�
    OS_CRITICAL_ENTER();//�����ٽ���
    if(DQ_reset())return 77;
    DQ_write_byte(0xcc);//Skip ROM
    DQ_write_byte(0x44);//��������
    OS_CRITICAL_EXIT();	//�˳��ٽ���	 
    OSTimeDlyHMSM(0,0,0,750,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ750ms,��ʱ����������л�
    OS_CRITICAL_ENTER();//�����ٽ���
    if(DQ_reset())return 77;
    DQ_write_byte(0xcc);//Skip ROM
    DQ_write_byte(0xbe);//��ʼ��ȡ����
    for(i=0;i<9;i++)
    {
        temp[i] = DQ_read_byte();
    }
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
    OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ1ms,��ʱ����������л�    
    crc = CRC8_Table(temp,8);
    if(crc==temp[8])
    {
        tempdata=(float)(temp[0]+((temp[1]&0x0F)*256))/16.0;
        last = tempdata;
    }else
    {
        tempdata=last;
    }
    

    return tempdata;
}

u16 get_soil_moisture(void)
{
    u16 temp = 0;
    temp = 100-Get_Adc_Average(ADC_Channel_1,10)/41;
    return temp;
}


























