// ********************************************************
//
//
//
//            MSP430F169                  MMC Card
//         -----------------          -----------------
//     /|\|              XIN|-   P3.5|                 |
//      | |                 |     |  |                 |
//      --|RST          XOUT|-    |--|Pin4/Vcc         |
//        |                 |        |                 |
//        |                 |        |                 |
//        |            P3.0 |<-------|Pin6/CD          |
//        |            P3.4 |------->|Pin1/CS          |
//        |                 |        |                 |
//        |      P3.2/SIMO1 |------->|Pin2/DIN         |
//        |      P3.1/SOMI1 |<-------|Pin7/DOUT        |
//        |      P3.3/UCLK1 |------->|Pin5/CLK         |
//        |                 |        |                 |
//        |                 |     |--|Pin3/GND         |
//                                |
//                                =
//
//         3.0-mmcCD         Out       0 - card inserted
//         3.1-Dout          Out       0 - off    1 - On -> init in SPI_Init
//         3.2-Din           Inp       0 - off    1 - On -> init in SPI_Init
//         3.3-Clk           Out       -                 -> init in SPI_Init
//         3.4-mmcCS         Out       0 - Active 1 - none Active
//
//
//
//
#ifndef _MMCLIB_C
#define _MMCLIB_C
//
//---------------------------------------------------------------
#include "mmc.h"
#include "System/IO/PortManager.h"
#include "globol.h"
//#define withDMA


// Function Prototypes
char mmcGetResponse(void);
char mmcGetXXResponse(const char resp);
char mmcCheckBusy(void);
//void initSPI (void);
unsigned char spiSendByte(const unsigned char data);
char mmc_GoIdle();

// Varialbes

//---------------------------------------------------------------------

void spi_set_divisor(unsigned int divisor){
  UCTL0 = SWRST;                            // 8-bit SPI Master **SWRST**
  UTCTL0 = CKPL | SSEL1 | SSEL0 | STC;      // SMCLK, 3-pin mode, clock idle low, data valid on rising edge, UCLK delayed
  UBR00 = divisor;                          // 0x02: UCLK/2 (4 MHz), works also with 3 and 4
  UBR10 = divisor >> 8;                     // -"-
  UMCTL0 = 0x00;                            // no modulation
  UCTL0 = CHAR | SYNC | MM | SWRST;         // 8-bit SPI Master **SWRST**
  UCTL0 &= ~SWRST;                          // clear SWRST
  ME1 |= USPIE0;                            // Enable USART1 SPI mode
  while (!(IFG1 & UTXIFG0));                // USART1 TX buffer ready (empty)?
}


// setup usart1 in spi mode
/*void initSPI (void)
{
  UCTL0 = SWRST;                            // 8-bit SPI Master **SWRST**
  UTCTL0 = CKPH | SSEL1 | SSEL0 | STC;      // SMCLK, 3-pin mode, clock idle low, data valid on rising edge, UCLK delayed
  UBR00 = 0x03;                             // 0x02: UCLK/2 (4 MHz), works also with 3 and 4
  UBR10 = 0x00;                             // -"-
  UMCTL0 = 0x00;                            // no modulation
  UCTL0 = CHAR | SYNC | MM | SWRST;         // 8-bit SPI Master **SWRST**
  UCTL0 &= ~SWRST;                          // clear SWRST
  ME1 |= USPIE0;                            // Enable USART1 SPI mode
  while (!(IFG1 & UTXIFG0));                // USART1 TX buffer ready (empty)?
}
*/

// Initialize MMC card
char initMMC (void)
{

  //raise SS and MOSI for 80 clock cycles
  //SendByte(0xff) 10 times with SS high
  //RAISE SS
  int i;
  //P3SEL |=  SIMO0 + SOMI0 + UCLK0;  //0000,1110
  //P3SEL &= ~0x11; //1111,0001
  //P3OUT |= SD_CS;  //0001,0000
  //P3DIR |= SD_CS;  //0001,1010

  //initSPI();
  //initialization sequence on PowerUp
  CS_HIGH();
  P2DIR |=SD_VCC;
  P2OUT &=~SD_VCC;
  for(i=0;i<=9;i++)
    spiSendByte(0xff);

  return (mmc_GoIdle());
}


