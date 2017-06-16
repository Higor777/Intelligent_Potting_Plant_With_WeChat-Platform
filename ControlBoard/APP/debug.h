#ifndef __DEBUG_H_
#define	__DEBUG_H_

#include <sys.h>

#define ParameterName_Size 15

typedef struct tagParameterNode_Type
{
	float *exterdata;
	float step;
	char name[ParameterName_Size];
	struct tagParameterNode_Type *prepara;
	struct tagParameterNode_Type *nextpara;
}ParameterNode_Type, *ParameterList_Type;


static void Show_Parameter(ParameterNode_Type *para, uint8_t config);
static void Insert_ParameterNode(ParameterNode_Type *insert_node, ParameterList_Type *insert_place, ParameterNode_Type *blank_node);

void ParameterList_Init(void);
void Data_Init(void);
void Show_Dir_Preset(uint8_t config);
void Parameter_Change(void);
void Car_Protect(void);
void Debug_Check(void);
void Car_Start(void);
void Car_Stop(void);
void System_data_reset(void);
#endif
