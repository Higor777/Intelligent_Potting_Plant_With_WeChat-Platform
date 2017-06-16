#include "sys.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"	
#include <stdarg.h>
#include "os.h"
/*************************************
//USART1_Tx->DMA1_Channel4
//USART1_Rx->DMA1_Channel5
//USART2_Tx->DMA1_Channel7
//USART2_Rx->DMA1_Channel6
//USART3_Tx->DMA1_Channel2
//USART3_Rx->DMA1_Channel3
//UART4_Tx->DMA2_Channel5
//UART4_Rx->DMA2_Channel3
//UART5
*************************************/


DMA_USART_Ch DMA_USART_Ch1;
DMA_USART_Ch DMA_USART_Ch2;
DMA_USART_Ch DMA_USART_Ch3;
DMA_USART_Ch DMA_USART_Ch4;
volatile Queue UART5_TxQueue;
volatile Queue UART5_RxQueue;


#if USART1_TX_EN == 1
uint8_t USART1TxDataBuff[USART1TxMaxBuffLen];
#define USART1_TxQueue  DMA_USART_Ch1.tx_queue
#endif

#if USART1_RX_EN == 1
uint8_t USART1RxDataBuff[USART1RxMaxBuffLen];
#define USART1_RxQueue  DMA_USART_Ch1.rx_queue
#endif

#if USART2_TX_EN == 1
uint8_t USART2TxDataBuff[USART2TxMaxBuffLen];
#define USART2_TxQueue  DMA_USART_Ch2.tx_queue
#endif

#if USART2_RX_EN == 1
uint8_t USART2RxDataBuff[USART2RxMaxBuffLen];
#define USART2_RxQueue  DMA_USART_Ch2.rx_queue
#endif

#if USART3_TX_EN == 1
uint8_t USART3TxDataBuff[USART3TxMaxBuffLen];
#define USART3_TxQueue  DMA_USART_Ch3.tx_queue
#endif

#if USART3_RX_EN == 1
uint8_t USART3RxDataBuff[USART3RxMaxBuffLen];
#define USART3_RxQueue  DMA_USART_Ch3.rx_queue
#endif

#if UART4_TX_EN == 1
uint8_t UART4TxDataBuff[UART4TxMaxBuffLen];
#define UART4_TxQueue  DMA_USART_Ch4.tx_queue
#endif

#if UART4_RX_EN == 1
uint8_t UART4RxDataBuff[UART4RxMaxBuffLen];
#define UART4_RxQueue  DMA_USART_Ch4.rx_queue
#endif

#if UART5_TX_EN == 1
uint8_t UART5TxDataBuff[UART5TxMaxBuffLen];
#endif

#if UART5_RX_EN == 1
uint8_t UART5RxDataBuff[UART5RxMaxBuffLen];
#endif

#define MAX_STRING_LEN  255 
void USART_printf(USART_TypeDef* USARTx, const char *format , ...)
{
    static char send_buf[MAX_STRING_LEN];
    int length = 0;
    va_list ap;
    va_start(ap,format);
    length = vsnprintf(send_buf,MAX_STRING_LEN,format,ap);
    va_end(ap);

    if(length != -1 && length <= MAX_STRING_LEN)
    {
        *(send_buf + length) = 0;
        if(USARTx == UART5)
        {
            USART_Send(USARTx,(uint8_t *)send_buf,length);
        }else
        {
            dma_send(USARTx,(uint8_t *)send_buf,length);
        }
    }else if (length > MAX_STRING_LEN)
    {
        length = sprintf(send_buf,"USART_printf:String length is too long,Length:%d\n",length);
        if(USARTx == UART5)
        {
            USART_Send(USARTx,(uint8_t *)send_buf,length);
        }else
        {
            dma_send(USARTx,(uint8_t *)send_buf,length);
        }
    }else
    {
        length = sprintf(send_buf,"USART_printf:vsnprintf() return -1,filed.\n");
        if(USARTx == UART5)
        {
            USART_Send(USARTx,(uint8_t *)send_buf,length);
        }else
        {
            dma_send(USARTx,(uint8_t *)send_buf,length);
        }
    }
}

void USART_Send(USART_TypeDef* USARTx,uint8_t * data, uint16_t length)
{
    uint16_t i = 0;
    if(USARTx == UART5)
    {
        for(i = 0;i < length;i++)
        {
            while((USARTx->SR&0X40)==0);//ѭ������,ֱ���������   
            USART_SendData(USARTx,*(data+i));
        }
    }else
    {
        dma_send(USARTx,(uint8_t *)data,length);
    }

}

void USART_Module_Init(void)
{
    USART_Queue_Init();
    usart_dma_send_init();
    usart_dma_receive_init();
    USART_Configuration();
}