char mmc_GoIdle()
{
  char response=0x01;
  CS_LOW();

  //Send Command 0 to put MMC in SPI mode
  mmcSendCmd(MMC_GO_IDLE_STATE,0,0x95);
  //Now wait for READY RESPONSE
  if(mmcGetResponse()!=0x01)
    return MMC_INIT_ERROR;

  while(response==0x01)
  {
    CS_HIGH();
    spiSendByte(0xff);
    CS_LOW();
    mmcSendCmd(MMC_SEND_OP_COND,0x00,0xff);
    response=mmcGetResponse();
  }
  CS_HIGH();
  spiSendByte(0xff);
  return (MMC_SUCCESS);
}

// mmc Get Responce
// mmc Get Responce
char mmcGetResponse(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  char response;

  while(i<=64)
  {
    response=spiSendByte(0xff);
    if(response==0x00)break;
    if(response==0x01)break;
    i++;
  }
  return response;
}

char mmcGetXXResponse(const char resp)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  char response;

  while(i<=500)
  {
    response=spiSendByte(0xff);
    if(response==resp)break;
    i++;
  }
  return response;
}
char mmcCheckBusy(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  char response;
  char rvalue;
  while(i<=64)
  {
    response=spiSendByte(0xff);
    response &= 0x1f;
    switch(response)
    {
      case 0x05: rvalue=MMC_SUCCESS;break;
      case 0x0b: return(MMC_CRC_ERROR);
      case 0x0d: return(MMC_WRITE_ERROR);
      default:
        rvalue = MMC_OTHER_ERROR;
        break;
    }
    if(rvalue==MMC_SUCCESS)break;
    i++;
  }
  char k=0;
  do
  {
    response=spiSendByte(0xff);
    k++;
  }while(response==0 && ++k);
  return response;
}

// The card will respond with a standard response token followed by a data
// block suffixed with a 16 bit CRC.

// The card will respond with a standard response token followed by a data
// block suffixed with a 16 bit CRC. mmcReadBlock
char read_from_sdcard(const unsigned long address, const unsigned int count, unsigned char *pBuffer)
{
  unsigned long i = 0;
  char rvalue = MMC_RESPONSE_ERROR;

  // Set the block length to read
  if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
  {
    // SS = LOW (on)
    CS_LOW ();
    // send read command MMC_READ_SINGLE_BLOCK=CMD17
    mmcSendCmd (MMC_READ_SINGLE_BLOCK,address, 0xFF);
    // Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (mmcGetResponse() == 0x00)
    {
      // now look for the data token to signify the start of
      // the data
      if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN)
      {
#ifndef withDMA
        // clock the actual data transfer and receive the bytes; spi_read automatically finds the Data Block
        for (i = 0; i < count; i++)
          pBuffer[i] = spiSendByte(0xff);   // is executed with card inserted
#else
        U0IFG &= ~(URXIFG0 + URXIFG0);      /* clear flags */
        /* Get the block */
        /* DMA trigger is UART1 receive for both DMA0 and DMA1 */
        DMACTL0 &= ~(DMA0TSEL_15 | DMA1TSEL_15);
        DMACTL0 |= (DMA0TSEL_3 | DMA1TSEL_3);
        /* Source DMA address: receive register.  */
        DMA0SA = U0RXBUF_;
        /* Destination DMA address: the user data buffer. */
        DMA0DA = (unsigned short)pBuffer;
        /* The size of the block to be transferred */
        DMA0SZ = (unsigned long)count;
        /* Configure the DMA transfer*/
        DMA0CTL =
          DMAIE   |                         /* Enable interrupt */
          DMADT_0 |                         /* Single transfer mode */
          DMASBDB |                         /* Byte mode */
          DMAEN |                           /* Enable DMA */
          DMADSTINCR1 | DMADSTINCR0;        /* Increment the destination address */

        /* We depend on the DMA priorities here.  Both triggers occur at
           the same time, since the source is identical.  DMA0 is handled
           first, and retrieves the byte.  DMA1 is triggered next, and
           sends the next byte. */
        /* Source DMA address: constant 0xFF (don't increment)*/
        DMA1SA = U0TXBUF_;
        /* Destination DMA address: the transmit buffer. */
        DMA1DA = U0TXBUF_;
        /* Increment the destination address */
        /* The size of the block to be transferred */
        DMA1SZ = (unsigned long)(count-1);
        /* Configure the DMA transfer*/
        DMA1CTL =
          DMADT_0 |                         /* Single transfer mode */
          DMASBDB |                         /* Byte mode */
          DMAEN;                            /* Enable DMA */

        /* Kick off the transfer by sending the first byte */
        U0TXBUF = 0xFF;
//      while (DMA0CTL & DMAEN) _NOP(); //LPM0;  // wait till done
//      while (DMA0CTL & DMAEN) _EINT(); LPM0;  // wait till done
        _EINT();
        LPM0;  // wait till done
#endif
        // get CRC bytes (not really needed by us, but required by MMC)
        spiSendByte(0xff);
        spiSendByte(0xff);
        rvalue = MMC_SUCCESS;
      }
      else
      {
        // the data token was never received
        rvalue = MMC_DATA_TOKEN_ERROR;      // 3
      }
    }
    else
    {
      // the MMC never acknowledge the read command
      rvalue = MMC_RESPONSE_ERROR;          // 2
    }
  }
  else
  {
    rvalue = MMC_BLOCK_SET_ERROR;           // 1
  }
  CS_HIGH ();
  spiSendByte(0xff);
  return rvalue;
}// mmc_read_block



