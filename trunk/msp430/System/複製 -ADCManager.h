#include "globol.h"
#include "TBManager.h"

#define ADC_AMOUNT  4

#define VOICE 0     //8192
#define ECG 1       //256
#define PIEZO 2     //64
#define PEDOMETER 3 //32
#define TEMP 4      //1




#define ECG_FREQ Crystal/256
#define TEMP_FREQ Crystal/1
#define VOICE_FREQ Crystal/8192
#define PEDOMETER_FREQ Crystal/1


#define ERROR_CREAT_ADC 1
#define ERROR_DELETE_ADC 1