static void USART_Queue_Init(void)
{
#if USART1_TX_EN == 1
    USART1_TxQueue.data = USART1TxDataBuff;
    USART1_TxQueue.Length = USART1TxMaxBuffLen;
#endif
    
#if USART1_RX_EN == 1
    USART1_RxQueue.data = USART1RxDataBuff;
    USART1_RxQueue.Length = USART1RxMaxBuffLen;
#endif
    
#if USART2_TX_EN == 1
    USART2_TxQueue.data = USART2TxDataBuff;
    USART2_TxQueue.Length = USART2TxMaxBuffLen;
#endif
           
#if USART2_RX_EN == 1 
    USART2_RxQueue.data = USART2RxDataBuff;
    USART2_RxQueue.Length = USART2RxMaxBuffLen;
#endif
    
#if USART3_TX_EN == 1
    USART3_TxQueue.data = USART3TxDataBuff;
    USART3_TxQueue.Length = USART3TxMaxBuffLen;
#endif
            
#if USART3_RX_EN == 1
    USART3_RxQueue.data = USART3RxDataBuff;
    USART3_RxQueue.Length = USART3RxMaxBuffLen;
#endif
    
#if UART4_TX_EN == 1
    UART4_TxQueue.data = UART4TxDataBuff;
    UART4_TxQueue.Length = UART4TxMaxBuffLen;
#endif
          
#if UART4_RX_EN == 1  
    UART4_RxQueue.data = UART4RxDataBuff;
    UART4_RxQueue.Length = UART4RxMaxBuffLen;
#endif
    
#if UART5_TX_EN == 1
    UART5_TxQueue.data = UART5TxDataBuff;
    UART5_TxQueue.Length = UART5TxMaxBuffLen;
#endif
          
#if UART5_RX_EN == 1  
    UART5_RxQueue.data = UART5RxDataBuff;
    UART5_RxQueue.Length = UART5RxMaxBuffLen;
#endif
    
}
//DMA���ڷ��ͳ�ʼ��
//DMA1_Channel7��DMA_Channel_4
//Դ��ַ��tx_queue.data
void usart_dma_send_init(void)
{
#if USART1_TX_EN + USART2_TX_EN + USART3_TX_EN + UART4_TX_EN > 0
    NVIC_InitTypeDef NVIC_InitStructure ;  //�����жϽṹ��  
    DMA_InitTypeDef DMA_InitStructure;		 //����DMA�ṹ��
#endif
    
#if USART1_TX_EN == 1
    /*********************************************************
    USART1
    DMA1Channel4
    *********************************************************/
    //DMA�����ж�����  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal4;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal4;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
    //DMAͨ������  
    DMA_DeInit(DMA1_Channel4);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART1->DR);//����ΪUASRT1
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_TxQueue.data;  
    //dma���䷽��  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    //����DMA�ڴ���ʱ�������ĳ���  .
    DMA_InitStructure.DMA_BufferSize = 0;  
	//����DMA���������ģʽ��һ������ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   

    //����DMA1��ͨ��           
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);    
    //ʹ���ж�  
    DMA_ITConfig(DMA1_Channel4,DMA_IT_TC|DMA_IT_TE,ENABLE); 
#endif

#if USART2_TX_EN == 1
    /*********************************************************
    USART2
    DMA1Channel7
    *********************************************************/
    //DMA�����ж�����  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal7;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal7;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
    //DMAͨ������  
    DMA_DeInit(DMA1_Channel7);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART2->DR);//����ΪUASRT2 
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2_TxQueue.data;  
    //dma���䷽��  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    //����DMA�ڴ���ʱ�������ĳ���  .
    DMA_InitStructure.DMA_BufferSize = 0;  
	//����DMA���������ģʽ��һ������ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   

    //����DMA1��ͨ��           
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);    
    //ʹ���ж�  
    DMA_ITConfig(DMA1_Channel7,DMA_IT_TC|DMA_IT_TE,ENABLE);   
#endif

#if USART3_TX_EN == 1
    /*********************************************************
    USART3
    DMA1Channel2
    *********************************************************/
    //DMA�����ж�����  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal2;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
    //DMAͨ������  
    DMA_DeInit(DMA1_Channel2);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART3->DR);//����ΪUASRT3
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_TxQueue.data;  
    //dma���䷽��  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    //����DMA�ڴ���ʱ�������ĳ���  .
    DMA_InitStructure.DMA_BufferSize = 0;  
	//����DMA���������ģʽ��һ������ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   

    //����DMA1��ͨ��           
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);    
    //ʹ���ж�  
    DMA_ITConfig(DMA1_Channel2,DMA_IT_TC|DMA_IT_TE,ENABLE);
#endif

#if UART4_TX_EN == 1
    /*********************************************************
    UART4
    DMA2Channel5
    *********************************************************/
    //DMA�����ж�����  
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA2_Channal4_5;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA2_Channal4_5;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  //����DMAʱ�� 
    //DMAͨ������  
    DMA_DeInit(DMA2_Channel5);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&UART4->DR);//����ΪUART4
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART4_TxQueue.data;  
    //dma���䷽��  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    //����DMA�ڴ���ʱ�������ĳ���  .
    DMA_InitStructure.DMA_BufferSize = 0;  
	//����DMA���������ģʽ��һ������ 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   

    //����DMA2��ͨ��           
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);    
    //ʹ���ж�  
    DMA_ITConfig(DMA2_Channel5,DMA_IT_TC|DMA_IT_TE,ENABLE);
#endif
}

/********************************************************************* 
*     DMA_USART�������� 
*����:data:�������ݴ�ŵ�ַ 
*     size:���������ֽ��� 
**********************************************************************/  

