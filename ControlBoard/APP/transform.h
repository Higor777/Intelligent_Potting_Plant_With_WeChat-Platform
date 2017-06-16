#ifndef __TRANSFORM_H
#define __TRANSFORM_H	


#define radius 0.5    //ȫ�������ĵ����������ĵľ���
#define  pi  3.1415926


typedef struct motor{
	float M1;
	float M2;
	float M3;
} motor;


typedef struct vector{
	float x;
	float y;
	float z;
}vector;


typedef struct robot{
	//�ٶ�
    motor single_expected_speed;    //ÿ��������ٶȴ�С
    vector robot_expected_speed;    //���̵������ٶȴ�С
}robot;


void move_base_control(void);
#endif