//---------------------------------------------------------------------
//char mmcWriteBlock (const unsigned long address)
char mmcWriteBlock (const unsigned long address, const unsigned int count, unsigned char *pBuffer)
{
  unsigned long i = 0;
  char rvalue = MMC_RESPONSE_ERROR;         // MMC_SUCCESS;
  //  char c = 0x00;   
  P4DIR |= SD_WRITING;    
  P4OUT |= SD_WRITING;
  // Set the block length to read
  if (mmcSetBlockLength (count) == MMC_SUCCESS)   // block length could be set
  {
    // SS = LOW (on)
    CS_LOW ();
    // send write command
    mmcSendCmd (MMC_WRITE_BLOCK,address, 0xFF);

    // check if the MMC acknowledged the write block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE)
    {
      spiSendByte(0xff);
      // send the data token to signify the start of the data
      spiSendByte(0xfe);
      // clock the actual data transfer and transmitt the bytes
//#ifndef withDMA
      for (i = 0; i < count; i++)
        spiSendByte(pBuffer[i]);
/*#else
      // Get the block
      // DMA trigger is UART send
      DMACTL0 &= ~(DMA0TSEL_15);
      DMACTL0 |= (DMA0TSEL_3);
      // Source DMA address: the data buffer.
      DMA0SA = (unsigned short)pBuffer;
      // Destination DMA address: the UART send register.
      DMA0DA = U0TXBUF_;
      // The size of the block to be transferred
      DMA0SZ = (unsigned long)count;
      // Configure the DMA transfer
      DMA0CTL =
        DMAREQ  |                           // start transfer /
        DMADT_0 |                           // Single transfer mode /
        DMASBDB |                           // Byte mode /
        DMAEN |                             // Enable DMA /
        DMASRCINCR1 | DMASRCINCR0;          // Increment the source address /
#endif*/
      // put CRC bytes (not really needed by us, but required by MMC)
      spiSendByte(0xff);
      spiSendByte(0xff);
      // read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
      //   rejected due to a crc error, status 110: Data rejected due to a Write error.
      mmcCheckBusy();
      rvalue = MMC_SUCCESS;
    }
    else
    {
      // the MMC never acknowledge the write command
      rvalue = MMC_RESPONSE_ERROR;   // 2
    }
  }
  else
  {
    rvalue = MMC_BLOCK_SET_ERROR;   // 1
  }
  // give the MMC the required clocks to finish up what ever it needs to do
  //  for (i = 0; i < 9; ++i)
  //    spiSendByte(0xff);

  CS_HIGH ();
  // Send 8 Clock pulses of delay.
  spiSendByte(0xff);
  P4OUT &= ~SD_WRITING;  
  P4DIR &= ~SD_WRITING;   
  return rvalue;
} // mmc_write_block