void dma_send(USART_TypeDef* USARTx,uint8_t *data,uint16_t size)  
{  
#if USART1_TX_EN + USART2_TX_EN + USART3_TX_EN + UART4_TX_EN > 0
    CPU_SR_ALLOC();
#endif
	assert_param(IS_USART_ALL_PERIPH(USARTx));
    if(size<=0)return;
        
	if(USARTx == USART1)
	{
#if USART1_TX_EN == 1
		//�����ݴ��뷢�Ͷ��� 
		queue_putarr(&USART1_TxQueue,data,size);  
		//�ж϶����Ƿ��������ڵ���ʱ�鿴ϵͳ����Ƶ���Ƿ����
		if(queue_full(&USART1_TxQueue))DMA_USART_Ch1.tx_queue_full = 1;
		else DMA_USART_Ch1.tx_queue_full = 0;
		
        //��ֹDMA����ֹͣ����
        if(DMA_USART_Ch1.Flag_Tx_Busy == 1 && ((DMA1_Channel4->CCR & DMA_CCR1_EN) != DMA_CCR1_EN || DMA1_Channel4->CNDTR == 0))
        {
            DMA_USART_Ch1.dmaSendErrorCount ++;
            if(DMA_GetCurrDataCounter(DMA1_Channel4) > 0)
            {
                //��DMA,��������  
                DMA_Cmd(DMA1_Channel4,ENABLE); 
            }else
            {
                //�������  
                DMA_USART_Ch1.Flag_Tx_Busy = 0; 
            }
        }
            
		if(DMA_USART_Ch1.Flag_Tx_Busy == 0 && queue_empty(&USART1_TxQueue) == 0)//���������У����������䣬��DMA���������ֱ�����п�
		{
            OS_CRITICAL_ENTER();//�����ٽ��� 
            DMA_Cmd(DMA1_Channel4,DISABLE); 
			//���ô������ݵ�ַ�볤��  
			if(USART1_TxQueue.tail > USART1_TxQueue.head)
			{
				//����βƫ�����ȶ���ͷ��
				DMA1_Channel4->CMAR = (uint32_t)&USART1_TxQueue.data[USART1_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel4,USART1_TxQueue.tail - USART1_TxQueue.head);  
				USART1_TxQueue.head = USART1_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}else 
			{
				//����βƫ�����ȶ���ͷС
				DMA1_Channel4->CMAR = (uint32_t)&USART1_TxQueue.data[USART1_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel4,USART1TxMaxBuffLen - USART1_TxQueue.head);   
				USART1_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}
			
			//��DMA,��ʼ����  
			DMA_Cmd(DMA1_Channel4,ENABLE); 
			DMA_USART_Ch1.Flag_Tx_Busy = 1;
            OS_CRITICAL_EXIT();	//�˳��ٽ���          
		}	
#endif  
	}else if(USARTx == USART2)
	{
#if USART2_TX_EN == 1
		//�����ݴ��뷢�Ͷ��� 
		queue_putarr(&USART2_TxQueue,data,size);  

		//�ж϶����Ƿ��������ڵ���ʱ�鿴ϵͳ����Ƶ���Ƿ����
		if(queue_full(&USART2_TxQueue))DMA_USART_Ch2.tx_queue_full = 1;
		else DMA_USART_Ch2.tx_queue_full = 0;
		
        //��ֹDMA����ֹͣ����
        if(DMA_USART_Ch2.Flag_Tx_Busy == 1 && ((DMA1_Channel7->CCR & DMA_CCR1_EN) != DMA_CCR1_EN || DMA1_Channel7->CNDTR == 0))
        {
            DMA_USART_Ch2.dmaSendErrorCount ++;
            if(DMA_GetCurrDataCounter(DMA1_Channel7) > 0)
            {
                //��DMA,��������  
                DMA_Cmd(DMA1_Channel7,ENABLE); 
            }else
            {
                //�������  
                DMA_USART_Ch2.Flag_Tx_Busy = 0; 
            }
        }
        
		if(DMA_USART_Ch2.Flag_Tx_Busy == 0 && queue_empty(&USART2_TxQueue) == 0)//���������У����������䣬��DMA���������ֱ�����п�
		{
            OS_CRITICAL_ENTER();//�����ٽ��� 
            DMA_Cmd(DMA1_Channel7,DISABLE); 
			//���ô������ݵ�ַ�볤��  
			if(USART2_TxQueue.tail > USART2_TxQueue.head)
			{
				//����βƫ�����ȶ���ͷ��
				DMA1_Channel7->CMAR = (uint32_t)&USART2_TxQueue.data[USART2_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel7,USART2_TxQueue.tail - USART2_TxQueue.head);  
				USART2_TxQueue.head = USART2_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}else
			{
				//����βƫ�����ȶ���ͷС
				DMA1_Channel7->CMAR = (uint32_t)&USART2_TxQueue.data[USART2_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel7,USART2TxMaxBuffLen - USART2_TxQueue.head);   
				USART2_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}
			
			//��DMA,��ʼ����  
			DMA_Cmd(DMA1_Channel7,ENABLE); 
			DMA_USART_Ch2.Flag_Tx_Busy = 1;
            OS_CRITICAL_EXIT();	//�˳��ٽ���	
		}	
#endif
	}else if(USARTx == USART3)
	{
#if USART3_TX_EN == 1
		//�����ݴ��뷢�Ͷ��� 
		queue_putarr(&USART3_TxQueue,data,size);  
		
		//�ж϶����Ƿ��������ڵ���ʱ�鿴ϵͳ����Ƶ���Ƿ����
		if(queue_full(&USART3_TxQueue))DMA_USART_Ch3.tx_queue_full = 1;
		else DMA_USART_Ch3.tx_queue_full = 0;
		
        //��ֹDMA����ֹͣ����
        if(DMA_USART_Ch3.Flag_Tx_Busy == 1 && ((DMA1_Channel2->CCR & DMA_CCR1_EN) != DMA_CCR1_EN || DMA1_Channel2->CNDTR == 0))
        {
            DMA_USART_Ch3.dmaSendErrorCount ++;
            if(DMA_GetCurrDataCounter(DMA1_Channel2) > 0)
            {
                //��DMA,��������  
                DMA_Cmd(DMA1_Channel2,ENABLE); 
            }else
            {
                //�������  
                DMA_USART_Ch3.Flag_Tx_Busy = 0; 
            }
        }
        
		if(DMA_USART_Ch3.Flag_Tx_Busy == 0 && queue_empty(&USART3_TxQueue) == 0)//���������У����������䣬��DMA���������ֱ�����п�
		{
            OS_CRITICAL_ENTER();//�����ٽ��� 
            DMA_Cmd(DMA1_Channel2,DISABLE); 
			//���ô������ݵ�ַ�볤��  
			if(USART3_TxQueue.tail > USART3_TxQueue.head)
			{
				//����βƫ�����ȶ���ͷ��
				DMA1_Channel2->CMAR = (uint32_t)&USART3_TxQueue.data[USART3_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel2,USART3_TxQueue.tail - USART3_TxQueue.head);  
				USART3_TxQueue.head = USART3_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}else
			{
				//����βƫ�����ȶ���ͷС
				DMA1_Channel2->CMAR = (uint32_t)&USART3_TxQueue.data[USART3_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA1_Channel2,USART3TxMaxBuffLen - USART3_TxQueue.head);   
				USART3_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}
			
			//��DMA,��ʼ����  
			DMA_Cmd(DMA1_Channel2,ENABLE); 
			DMA_USART_Ch3.Flag_Tx_Busy = 1;
            OS_CRITICAL_EXIT();	//�˳��ٽ���         
		}
#endif  
	}else if(USARTx == UART4)
	{
#if UART4_TX_EN == 1
		//�����ݴ��뷢�Ͷ��� 
		queue_putarr(&UART4_TxQueue,data,size);  
		
		//�ж϶����Ƿ��������ڵ���ʱ�鿴ϵͳ����Ƶ���Ƿ����
		if(queue_full(&UART4_TxQueue))DMA_USART_Ch4.tx_queue_full = 1;
		else DMA_USART_Ch4.tx_queue_full = 0;
		
        //��ֹDMA����ֹͣ����
        if(DMA_USART_Ch4.Flag_Tx_Busy == 1 && ((DMA2_Channel5->CCR & DMA_CCR1_EN) != DMA_CCR1_EN || DMA2_Channel5->CNDTR == 0))
        {
            DMA_USART_Ch4.dmaSendErrorCount ++;
            if(DMA_GetCurrDataCounter(DMA2_Channel5) > 0)
            {
                //��DMA,��������  
                DMA_Cmd(DMA2_Channel5,ENABLE); 
            }else
            {
                //�������  
                DMA_USART_Ch4.Flag_Tx_Busy = 0; 
            }
        }
        
		if(DMA_USART_Ch4.Flag_Tx_Busy == 0 && queue_empty(&UART4_TxQueue) == 0)//���������У����������䣬��DMA���������ֱ�����п�
		{
            OS_CRITICAL_ENTER();//�����ٽ��� 
            DMA_Cmd(DMA2_Channel5,DISABLE); 
			//���ô������ݵ�ַ�볤��  
			if(UART4_TxQueue.tail > UART4_TxQueue.head)
			{
				//����βƫ�����ȶ���ͷ��
				DMA2_Channel5->CMAR = (uint32_t)&UART4_TxQueue.data[UART4_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA2_Channel5,UART4_TxQueue.tail - UART4_TxQueue.head);  
				UART4_TxQueue.head = UART4_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}else
			{
				//����βƫ�����ȶ���ͷС
				DMA2_Channel5->CMAR = (uint32_t)&UART4_TxQueue.data[UART4_TxQueue.head];//���õ�ַΪ����ͷ
				DMA_SetCurrDataCounter(DMA2_Channel5,UART4TxMaxBuffLen - UART4_TxQueue.head);   
				UART4_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			}
			
			//��DMA,��ʼ����  
			DMA_Cmd(DMA2_Channel5,ENABLE); 
			DMA_USART_Ch4.Flag_Tx_Busy = 1;
            OS_CRITICAL_EXIT();	//�˳��ٽ���	
		}
#endif
	}

} 
	
