/*
   ���� 
   ������ҵ��ѧ���������
   ��ʮһ��������������
   LDC1614���Գ���
   2016.7.29
   by.LST
*/
#include "LDC1614.h"
#include "myiic.h"

static void LDC1614_Delay(void);



/*
 *   LDC1614_1_Init
 *   �ú������ڳ�ʼ��LDC1614
 *
 *   ������
 *   RegisterAddress 
 *    |__ void
 *   Data
 *    |__ device_id 
 */
uint16_t LDC1614_1_Init(void)
{
    uint16_t device_id;
    int cnt;
	I2C_InitTypeDef    I2C_InitStructure;
    GPIO_InitTypeDef               GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LDC1614_1_I2CX_GPIO_RCC,ENABLE);            //û���ⲿ�ж�,û����ӳ��,���Բ���Ҫ�������ù���

	GPIO_InitStructure.GPIO_Pin    =LDC1614_1_SCLPIN|LDC1614_1_SDAPIN;
	GPIO_InitStructure.GPIO_Speed  =GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   =GPIO_Mode_AF_OD;            //���ùܽ�Ϊ���ù��ܿ�©��� SDA�������������

	GPIO_Init(LDC1614_1_I2CX_GPIO, &GPIO_InitStructure);
    //��ʼ��LDC1614_1_I2CX

	RCC_APB1PeriphClockCmd(LDC1614_1_I2CX_RCC,ENABLE);
	I2C_DeInit(LDC1614_1_I2CX);

	I2C_InitStructure.I2C_ClockSpeed          =200000;   		 	//200KHz I2Cʱ��Ƶ��
	I2C_InitStructure.I2C_Mode                =I2C_Mode_I2C;   	//I2Cģʽ
	I2C_InitStructure.I2C_DutyCycle           =I2C_DutyCycle_2; 	//ʱ��ռ�ձ�
	I2C_InitStructure.I2C_OwnAddress1         =0x30;     			//������ַ ���������
	I2C_InitStructure.I2C_Ack                 =I2C_Ack_Enable; 	//����ACKӦ����Ӧ
	I2C_InitStructure.I2C_AcknowledgedAddress =I2C_AcknowledgedAddress_7bit;//7λ��ַģʽ ��10Ϊ��ַģʽ

	I2C_Init(LDC1614_1_I2CX,&I2C_InitStructure);
	I2C_Cmd(LDC1614_1_I2CX,ENABLE);
	I2C_AcknowledgeConfig(LDC1614_1_I2CX,ENABLE);
    
    

    LDC1614_1_WriteReg(RESET_DEV, 0x80, 0x00);

    for(cnt=0;cnt<300;cnt++){
    LDC1614_Delay();
    }
    //��ȡ�豸ID
    device_id = LDC1614_1_ReadReg(DEVICE_ID);

    //���мĴ�������
    if(device_id==0x3055){

    LDC1614_1_WriteReg(RCOUNT_CH0, 0x02, 0x20); 
    LDC1614_1_WriteReg(RCOUNT_CH1, 0x02, 0x20);
    LDC1614_1_WriteReg(RCOUNT_CH2, 0x02, 0x20);
    LDC1614_1_WriteReg(RCOUNT_CH3, 0x02, 0x20);

    LDC1614_1_WriteReg(SETTLECOUNT_CH0, 0x00, 0x1E);
    LDC1614_1_WriteReg(SETTLECOUNT_CH1, 0x00, 0x1E);
    LDC1614_1_WriteReg(SETTLECOUNT_CH2, 0x00, 0x1E);
    LDC1614_1_WriteReg(SETTLECOUNT_CH3, 0x00, 0x1E);

    LDC1614_1_WriteReg(CLOCK_DIVIDERS_CH0, 0x10, 0x03);
    LDC1614_1_WriteReg(CLOCK_DIVIDERS_CH1, 0x10, 0x03); 
    LDC1614_1_WriteReg(CLOCK_DIVIDERS_CH2, 0x10, 0x03);  
    LDC1614_1_WriteReg(CLOCK_DIVIDERS_CH3, 0x10, 0x03); 
     
    LDC1614_1_WriteReg(ERROR_CONFIG, 0x00, 0x01);

    LDC1614_1_WriteReg(DRIVE_CURRENT_CH0, 0x30, 0x00); ///0X70  
    LDC1614_1_WriteReg(DRIVE_CURRENT_CH1, 0x30, 0x00);
    LDC1614_1_WriteReg(DRIVE_CURRENT_CH2, 0x30, 0x00);
    LDC1614_1_WriteReg(DRIVE_CURRENT_CH3, 0x30, 0x00);

    LDC1614_1_WriteReg(MUX_CONFIG, 0xC2, 0x0C);

    LDC1614_1_WriteReg(CONFIG, 0x16, 0x01); 
    //LDC1614_1_WriteReg(CONFIG, 0x14, 0x01); 
    //LDC1614_1_WriteReg(CONFIG, 0x54, 0x01); 
    //LDC1614_1_WriteReg(CONFIG, 0x94, 0x01); 
    //LDC1614_1_WriteReg(CONFIG, 0xD4, 0x01); 

    return device_id;


    }
    else return 0;

}

