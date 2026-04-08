#ifndef _FOC_SENSOR_H_
#define _FOC_SENSOR_H_

void PostionToZeroDouble(void);
void Speed_Closeloop(float TargetSpeed);
void SpdCur_Closeloop(float TargetSpeed);
void Current_Closeloop_Test(void);
#endif