/********************************************************************* 
*        DMA1Channel4�����жϴ����� 
**********************************************************************/  
#if USART1_TX_EN == 1
void DMA1_Channel4_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC4) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC4);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel4,DISABLE);  
	
		if(USART1_TxQueue.tail > USART1_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷ��
			DMA1_Channel4->CMAR = (uint32_t)&USART1_TxQueue.data[USART1_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel4,USART1_TxQueue.tail - USART1_TxQueue.head);  
			USART1_TxQueue.head = USART1_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel4,ENABLE); 
		}else if(USART1_TxQueue.tail < USART1_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷС
			DMA1_Channel4->CMAR = (uint32_t)&USART1_TxQueue.data[USART1_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel4,USART1TxMaxBuffLen - USART1_TxQueue.head);   
			USART1_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel4,ENABLE); 
		}else
		{
			//����βƫ���������ͷƫ������ȣ�˵�����пգ��������
			//�������  
			DMA_USART_Ch1.Flag_Tx_Busy = 0; 
		}
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE4) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE4);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel4,DISABLE);  
        if(DMA_GetCurrDataCounter(DMA1_Channel4) > 0)
        {
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel4,ENABLE); 
        }else
        {
            //�������  
			DMA_USART_Ch1.Flag_Tx_Busy = 0; 
        }
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL4);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
} 
#endif
/********************************************************************* 
*        DMA1Channel7�����жϴ����� 
**********************************************************************/  
#if USART2_TX_EN == 1
void DMA1_Channel7_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC7) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC7);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel7,DISABLE);  
	
		if(USART2_TxQueue.tail > USART2_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷ��
			DMA1_Channel7->CMAR = (uint32_t)&USART2_TxQueue.data[USART2_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel7,USART2_TxQueue.tail - USART2_TxQueue.head);  
			USART2_TxQueue.head = USART2_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel7,ENABLE); 
		}else if(USART2_TxQueue.tail < USART2_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷС
			DMA1_Channel7->CMAR = (uint32_t)&USART2_TxQueue.data[USART2_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel7,USART2TxMaxBuffLen - USART2_TxQueue.head);   
			USART2_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel7,ENABLE); 
		}else
		{
			//����βƫ���������ͷƫ������ȣ�˵�����пգ��������
			//�������  
			DMA_USART_Ch2.Flag_Tx_Busy = 0; 
		}
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE7) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE7);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel7,DISABLE);  
        if(DMA_GetCurrDataCounter(DMA1_Channel7) > 0)
        {
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel7,ENABLE); 
        }else
        {
            //�������  
			DMA_USART_Ch2.Flag_Tx_Busy = 0; 
        }
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL7);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
} 
#endif
/********************************************************************* 
*        DMA1Channel2�����жϴ����� 
**********************************************************************/  
#if USART3_TX_EN == 1
void DMA1_Channel2_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC2);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel2,DISABLE);  
	
		if(USART3_TxQueue.tail > USART3_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷ��
			DMA1_Channel2->CMAR = (uint32_t)&USART3_TxQueue.data[USART3_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel2,USART3_TxQueue.tail - USART3_TxQueue.head);  
			USART3_TxQueue.head = USART3_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel2,ENABLE); 
		}else if(USART3_TxQueue.tail < USART3_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷС
			DMA1_Channel2->CMAR = (uint32_t)&USART3_TxQueue.data[USART3_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA1_Channel2,USART3TxMaxBuffLen - USART3_TxQueue.head);   
			USART3_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel2,ENABLE); 
		}else
		{
			//����βƫ���������ͷƫ������ȣ�˵�����пգ��������
			//�������  
			DMA_USART_Ch3.Flag_Tx_Busy = 0; 
		}
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE2) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE2);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel2,DISABLE);  
        if(DMA_GetCurrDataCounter(DMA1_Channel2) > 0)
        {
			//��DMA,��������  
			DMA_Cmd(DMA1_Channel2,ENABLE); 
        }else
        {
            //�������  
			DMA_USART_Ch3.Flag_Tx_Busy = 0; 
        }
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL2);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}  
#endif

