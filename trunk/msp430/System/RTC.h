#include "globol.h"


struct sRESERVATION{
    BYTE  Second;
    BYTE  Minute;
    BYTE  Hour;
    BYTE  Day;
    BYTE  Month;
    BYTE  Year;
    ULONG Intervel;
};

typedef struct{
    BYTE  Second;
    BYTE  Minute;
    BYTE  Hour;
    BYTE  Day;
    BYTE  Month;
    BYTE  Year;
}RTC;

#define   RESERVATION struct sRESERVATION

#define   CPU_TICK_1    32767 //1s
#define   CPU_TICK_2    16383 //0.25s
#define   CPU_TICK_3    8191  //0.125s
#define   CPU_TICK_4    4095  //0.0625s
#define   CPU_TICK_5    2047  //0.03125s
#define   CPU_TICK_6    1023  //0.015625s
#define   CPU_TICK_7    511   //0.0078125s
#define   CPU_TICK_8    255   //0.00390625s
#define   CPU_TICK_9    127   //0.001953125s
#define   CPU_TICK_10   31    //0.0009765625s
#define   CPU_TICK_11   15    //0.00048828125s