void write_to_sdcard(const unsigned long address, const unsigned int count, unsigned char *pBuffer){
  //因為寫入的address一定要是512的倍數..所以利用這個函數來做轉換
  //將address mod 512作為偏移量offset..先讀取出address-OFFSET後512byte的資料..並在對應的位置寫入要修改的資料..
  //最後在一次寫入512BYTE
  unsigned long Offset=address%512;
  if((Offset!=0)||(count<512)){  //如果不滿512一樣要取出來蓋掉原本的
    read_from_sdcard(address-Offset,512,sd_buffer);
    for(int i=0;i<count;i++){
      sd_buffer[Offset+i]=pBuffer[i];
    }
    mmcWriteBlock (address-Offset,512,sd_buffer);
  }else {  //address剛好是512的倍數
    mmcWriteBlock (address,512,pBuffer);  //address剛好是512的倍數,大寫也是512..直接寫入
  }
}
//---------------------------------------------------------------------
void mmcSendCmd (const char cmd, unsigned long data, const char crc)
{
  char frame[6];
  char temp;
  int i;
  frame[0]=(cmd|0x40);
  for(i=3;i>=0;i--){
    temp=(char)(data>>(8*i));
    frame[4-i]=(temp);
  }
  frame[5]=(crc);
  for(i=0;i<6;i++)
    spiSendByte(frame[i]);
}


//--------------- set blocklength 2^n ------------------------------------------------------
char mmcSetBlockLength (const unsigned int blocklength)
{
  //  char rValue = MMC_TIMEOUT_ERROR;
  //  char i = 0;
  // SS = LOW (on)
  CS_LOW ();
  // Set the block length to read
  //MMC_SET_BLOCKLEN =CMD16
  mmcSendCmd(MMC_SET_BLOCKLEN,(unsigned long)(blocklength&0x0000FFFF), 0xFF);

  // get response from MMC - make sure that its 0x00 (R1 ok response format)
  if(mmcGetResponse()!=0x00)
  { initMMC();
    mmcSendCmd(MMC_SET_BLOCKLEN, blocklength, 0xFF);
    mmcGetResponse();
  }

  CS_HIGH ();

  // Send 8 Clock pulses of delay.
  spiSendByte(0xff);

  return MMC_SUCCESS;
} // Set block_length


unsigned char spiSendByte(const unsigned char data)
{
  char k=0;
  while ((IFG1&UTXIFG0) ==0 && ++k);   // wait while not ready / for RX
  TXBUF0 = data;         // write
  while ((IFG1 & URXIFG0)==0 && ++k);   // wait for RX buffer (full)
  return (RXBUF0);
}


// Reading the contents of the CSD and CID registers in SPI mode is a simple
// read-block transaction.
char mmcReadRegister (const char cmd_register, const unsigned char length, unsigned char *pBuffer)
{
  char uc = 0;
  char rvalue = MMC_TIMEOUT_ERROR;

  if (mmcSetBlockLength (length) == MMC_SUCCESS)
  {
    CS_LOW ();
    // CRC not used: 0xff as last byte
    mmcSendCmd(cmd_register, 0x000000, 0xff);

    // wait for response
    // in the R1 format (0x00 is no errors)
    if (mmcGetResponse() == 0x00)
    {
      if (mmcGetXXResponse(0xfe)== 0xfe)
        for (uc = 0; uc < length; uc++)
          pBuffer[uc] = spiSendByte(0xff);  //mmc_buffer[uc] = spiSendByte(0xff);
      // get CRC bytes (not really needed by us, but required by MMC)
      spiSendByte(0xff);
      spiSendByte(0xff);
      rvalue = MMC_SUCCESS;
    }
    else
      rvalue = MMC_RESPONSE_ERROR;
    // CS = HIGH (off)
    CS_HIGH ();

    // Send 8 Clock pulses of delay.
    spiSendByte(0xff);
  }
  CS_HIGH ();
  return rvalue;
} // mmc_read_register