/********************************************************************* 
*        DMA2Channel4_5�����жϴ����� 
**********************************************************************/  
#if UART4_TX_EN == 1
void DMA2_Channel4_5_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA2_IT_TC5) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA2_IT_TC5);  
		//�ر�DMA  
		DMA_Cmd(DMA2_Channel5,DISABLE);  
	
		if(UART4_TxQueue.tail > UART4_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷ��
			DMA2_Channel5->CMAR = (uint32_t)&UART4_TxQueue.data[UART4_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA2_Channel5,UART4_TxQueue.tail - UART4_TxQueue.head);  
			UART4_TxQueue.head = UART4_TxQueue.tail;//����ͷ����������β,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA2_Channel5,ENABLE); 
		}else if(UART4_TxQueue.tail < UART4_TxQueue.head)
		{
			//����βƫ�����ȶ���ͷС
			DMA2_Channel5->CMAR = (uint32_t)&UART4_TxQueue.data[UART4_TxQueue.head];//���õ�ַΪ����ͷ
			DMA_SetCurrDataCounter(DMA2_Channel5,UART4TxMaxBuffLen - UART4_TxQueue.head);   
			UART4_TxQueue.head = 0;//����ͷ������������ʼ,����һ���������ڶ��л�������С���ͷ�æʱ���µķ������ݿ��ܽ���ʵ�ʵġ�����ͷ����δ������ɵ����ݸ���
			//��DMA,��������  
			DMA_Cmd(DMA2_Channel5,ENABLE); 
		}else
		{
			//����βƫ���������ͷƫ������ȣ�˵�����пգ��������
			//�������  
			DMA_USART_Ch4.Flag_Tx_Busy = 0; 
		}
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA2_IT_TE5) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA2_IT_TE5);  
		//�ر�DMA  
		DMA_Cmd(DMA2_Channel5,DISABLE);  
        if(DMA_GetCurrDataCounter(DMA2_Channel5) > 0)
        {
			//��DMA,��������  
			DMA_Cmd(DMA2_Channel5,ENABLE); 
        }else
        {
            //�������  
			DMA_USART_Ch4.Flag_Tx_Busy = 0; 
        }
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA2_IT_GL5);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}  
#endif

//DMA���ڽ�����س�ʼ��
void usart_dma_receive_init(void)
{
#if USART1_RX_EN + USART2_RX_EN + USART3_RX_EN + UART4_RX_EN + UART5_RX_EN > 0
		NVIC_InitTypeDef NVIC_InitStructure ;  //�����жϽṹ��  
		DMA_InitTypeDef DMA_InitStructure;		 //����DMA�ṹ��  
#endif
    
#if USART1_RX_EN == 1
		//DMA�����ж�����  
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal5;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal5;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
		//DMAͨ������  
		DMA_DeInit(DMA1_Channel5);   
		//�����ַ  
		DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART1->DR);//����Ϊ����1
		//�ڴ��ַ  
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_RxQueue.data;  
		//dma���䷽��  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
		//����DMA�ڴ���ʱ�������ĳ���  
		DMA_InitStructure.DMA_BufferSize = 1;  
		//����DMA���������ģʽ��һ������  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
		//����DMA���ڴ����ģʽ  
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  
		//���������ֳ�  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
		//�ڴ������ֳ�  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
		//����DMA�Ĵ���ģʽ  
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		//����DMA�����ȼ���  
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
		//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  

		//����DMA1��ͨ��           
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);    
		//ʹ��ͨ��  
		DMA_Cmd(DMA1_Channel5,ENABLE);  
		//ʹ���ж�  
		DMA_ITConfig(DMA1_Channel5,DMA_IT_TC|DMA_IT_TE,ENABLE); 
#endif

