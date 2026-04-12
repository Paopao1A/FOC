#include "task.h"

uint8_t RotationDirection=Anticlockwise;
void VrTask(void)
{
    float Vr_Target;
    switch(Motor_CurrentState.state)
    {
        case TO_ZERO:
            break;

        case SPEED_OPENLOOP:
            Vr_Target=AdcParaFinal.Vr / 4095.0f * OPENSPD_REF_MAX;//把电位器数值映射到0-200rpm
            spwm.Speed=Vr_Target;
            break;
        
        case SPEED_LOOP:
            Vr_Target=AdcParaFinal.Vr / 4095.0f * SPD_REF_MAX;//把电位器数值映射到0-2000rpm
            spwm.Speed=Vr_Target;
            break;
        
        case CUR_LOOP:
            Vr_Target=AdcParaFinal.Vr / 4095.0f * CUR_REF_MAX;//把电位器数值映射到0-2000rpm
            spwm.Speed=Vr_Target;
            break;
        
        case POS_LOOP:
            Vr_Target=AdcParaFinal.Vr / 4095.0f * POS_REF_MAX;//把电位器数值映射到0-2Π
            if(Vr_Target<=0.2f)
                Vr_Target=0.2f;
            if(Vr_Target>=POS_REF_MAX-0.5f)
                Vr_Target=POS_REF_MAX-0.5f;
			spwm.M_Theta=Vr_Target;
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
        else if(strcmp(token,"CUR0_Iq")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR0_Iqki")==0)
            {
                Cur_Iq_PID[0].ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR0_Iqkp")==0)
            {
                Cur_Iq_PID[0].kp=atof(Value);
            }
            else if(strcmp(Name,"CUR0_Iqkd")==0)
            {
                Cur_Iq_PID[0].kd=atof(Value);
            }
        }
        else if(strcmp(token,"CUR0_Id")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR0_Idki")==0)
            {
                Cur_Id_PID[0].ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR0_Idkp")==0)
            {
                Cur_Id_PID[0].kp=atof(Value);
            }
            else if(strcmp(Name,"CUR0_Idkd")==0)
            {
                Cur_Id_PID[0].kd=atof(Value);
            }
        }

        /*电流位置闭环指令*/
        else if(strcmp(token,"POS_CUR")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"POS_CURki")==0)
            {
                PosCur_PID.ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"POS_CURkp")==0)
            {
                PosCur_PID.kp=atof(Value);
            }
            else if(strcmp(Name,"POS_CURkd")==0)
            {
                PosCur_PID.kd=atof(Value);
            }
        }
        else if(strcmp(token,"CUR1_Iq")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR1_Iqki")==0)
            {
                Cur_Iq_PID[1].ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR1_Iqkp")==0)
            {
                Cur_Iq_PID[1].kp=atof(Value);
            }
            else if(strcmp(Name,"CUR1_Iqkd")==0)
            {
                Cur_Iq_PID[1].kd=atof(Value);
            }
        }
        else if(strcmp(token,"CUR1_Id")==0)
        {
            char* Name=strtok(NULL,",");
            char* Value=strtok(NULL,",");

            if(strcmp(Name,"CUR1_Idki")==0)
            {
                Cur_Id_PID[1].ki=atof(Value);//字符串转浮点数
            }
            else if(strcmp(Name,"CUR1_Idkp")==0)
            {
                Cur_Id_PID[1].kp=atof(Value);
            }
            else if(strcmp(Name,"CUR1_Idkd")==0)
            {
                Cur_Id_PID[1].kd=atof(Value);
            }
        }
    }

}

void SendToVofa(void)
{
    printf("%f,%f\r\n",PosCur_PID.target,PosCur_PID.actual);
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
        
        case KEY2_LONG_PRESS:
            Motor_Start();					 // 启动PWM
            Motor_CurrentState.state = POS_LOOP; // 有感，电流位置闭环状态
            break;
				
		case KEY3_SHORT_PRESS:
			Motor_Stop();
		    Motor_CurrentState.state = STOP; // 停止状态
            break;
        
        case KEY3_DOUBLE_PRESS:
            if(RotationDirection==Anticlockwise)
            {
                RotationDirection=Clockwise;
            }
            else
            {
                RotationDirection=Anticlockwise;
            } // 切换旋转方向
            break;

        default:
            break;
    }

}
