#include "RTC.h"
#include "System/TBManager.h"
#include "System/ITC/Signal.h"

RTC Date_Time;
void SetTimeDate()
{
    BYTE *t;
    t = (BYTE *) &Date_Time;
    for(int i=5;i>=0;i--) t[i] = Get_Command_Argu();
}

BYTE I2BCD(BYTE trans)
{
    if ((trans & 0x0f) == 0x0a)
    {
	trans &= 0xf0;
	trans += 0x10;
    }
    return trans;
}

void UpdateRTC()
{
        Send_Signal(SIG_TIME_UPDATE);
        Date_Time.Second++;
	if (Date_Time.Second == 60) 	//SECOND
	{
            Date_Time.Second = 0;
            Date_Time.Minute++;
	    if(Date_Time.Minute == 60)		//MINUTE
	    {
	        Date_Time.Minute = 0;
	        Date_Time.Hour++;
		if(Date_Time.Hour == 24)		//HOUR
		{
	            Date_Time.Hour = 0;
		    Date_Time.Day++;
		    if(Date_Time.Day == 30)		//DAY
		    {
                        Date_Time.Day = 0;
                        Date_Time.Month++;
			if(Date_Time.Month == 12)		//MONTH
			{
			    Date_Time.Month = 0;
			    Date_Time.Year++;	
			    if(Date_Time.Year == 99) //YEAR
			    {
				Date_Time.Year = 0;				
			    }//YEAR						
			}//MOUNTH
		    }//DATE
		}//HOUR
	    }//MINUTE
        }//SECOND
    //  ShowDateTime();
 /*     if (Reservation != 0){
          if (ReservationTimeUP() != 0){
               AddReservationCommand(&Reservation->Intervel);
          }
      }*/
}

void InitialRTC()
{
    Take_Signal(SIG_DATE_TIME_ADJ,SetTimeDate);
    Date_Time.Second = 0;
    Date_Time.Minute = 59;
    Date_Time.Hour = 10;
    Date_Time.Day = 7;
    Date_Time.Month = 6;
    Date_Time.Year = 6;
    TACCR0 = 32767;
    TACTL = TACLR | MC_1 | TASSEL_1;      // ACLK, clear TAR, up mode
    TACCTL0 |= CCIE;
    
}

void SynthethizeDT(ULONG *DateTime)
{
    *DateTime =  Date_Time.Second | ((UINT)Date_Time.Minute << 5) | ((ULONG)Date_Time.Hour << 11) |
                ((ULONG)Date_Time.Day << 16) | ((ULONG)Date_Time.Month << 21) | ((ULONG)Date_Time.Year << 25);
}

#pragma vector = TIMERA0_VECTOR
__interrupt void RTCinterrupt(void)
{
    UpdateRTC();
 //   if (IsCommandEmpty() != 0) LPM1_EXIT; //sorry, this code has to write here...
}

#pragma vector = TIMERA1_VECTOR
__interrupt void R(void)
{
}