#if USART2_RX_EN == 1
		//DMA�����ж�����  
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal6;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal6;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
		//DMAͨ������  
		DMA_DeInit(DMA1_Channel6);   
		//�����ַ  
		DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART2->DR);//����Ϊ����2
		//�ڴ��ַ  
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2_RxQueue.data;  
		//dma���䷽��  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
		//����DMA�ڴ���ʱ�������ĳ���  
		DMA_InitStructure.DMA_BufferSize = 1;  
		//����DMA���������ģʽ��һ������  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
		//����DMA���ڴ����ģʽ  
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  
		//���������ֳ�  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
		//�ڴ������ֳ�  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
		//����DMA�Ĵ���ģʽ  
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		//����DMA�����ȼ���  
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
		//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  

		//����DMA1��ͨ��           
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);    
		//ʹ��ͨ��  
		DMA_Cmd(DMA1_Channel6,ENABLE);  
		//ʹ���ж�  
		DMA_ITConfig(DMA1_Channel6,DMA_IT_TC|DMA_IT_TE,ENABLE);  
#endif

#if USART3_RX_EN == 1
		//DMA�����ж�����  
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA1_Channal3;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA1_Channal3;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //����DMAʱ�� 
		//DMAͨ������  
		DMA_DeInit(DMA1_Channel3);   
		//�����ַ  
		DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&USART3->DR);//����Ϊ����3
		//�ڴ��ַ  
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_RxQueue.data;  
		//dma���䷽��  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
		//����DMA�ڴ���ʱ�������ĳ���  
		DMA_InitStructure.DMA_BufferSize = 1;  
		//����DMA���������ģʽ��һ������  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
		//����DMA���ڴ����ģʽ  
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  
		//���������ֳ�  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
		//�ڴ������ֳ�  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
		//����DMA�Ĵ���ģʽ  
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		//����DMA�����ȼ���  
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
		//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  

		//����DMA1��ͨ��           
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);    
		//ʹ��ͨ��  
		DMA_Cmd(DMA1_Channel3,ENABLE);  
		//ʹ���ж�  
		DMA_ITConfig(DMA1_Channel3,DMA_IT_TC|DMA_IT_TE,ENABLE);  
#endif

#if UART4_RX_EN == 1
        //DMA�����ж�����  
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_DMA2_Channal3;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_DMA2_Channal3;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  //����DMAʱ�� 
		//DMAͨ������  
		DMA_DeInit(DMA2_Channel3);   
		//�����ַ  
		DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&UART4->DR);//����Ϊ����3
		//�ڴ��ַ  
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART4_RxQueue.data;  
		//dma���䷽��  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
		//����DMA�ڴ���ʱ�������ĳ���  
		DMA_InitStructure.DMA_BufferSize = 1;  
		//����DMA���������ģʽ��һ������  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
		//����DMA���ڴ����ģʽ  
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  
		//���������ֳ�  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
		//�ڴ������ֳ�  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
		//����DMA�Ĵ���ģʽ  
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
		//����DMA�����ȼ���  
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
		//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��		
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  

		//����DMA1��ͨ��           
		DMA_Init(DMA2_Channel3, &DMA_InitStructure);    
		//ʹ��ͨ��  
		DMA_Cmd(DMA2_Channel3,ENABLE);  
		//ʹ���ж�  
		DMA_ITConfig(DMA2_Channel3,DMA_IT_TC|DMA_IT_TE,ENABLE);
#endif
}

/********************************************************************* 
*        DMA�����жϴ����� 
**********************************************************************/  
#if USART1_RX_EN == 1
void DMA1_Channel5_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC5) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC5);  
		DMA_Cmd(DMA1_Channel5,DISABLE); 
		
		if(!queue_full(&USART1_RxQueue))//����δ�����������
		{
			queue_tail_inc(&USART1_RxQueue);//����β����1����ֵ�Ѿ���DMA����
			DMA1_Channel5->CMAR = (uint32_t)&USART1_RxQueue.data[USART1_RxQueue.tail];
		}else
		{
			;//����������
		}
		//USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 
		DMA_SetCurrDataCounter(DMA1_Channel5,1);
		//ʹ��DMA  
		DMA_Cmd(DMA1_Channel5,ENABLE); 
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE5) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE5);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel5,DISABLE);  
        DMA1_Channel5->CMAR = (uint32_t)&USART1_RxQueue.data[USART1_RxQueue.tail];
        DMA_SetCurrDataCounter(DMA1_Channel5,1);
        //ʹ��DMA  
        DMA_Cmd(DMA1_Channel5,ENABLE); 
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL5);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}
#endif

/********************************************************************* 
*        DMA�����жϴ����� 
**********************************************************************/  
#if USART2_RX_EN == 1
void DMA1_Channel6_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC6) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC6);  
		DMA_Cmd(DMA1_Channel6,DISABLE); 
		
		if(!queue_full(&USART2_RxQueue))//����δ�����������
		{
			queue_tail_inc(&USART2_RxQueue);//����β����1����ֵ�Ѿ���DMA����
			DMA1_Channel6->CMAR = (uint32_t)&USART2_RxQueue.data[USART2_RxQueue.tail];
		}else
		{
			;//����������
		}
		//USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 
		DMA_SetCurrDataCounter(DMA1_Channel6,1);
		//ʹ��DMA  
		DMA_Cmd(DMA1_Channel6,ENABLE); 
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE6) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE6);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel6,DISABLE); 
        DMA1_Channel6->CMAR = (uint32_t)&USART2_RxQueue.data[USART2_RxQueue.tail];        
        DMA_SetCurrDataCounter(DMA1_Channel6,1);
        //ʹ��DMA  
        DMA_Cmd(DMA1_Channel6,ENABLE); 
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL6);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}  
#endif

