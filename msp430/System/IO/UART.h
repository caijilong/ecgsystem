#include "globol.h"

#define   IN_BUF_LEN   64         //The length must be pow of 2
#define   OUT_BUF_LEN  64         //The length must be pow of 2

//  OUT_BUF_LEN * 11/115200 for evaluating the interval of sending task.
//  if OUT_BUF_LEN equal to 32 byte while baud rate is 115200, the interval set less than 3ms.
//  Since the buffer will fill out when 3ms time up.

//UART error
#define TX_BUFFER_FULL  1
#define RX_BUFFER_FULL  2
#define TX_TIME_OUT     4

//UART Flag
#define TRANSMETTING    1
#define TIME_OUT        2