/*
 *   LDC1614_1_WriteReg
 *   �ú�����������LDC1614�ļĴ���
 *
 *   ������
 *   RegisterAddress 
 *    |__ LDC1614�Ĵ�����ַ
 *   Data
 *    |__ ����д���uint16_t���� 
 */
void LDC1614_1_WriteReg(uint8_t RegisterAddress, uint8_t Data1 , uint8_t Data0)
{
    
    I2C_GenerateSTART(LDC1614_1_I2CX, ENABLE);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_MODE_SELECT)); 

    //���ʹӻ���ַ
	I2C_Send7bitAddress(LDC1614_1_I2CX,LDC1614_1_DEV_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); 

    //дLDC1614�Ĵ�����ַ
	I2C_SendData(LDC1614_1_I2CX, RegisterAddress);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //��Ĵ�����д��������
	I2C_SendData(LDC1614_1_I2CX, Data1);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //��Ĵ�����д��������
	I2C_SendData(LDC1614_1_I2CX, Data0);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
	I2C_GenerateSTOP(LDC1614_1_I2CX, ENABLE);                                        

    
    LDC1614_Delay();
}

/*
 *   LDC1614_1_ReadReg
 *   �ú������ڶ�ȡLDC1614������
 *
 *   ������
 *     RegisterAddress 
 *        |__ LDC1614�Ĵ�����ַ
 *   ����ֵ
          |__ �Ĵ�����ֵ
 *      
 */