/********************************************************************* 
*        DMA�����жϴ����� 
**********************************************************************/ 
#if USART3_RX_EN == 1
void DMA1_Channel3_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TC3);  
		DMA_Cmd(DMA1_Channel3,DISABLE); 
		
		if(!queue_full(&USART3_RxQueue))//����δ�����������
		{
			queue_tail_inc(&USART3_RxQueue);//����β����1����ֵ�Ѿ���DMA����
			DMA1_Channel3->CMAR = (uint32_t)&USART3_RxQueue.data[USART3_RxQueue.tail];
		}else
		{
			;//����������
		}
		//USART_ITConfig(USART3,USART_IT_IDLE,ENABLE); 
		
		DMA_SetCurrDataCounter(DMA1_Channel3,1);
		//ʹ��DMA  
		DMA_Cmd(DMA1_Channel3,ENABLE); 
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA1_IT_TE3) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA1_IT_TE3);  
		//�ر�DMA  
		DMA_Cmd(DMA1_Channel3,DISABLE);  
        DMA1_Channel3->CMAR = (uint32_t)&USART3_RxQueue.data[USART3_RxQueue.tail];
        DMA_SetCurrDataCounter(DMA1_Channel3,1);
        //ʹ��DMA  
        DMA_Cmd(DMA1_Channel3,ENABLE); 
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA1_IT_GL3);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}
#endif

/********************************************************************* 
*        DMA�����жϴ����� 
**********************************************************************/  
#if UART4_RX_EN == 1
void DMA2_Channel3_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(DMA_GetITStatus(DMA2_IT_TC3) != RESET)   
	{  
		//�����־λ  
		DMA_ClearITPendingBit(DMA2_IT_TC3);  
		DMA_Cmd(DMA2_Channel3,DISABLE); 
		
		if(!queue_full(&UART4_RxQueue))//����δ�����������
		{
			queue_tail_inc(&UART4_RxQueue);//����β����1����ֵ�Ѿ���DMA����
			DMA2_Channel3->CMAR = (uint32_t)&UART4_RxQueue.data[UART4_RxQueue.tail];
		}else
		{
			;//����������
		}
		//USART_ITConfig(UART4,USART_IT_IDLE,ENABLE); 
		
		DMA_SetCurrDataCounter(DMA2_Channel3,1);
		//ʹ��DMA  
		DMA_Cmd(DMA2_Channel3,ENABLE); 
	} 
    //DMA��������ж�
    if(DMA_GetITStatus(DMA2_IT_TE3) != RESET)
    {
		//�����־λ  
		DMA_ClearITPendingBit(DMA2_IT_TE3);  
		//�ر�DMA  
		DMA_Cmd(DMA2_Channel3,DISABLE);  
        DMA2_Channel3->CMAR = (uint32_t)&UART4_RxQueue.data[UART4_RxQueue.tail];
        DMA_SetCurrDataCounter(DMA2_Channel3,1);
        //ʹ��DMA  
        DMA_Cmd(DMA2_Channel3,ENABLE); 
    }
    //�����־λ  
    DMA_ClearITPendingBit(DMA2_IT_GL3);  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}
#endif

//��ʼ�� ����1\2\3\4\5
void USART_Configuration(void)
{
#if USART1_TX_EN + USART2_TX_EN + USART3_TX_EN + UART4_TX_EN + UART5_TX_EN + USART1_RX_EN + USART2_RX_EN + USART3_RX_EN + UART4_RX_EN + UART5_RX_EN > 0
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    
#if USART1_TX_EN + USART1_RX_EN > 0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;// PA11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA

	USART_InitStructure.USART_BaudRate =USART1_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);

	//����DMA��ʽ����  
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 	
	//�ж�����  
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART1,USART_IT_TXE,DISABLE);  
	USART_ITConfig(USART1,USART_IT_IDLE,DISABLE); 
    USART_ITConfig(USART1,USART_IT_ORE,ENABLE); 
	USART_Cmd(USART1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_USART1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_USART1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	
#if USART2_TX_EN + USART2_RX_EN > 0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA

	USART_InitStructure.USART_BaudRate =USART2_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2,&USART_InitStructure);

	//����DMA��ʽ����  
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
	//�ж�����  
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);  
	USART_ITConfig(USART2,USART_IT_IDLE,DISABLE); 
    USART_ITConfig(USART2,USART_IT_ORE,ENABLE); 
	USART_Cmd(USART2,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_USART2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_USART2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if USART3_TX_EN + USART3_RX_EN > 0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB	
	
	USART_InitStructure.USART_BaudRate =USART3_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStructure);

	//����DMA��ʽ����  
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE); 	
	//�ж�����  
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART3,USART_IT_TXE,DISABLE);  
	USART_ITConfig(USART3,USART_IT_IDLE,DISABLE); 
    USART_ITConfig(USART3,USART_IT_ORE,ENABLE); 
	USART_Cmd(USART3,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_USART3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_USART3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	
#if UART4_TX_EN + UART4_RX_EN > 0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC	
	
	USART_InitStructure.USART_BaudRate =UART4_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART4,&USART_InitStructure);

	//����DMA��ʽ����  
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE); 	
	//�ж�����  
	USART_ITConfig(UART4,USART_IT_TC,DISABLE);  
	USART_ITConfig(UART4,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(UART4,USART_IT_TXE,DISABLE);  
	USART_ITConfig(UART4,USART_IT_IDLE,DISABLE); 
    USART_ITConfig(UART4,USART_IT_ORE,ENABLE); 
	USART_Cmd(UART4,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_UART4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_UART4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART5_TX_EN + UART5_RX_EN > 0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // PC12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;// PD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD
	
	USART_InitStructure.USART_BaudRate =UART5_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART5,&USART_InitStructure);
	//�ж�����  
	USART_ITConfig(UART5,USART_IT_TC,DISABLE);  
	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);  
	USART_ITConfig(UART5,USART_IT_TXE,DISABLE);  
	USART_ITConfig(UART5,USART_IT_IDLE,DISABLE); 
	USART_Cmd(UART5,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PREEMPT_PRIO_UART5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRIO_UART5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

#if USART1_TX_EN + USART1_RX_EN > 0
void USART1_IRQHandler(void)      //����2�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //�����ж� 
	{
		DMA_USART_Ch1.Flag_Rx_IDLE = 1;
		USART_ITConfig(USART1,USART_IT_IDLE,DISABLE); 
	} 
	
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)  //��������ж�
	{  
		//�رշ�������ж�  
		USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
		USART_ClearITPendingBit(USART1,USART_IT_TC);
	}  
    
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) != RESET) //���ORE��־
    {
        USART_ClearFlag(USART1,USART_FLAG_ORE);
        USART_ReceiveData(USART1);
        //��ֹDMA����ֹͣ����
        if(DMA_GetCurrDataCounter(DMA1_Channel5) == 0)
        {
            DMA_Cmd(DMA1_Channel5,DISABLE); 
            DMA_SetCurrDataCounter(DMA1_Channel5,1);
            DMA_Cmd(DMA1_Channel5,ENABLE); 
            DMA_USART_Ch1.dmaRecErrorCount ++;
        }
    }
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
} 
#endif

