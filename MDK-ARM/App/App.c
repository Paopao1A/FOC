#include "task.h"

void VrTask(void)
{
    float Vr_TargetSpeed;
    switch(Motor_CurrentState.state)
    {
        case TO_ZERO:
            break;

        case SPEED_OPENLOOP:
            Vr_TargetSpeed=AdcParaFinal.Vr / 4095.0f * OPENSPD_REF_MAX;//把电位器数值映射到0-200rpm
            spwm.Speed=Vr_TargetSpeed;
            break;
        
        case SPEED_LOOP:
            Vr_TargetSpeed=AdcParaFinal.Vr / 4095.0f * SPD_REF_MAX;//把电位器数值映射到0-2000rpm
            spwm.Speed=Vr_TargetSpeed;
            break;
        
        case CUR_LOOP:
            Vr_TargetSpeed=AdcParaFinal.Vr / 4095.0f * CUR_REF_MAX;//把电位器数值映射到0-2000rpm
            spwm.Speed=Vr_TargetSpeed;
            break;

        default:
            break;
    }
}

void RecevToVofa(void)
{
    if(GET_Vofa_Rx_Flag())//如果接收到数据包
    {
			/*速度闭环，电流开环指令*/
        char* token = strtok(Vofa_Rx_Data,",");
        if(strcmp(token,"SPD")==0)//如果是速度指令
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"SPDki")==0)
            {
                Speed_PID.ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"SPDkp")==0)
            {
                Speed_PID.kp=atof(Value);
            }
            else if(strcmp(Name,"SPDkd")==0)
            {
                Speed_PID.kd=atof(Value);
            }
        }
				/*速度电流闭环指令*/
				else if(strcmp(token,"SPD_CUR")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"SPD_CURki")==0)
            {
                SpdCur_PID.ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"SPD_CURkp")==0)
            {
                SpdCur_PID.kp=atof(Value);
            }
            else if(strcmp(Name,"SPD_CURkd")==0)
            {
                SpdCur_PID.kd=atof(Value);
            }
        }
        else if(strcmp(token,"CUR_Iq")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR_Iqki")==0)
            {
                Cur_Iq_PID.ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR_Iqkp")==0)
            {
                Cur_Iq_PID.kp=atof(Value);
            }
            else if(strcmp(Name,"CUR_Iqkd")==0)
            {
                Cur_Iq_PID.kd=atof(Value);
            }
        }
        else if(strcmp(token,"CUR_Id")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR_Idki")==0)
            {
                Cur_Id_PID.ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR_Idkp")==0)
            {
                Cur_Id_PID.kp=atof(Value);
            }
            else if(strcmp(Name,"CUR_Idkd")==0)
            {
                Cur_Id_PID.kd=atof(Value);
            }
        }
    }

}

void SendToVofa(void)
{
    printf("%f,%f\r\n",SpdCur_PID.target,SpdCur_PID.actual);
}

void HmiTask(void)
{
    uint8_t KeyNum;
    KeyNum=Key_NumGet();

    switch(KeyNum)
    {
        case KEY1_SHORT_PRESS:
		    Motor_Start();					 // 启动PWM
		    Motor_CurrentState.state = SPEED_OPENLOOP; // 无感，速度开环状态
            break;
        
        case KEY2_SHORT_PRESS:
            Motor_Start();					 // 启动PWM
            Motor_CurrentState.state = SPEED_LOOP; // 有感，速度闭环电流开环状态
            break;

        case KEY2_DOUBLE_PRESS:
            Motor_Start();					 // 启动PWM
            Motor_CurrentState.state = CUR_LOOP; // 有感，速度电流闭环状态
            break;
				
				case KEY3_SHORT_PRESS:
						Motor_Stop();
						Motor_CurrentState.state = STOP; // 停止状态

        default:
            break;
    }

}