uint16_t LDC1614_1_ReadReg(uint8_t RegisterAddress)
{
    uint16_t result;

  	while(I2C_GetFlagStatus(LDC1614_1_I2CX,I2C_FLAG_BUSY));


	I2C_AcknowledgeConfig(LDC1614_1_I2CX, ENABLE);


	I2C_GenerateSTART(LDC1614_1_I2CX,ENABLE);
	while(!I2C_CheckEvent(LDC1614_1_I2CX,I2C_EVENT_MASTER_MODE_SELECT));                 //����ǲ�����ģʽ����ʼλ�Ѿ����� ��ע:��������Ŀ�ľ���Ϊ����ո��¼�

    //���ʹӻ���ַ
	I2C_Send7bitAddress(LDC1614_1_I2CX,LDC1614_1_DEV_ADDR, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//��������ǲ����Ѿ����͵�ַ���

    //дLDC1614�Ĵ�����ַ
	I2C_SendData(LDC1614_1_I2CX, RegisterAddress);
	while (!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));         //��������ǲ����Ѿ��������

	//��Ҫ��ȡ���ݣ����ʱ����Ҫ�任���ݴ��䷽�򣬾�Ҫ�������·�����ʼλ
    //�ٴβ�����ʼ�ź�
	I2C_GenerateSTART(LDC1614_1_I2CX, ENABLE);
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_MODE_SELECT));

    //���ʹӻ���ַ�Ͷ�ȡλ
	I2C_Send7bitAddress(LDC1614_1_I2CX,LDC1614_1_DEV_READ, I2C_Direction_Receiver);         //��һ�η���EEPROM��ַ
	while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


    while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED));            //����ǲ��ǽ��ܵ�������
    result = I2C_ReceiveData(LDC1614_1_I2CX)<<8;

    I2C_AcknowledgeConfig(LDC1614_1_I2CX, DISABLE);         //�ر�Ӧ��
    I2C_GenerateSTOP(LDC1614_1_I2CX,ENABLE);           //ʹ��ֹͣ����

    while(!I2C_CheckEvent(LDC1614_1_I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED));            //����ǲ��ǽ��ܵ�������
    
    result|=I2C_ReceiveData(LDC1614_1_I2CX);
        
	I2C_AcknowledgeConfig(LDC1614_1_I2CX, ENABLE);          //����Ӧ��ģʽ
  
  
    LDC1614_Delay(); 

    return result;
}

/*
 * �������ܣ���LDC1614��ͨ�����
 * �����w
 *       Status - ���ݼĴ���״̬
 *       Regs_Addr - ���ݼĴ�����ַ
 * ��������ֵ�����ֵ��uint32_t��
 */   
uint32_t LDC1614_1_GetResult(uint8_t Regs_Addr) 
{
  uint32_t result,temp;
     
  result= LDC1614_1_ReadReg( Regs_Addr);
  temp  = LDC1614_1_ReadReg( Regs_Addr-1);
  temp=temp&0x0FFF;
  temp=temp<<16;
  result=result|temp;
  
  return result;
}
/*
 * �������ܣ���LDC1614��ͨ�����
 * �����w
 *       *result - ���ָ��
 *       
 * ��������ֵ����
 */  
void LDC1614_1_GetAllResult(uint32_t *result)
{
  uint16_t ret=0;
  
  ret=LDC1614_1_ReadReg(STATUS);
  
  if((ret&STATUS_CH0_UNREADCONV)!=0){
  result[0] = LDC1614_1_ReadReg( DATA_LSB_CH0);
  }
  if((ret&STATUS_CH1_UNREADCONV)!=0){
  result[1] = LDC1614_1_ReadReg( DATA_LSB_CH1);
  }
  if((ret&STATUS_CH2_UNREADCONV)!=0){
  result[2] = LDC1614_1_ReadReg( DATA_LSB_CH2);
  }
  if((ret&STATUS_CH3_UNREADCONV)!=0){
  result[3] = LDC1614_1_ReadReg( DATA_LSB_CH3);
  }


} 


/*
 *   LDC1614_2_Init
 *   �ú������ڳ�ʼ��LDC1614
 *
 *   ������
 *   RegisterAddress 
 *    |__ void
 *   Data
 *    |__ device_id 
 */