unsigned long MMC_ReadCardSize(void)
{
  // Read contents of Card Specific Data (CSD)

  unsigned long MMC_CardSize;
  unsigned short i,      // index
                 j,      // index
                 b,      // temporary variable
                 response,   // MMC response to command
                 mmc_C_SIZE;

  unsigned char mmc_READ_BL_LEN,  // Read block length
                mmc_C_SIZE_MULT;

  CS_LOW ();

  spiSendByte(MMC_READ_CSD);   // CMD 9
  for(i=4; i>0; i--)      // Send four dummy bytes
    spiSendByte(0);
  spiSendByte(0xFF);   // Send CRC byte

  response = mmcGetResponse();

  // data transmission always starts with 0xFE
  b = spiSendByte(0xFF);

  if( !response )
  {
    while (b != 0xFE) b = spiSendByte(0xFF);
    // bits 127:87
    for(j=5; j>0; j--)          // Host must keep the clock running for at
      b = spiSendByte(0xff);


    // 4 bits of READ_BL_LEN
    // bits 84:80
    b =spiSendByte(0xff);  // lower 4 bits of CCC and
    mmc_READ_BL_LEN = b & 0x0F;

    b = spiSendByte(0xff);

    // bits 73:62  C_Size
    // xxCC CCCC CCCC CC
    mmc_C_SIZE = (b & 0x03) << 10;
    b = spiSendByte(0xff);
    mmc_C_SIZE += b << 2;
    b = spiSendByte(0xff);
    mmc_C_SIZE += b >> 6;

    // bits 55:53
    b = spiSendByte(0xff);

    // bits 49:47
    mmc_C_SIZE_MULT = (b & 0x03) << 1;
    b = spiSendByte(0xff);
    mmc_C_SIZE_MULT += b >> 7;

    // bits 41:37
    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

  }

  for(j=4; j>0; j--)          // Host must keep the clock running for at
    b = spiSendByte(0xff);  // least Ncr (max = 4 bytes) cycles after
                               // the card response is received
  b = spiSendByte(0xff);
  CS_LOW ();

  MMC_CardSize = (mmc_C_SIZE + 1);
  // power function with base 2 is better with a loop
  // i = (pow(2,mmc_C_SIZE_MULT+2)+0.5);
  for(i = 2,j=mmc_C_SIZE_MULT+2; j>1; j--)
    i <<= 1;
  MMC_CardSize *= i;
  // power function with base 2 is better with a loop
  //i = (pow(2,mmc_READ_BL_LEN)+0.5);
  for(i = 2,j=mmc_READ_BL_LEN; j>1; j--)
    i <<= 1;
  MMC_CardSize *= i;

  return (MMC_CardSize);

}

#ifdef withDMA
  #ifdef __IAR_SYSTEMS_ICC__
    #if __VER__ < 200
      interrupt[DACDMA_VECTOR] void DMA_isr(void)
    #else
      #pragma vector = DACDMA_VECTOR
      __interrupt void DMA_isr(void)
    #endif
  #endif

  #ifdef __CROSSWORKS__
    void DMA_isr(void)   __interrupt[DACDMA_VECTOR]
  #endif

  #ifdef __TI_COMPILER_VERSION__
    __interrupt void DMA_isr(void);
    DMA_ISR(DMA_isr)
    __interrupt void DMA_isr(void)
  #endif
  {
    DMA0CTL &= ~(DMAIFG);
    LPM3_EXIT;
  }
#endif


//---------------------------------------------------------------------
#endif /* _MMCLIB_C */

/*
void write_to_sdcard(unsigned long addr,unsigned int amo,unsigned char *buf){
  unsigned long address=addr;
  unsigned int amount=amo;
  mmcReadBlock(address,amount,buf);
}
void read_from_sdcard(unsigned long addr,unsigned int amo,unsigned char *buf){
  const unsigned long address=addr;
  const unsigned int amount=amo;
  mmcWriteBlock(address,amount,buf);
}
*/

/*
char mmc_ping(void)
{
  if (!(P5IN & 0x01))
    return (MMC_SUCCESS);
  else
    return (MMC_INIT_ERROR);
}
*/
