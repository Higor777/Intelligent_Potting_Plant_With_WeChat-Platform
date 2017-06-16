#include "stm32f10x.h"
#include "iwatchdog.h"

void IWDG_Configuration(void)
{
    //д��0x5555,���������Ź��Ĵ���д�빦��
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    //���Ź�ʱ�ӷ�Ƶ,40K/4=10K HZ(0.1ms)
    IWDG_SetPrescaler(IWDG_Prescaler_4);

    //ι��ʱ�� 50ms/0.1MS=500 .ע�ⲻ�ܴ���0xfff
    IWDG_SetReload(500);

    //ι��
    IWDG_ReloadCounter();

    //ʹ�ܿ��Ź�
    IWDG_Enable();
}