uint16_t LDC1614_2_Init(void)
{
  I2C_InitTypeDef i2c_init_param;
  uint16_t device_id;
    int cnt;
    
  //��ʼ��LDC1614_2_I2CX
//  i2c_init_param.I2C_I2Cx = LDC1614_2_I2CX;       //��LDC1614.h���޸ĸ�ֵ
//  i2c_init_param.I2C_IntEnable = FALSE;
//  i2c_init_param.I2C_ICR = LDC1614_SCL_200KHZ;  //�ɸ���ʵ�ʵ�·����SCLƵ��
//  i2c_init_param.I2C_SclPin = LDC1614_2_SCLPIN;   //��LDC1614.h���޸ĸ�ֵ
//  i2c_init_param.I2C_SdaPin = LDC1614_2_SDAPIN;   //��LDC1614.h���޸ĸ�ֵ
//  i2c_init_param.I2C_Isr = NULL;
//  
//  LPLD_I2C_Init(i2c_init_param);
  
  LDC1614_2_WriteReg(RESET_DEV, 0x80, 0x00);
  
  for(cnt =0;cnt<300;cnt++){
    LDC1614_Delay();
  }
  
  //��ȡ�豸ID
  device_id = LDC1614_2_ReadReg(DEVICE_ID);

  //���мĴ�������
  if(device_id==0x3055){
  
  LDC1614_2_WriteReg(RCOUNT_CH0, 0x02, 0x20);    //040d
  LDC1614_2_WriteReg(RCOUNT_CH1, 0x02, 0x20);
  LDC1614_2_WriteReg(RCOUNT_CH2, 0x02, 0x20);
  LDC1614_2_WriteReg(RCOUNT_CH3, 0x02, 0x20);
                                    
  LDC1614_2_WriteReg(SETTLECOUNT_CH0, 0x00, 0x1E);
  LDC1614_2_WriteReg(SETTLECOUNT_CH1, 0x00, 0x1E);
  LDC1614_2_WriteReg(SETTLECOUNT_CH2, 0x00, 0x1E);
  LDC1614_2_WriteReg(SETTLECOUNT_CH3, 0x00, 0x1E);
    
  LDC1614_2_WriteReg(CLOCK_DIVIDERS_CH0, 0x10, 0x03);
  LDC1614_2_WriteReg(CLOCK_DIVIDERS_CH1, 0x10, 0x03); 
  LDC1614_2_WriteReg(CLOCK_DIVIDERS_CH2, 0x10, 0x03);  
  LDC1614_2_WriteReg(CLOCK_DIVIDERS_CH3, 0x10, 0x03); 
     
  //LDC1614_2_WriteReg(ERROR_CONFIG, 0x00, 0x01);
  
  LDC1614_2_WriteReg(DRIVE_CURRENT_CH0, 0x30, 0x00); ///0X70  
  LDC1614_2_WriteReg(DRIVE_CURRENT_CH1, 0x30, 0x00);
  LDC1614_2_WriteReg(DRIVE_CURRENT_CH2, 0x30, 0x00);
  LDC1614_2_WriteReg(DRIVE_CURRENT_CH3, 0x30, 0x00);
    
  //LDC1614_2_WriteReg(MUX_CONFIG, 0xA2, 0x0F);
  LDC1614_2_WriteReg(MUX_CONFIG, 0xC2, 0x0C);

  LDC1614_2_WriteReg(CONFIG, 0x16, 0x01); 
  //LDC1614_2_WriteReg(CONFIG, 0x14, 0x01); 
  //LDC1614_2_WriteReg(CONFIG, 0x54, 0x01); 
  //LDC1614_2_WriteReg(CONFIG, 0x94, 0x01); 
  //LDC1614_2_WriteReg(CONFIG, 0xD4, 0x01); 
  
  return device_id;
  
  
  }
  else return 0;

}

/*
 *   LDC1614_2_WriteReg
 *   �ú�����������LDC1614�ļĴ���
 *
 *   ������
 *   RegisterAddress 
 *    |__ LDC1614�Ĵ�����ַ
 *   Data
 *    |__ ����д���uint16_t���� 
 */
void LDC1614_2_WriteReg(uint8_t RegisterAddress, uint8_t Data1 , uint8_t Data0)
{
  //���ʹӻ���ַ
//  LPLD_I2C_StartTrans(LDC1614_2_I2CX, LDC1614_2_DEV_ADDR, I2C_MWSR);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);
//  
//  //дLDC1614�Ĵ�����ַ
//  LPLD_I2C_WriteByte(LDC1614_2_I2CX, RegisterAddress);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);
//  
//  //��Ĵ�����д��������
//  LPLD_I2C_WriteByte(LDC1614_2_I2CX, Data1);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);
//  
//  //��Ĵ�����д��������
//  LPLD_I2C_WriteByte(LDC1614_2_I2CX, Data0);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);

//  LPLD_I2C_Stop(LDC1614_2_I2CX);

//  LDC1614_Delay();
}