#if USART2_TX_EN + USART2_RX_EN > 0
void USART2_IRQHandler(void)      //����2�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //�����ж� 
	{
		DMA_USART_Ch2.Flag_Rx_IDLE = 1;
		USART_ITConfig(USART2,USART_IT_IDLE,DISABLE); 
	} 
	
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)  //��������ж�
	{  
		//�رշ�������ж�  
		USART_ITConfig(USART2,USART_IT_TC,DISABLE);  
		USART_ClearITPendingBit(USART2,USART_IT_TC);
	}  
    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) != RESET) //���ORE��־
    {
        USART_ClearFlag(USART2,USART_FLAG_ORE);
        USART_ReceiveData(USART2);
        //��ֹDMA����ֹͣ����
        if(DMA_GetCurrDataCounter(DMA1_Channel6) == 0)
        {
            DMA_Cmd(DMA1_Channel6,DISABLE); 
            DMA_SetCurrDataCounter(DMA1_Channel6,1);
            DMA_Cmd(DMA1_Channel6,ENABLE); 
            DMA_USART_Ch2.dmaRecErrorCount ++;
        }
    }
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
} 
#endif
 
#if USART3_TX_EN + USART3_RX_EN > 0
void USART3_IRQHandler(void)      //����2�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //�����ж� 
	{
		DMA_USART_Ch3.Flag_Rx_IDLE = 1;
		USART_ITConfig(USART3,USART_IT_IDLE,DISABLE); 
	} 
	
	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)  //��������ж�
	{  
		//�رշ�������ж�  
		USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
		USART_ClearITPendingBit(USART3,USART_IT_TC);
	} 
    if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) != RESET) //���ORE��־
    {
        USART_ClearFlag(USART3,USART_FLAG_ORE);
        USART_ReceiveData(USART3);
        //��ֹDMA����ֹͣ����
        if(DMA_GetCurrDataCounter(DMA1_Channel3) == 0)
        {
            DMA_Cmd(DMA1_Channel3,DISABLE); 
            DMA_SetCurrDataCounter(DMA1_Channel3,1);
            DMA_Cmd(DMA1_Channel3,ENABLE); 
            DMA_USART_Ch3.dmaRecErrorCount ++;
        }
    }  
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
} 
#endif

#if UART4_TX_EN + UART4_RX_EN > 0
void UART4_IRQHandler(void)      //����4�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
    CPU_SR_ALLOC();
	OSIntEnter(); 
    OS_CRITICAL_ENTER();//�����ٽ���    
#endif
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET) //�����ж� 
	{
		USART_ITConfig(UART4,USART_IT_IDLE,DISABLE); 
	} 
	
	if(USART_GetITStatus(UART4, USART_IT_TC) != RESET)  //��������ж�
	{  
		//�رշ�������ж�  
		USART_ITConfig(UART4,USART_IT_TC,DISABLE);  
		USART_ClearITPendingBit(UART4,USART_IT_TC);
	}  
    if(USART_GetFlagStatus(UART4,USART_FLAG_ORE) != RESET) //���ORE��־
    {
        USART_ClearFlag(UART4,USART_FLAG_ORE);
        USART_ReceiveData(UART4);
        //��ֹDMA����ֹͣ����
        if(DMA_GetCurrDataCounter(DMA2_Channel3) == 0)
        {
            DMA_Cmd(DMA2_Channel3,DISABLE); 
            DMA_SetCurrDataCounter(DMA2_Channel3,1);
            DMA_Cmd(DMA2_Channel3,ENABLE); 
        } 
    } 
#if SYSTEM_SUPPORT_UCOS  
    OS_CRITICAL_EXIT();	//�˳��ٽ���	
	OSIntExit();    	//�˳��ж�
#endif
}
#endif

#if UART5_TX_EN + UART5_RX_EN > 0
void UART5_IRQHandler(void)      //����5�жϷ������
{
	uint8_t data;
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //�����ж� 
	{
		data = USART_ReceiveData(UART5);
		queue_put(&UART5_RxQueue,data);
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)  //��������ж�
	{  
		//�رշ�������ж�  
		USART_ITConfig(UART5,USART_IT_TC,DISABLE);  
		USART_ClearITPendingBit(UART5,USART_IT_TC);
	} 
    
    if(USART_GetFlagStatus(UART5,USART_FLAG_ORE) != RESET) //���ORE��־
    {
      USART_ClearFlag(UART5,USART_FLAG_ORE);
      USART_ReceiveData(UART5);
    }
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif    
} 
#endif

 

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((printf_usart->SR&0X40)==0);//ѭ������,ֱ���������   
    printf_usart->DR = (u8) ch;      
	return ch;
}
#endif 



