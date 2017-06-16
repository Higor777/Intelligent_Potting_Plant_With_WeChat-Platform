#ifndef __KEY_H_
#define	__KEY_H_

#include "sys.h"

#define Key_1 GPIO_ReadInputDataBit(GPIOF,1)
#define Key_2 GPIO_ReadInputDataBit(GPIOF,2)
#define Key_3 GPIO_ReadInputDataBit(GPIOF,3)
#define Key_4 GPIO_ReadInputDataBit(GPIOF,4)



#define Key_1_MASK		 0x01U
#define Key_1_SHIFT    0
#define Key_2_MASK 	   0x02U
#define Key_2_SHIFT 	 1
#define Key_3_MASK 	 	 0x04U
#define Key_3_SHIFT 	 2
#define Key_4_MASK 	   0x08U
#define Key_4_SHIFT  	 3



#define KEY_Scan_STA_Update_MASK	0x80U



//�����ȴ�ʱ��
#define Key_KeepTime	30
//���������ʱ����
#define Key_KeepStep	5

//�Ƿ��ڶ�ʱ���ж�����ɨ�谴��IO״̬
//ɨ�谴��IO״̬Ӧÿ10msһ��
//����ͨ����ʱ����
#define USE_PIT_KEYSCAN 1



void Key_Init(void);
void Key_Scan(void);
uint8_t Key_STA(void);

#endif