/*
 *   LDC1614_2_ReadReg
 *   �ú������ڶ�ȡLDC1614������
 *
 *   ������
 *     RegisterAddress 
 *        |__ LDC1614�Ĵ�����ַ
 *   ����ֵ
          |__ �Ĵ�����ֵ
 *      
 */
uint16_t LDC1614_2_ReadReg(uint8_t RegisterAddress)
{
  uint16_t result;

  //���ʹӻ���ַ
//  LPLD_I2C_StartTrans(LDC1614_2_I2CX, LDC1614_2_DEV_ADDR, I2C_MWSR);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);

//  //дLDC1614�Ĵ�����ַ
//  LPLD_I2C_WriteByte(LDC1614_2_I2CX, RegisterAddress);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);

//  //�ٴβ�����ʼ�ź�
//  LPLD_I2C_ReStart(LDC1614_2_I2CX);

//  //���ʹӻ���ַ�Ͷ�ȡλ
//  LPLD_I2C_WriteByte(LDC1614_2_I2CX, LDC1614_2_DEV_READ);
//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);


//  //ת������ģʽΪ��
// LPLD_I2C_SetMasterWR(LDC1614_2_I2CX, I2C_MRSW);
// result =LPLD_I2C_ReadByte(LDC1614_2_I2CX);
// LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);
//  //�ر�Ӧ��ACK
// //LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_OFF);//�ر�ACK
//// i2c1->C1 &= 0xF7;
//// i2c1->
// result =LPLD_I2C_ReadByte(LDC1614_2_I2CX)<<8;


//  LPLD_I2C_WaitAck(LDC1614_2_I2CX, I2C_ACK_ON);   

//  LPLD_I2C_Stop(LDC1614_2_I2CX);
//  result =result|LPLD_I2C_ReadByte(LDC1614_2_I2CX);
//  
//  LDC1614_Delay(); 

  return result;
}

/*
 * �������ܣ���LDC1614��ͨ�����
 * �����w
 *       Status - ���ݼĴ���״̬
 *       Regs_Addr - ���ݼĴ�����ַ
 * ��������ֵ�����ֵ��uint32_t��
 */   
uint32_t LDC1614_2_GetResult(uint8_t Regs_Addr) 
{
  uint32_t result,temp;
     
  result= LDC1614_2_ReadReg( Regs_Addr);
  temp  = LDC1614_2_ReadReg( Regs_Addr-1);
  temp=temp&0x0FFF;
  temp=temp<<16;
  result=result|temp;
  
  return result;
}
/*
 * �������ܣ���LDC1614��ͨ�����
 * �����w
 *       *result - ���ָ��
 *       
 * ��������ֵ����
 */  
void LDC1614_2_GetAllResult(uint32_t *result)
{
  uint16_t ret=0;
  
  ret=LDC1614_2_ReadReg(STATUS);
  
  if((ret&STATUS_CH0_UNREADCONV)!=0){
  result[0] = LDC1614_2_ReadReg( DATA_LSB_CH0);
  }
  if((ret&STATUS_CH1_UNREADCONV)!=0){
  result[1] = LDC1614_2_ReadReg( DATA_LSB_CH1);
  }
  if((ret&STATUS_CH2_UNREADCONV)!=0){
  result[2] = LDC1614_2_ReadReg( DATA_LSB_CH2);
  }
  if((ret&STATUS_CH3_UNREADCONV)!=0){
  result[3] = LDC1614_2_ReadReg( DATA_LSB_CH3);
  }


} 
/*
 * ��ʱ����
 */
static void LDC1614_Delay(void){
  int n;
  for(n=1;n<2000;n++) {
    n=n;//asm("nop");
  }
}