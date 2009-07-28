#include  "Globol.h"
#include  "System/mmc.h"
#include  "System/Fat16.h"


//u16 Next=0;  //找位址用的

FAT_Struct *FAT16;
UINT File_Error;

void InitFAT16(){
   u32 address=0;
   read_from_sdcard(address,32,sd_buffer);
   FAT16->Byte_Per_Sector=sd_buffer[0x0B]|(u16)sd_buffer[0x0C]<<8;        //0x0B=X
   FAT16->Sector_Per_Cluster=sd_buffer[0x0D];                                  //0x0D=Y
   FAT16->Reserve_Sector=sd_buffer[0x0E]|(u16)(sd_buffer[0x0F]<<8);       //0x0E=A
   //FAT16->Num_of_Fat=sd_buffer[0x10];                                          //0x10=B
   //FAT16->Root_files=sd_buffer[0x11]|(u16)(sd_buffer[0x12]<<8);           //0x11=E
   FAT16->Sector_Per_Fat=sd_buffer[0x16]|(u16)(sd_buffer[0x17]<<8);       //0x16=C
   //FAT16->Hidden_Sector=sd_buffer[0x11]|
   //                        (u32)(sd_buffer[0x12]<<8)|
   //                        (u32)(sd_buffer[0x12]<<16)|
   //                        (u32)(sd_buffer[0x12]<<24);                           //0x1C=D
    //--------------------------------------------------------------------
   FAT16->Fat1_Addr=(u32)(FAT16->Byte_Per_Sector*(FAT16->Reserve_Sector));
   FAT16->Fat2_Addr=(u32)FAT16->Fat1_Addr+((u32)FAT16->Sector_Per_Fat*(u32)FAT16->Byte_Per_Sector);
   FAT16->DirTable_Addr=(u32)FAT16->Fat2_Addr+((u32)FAT16->Sector_Per_Fat*(u32)FAT16->Byte_Per_Sector);
   FAT16->Data_Addr=(u32)FAT16->DirTable_Addr+(512*32);
   //FAT16->DBR_Addr=0;
}

void InitFATwithFormat(u8 *Label){

  InitFAT16();
  //u32 Count=(0xFFFFFFFF&((FAT16->Sector_Per_Fat*FAT16->Num_of_Fat)+32))<<9;
  u32 Count=(0xFFFFFFFF&((FAT16->Sector_Per_Fat*2)+32))<<9;
  //u16 Count=(FAT16->Sector_Per_Fat*FAT16->Num_of_Fat)+32;
  //u32 Start=(u32)(FAT16->Fat1_Addr&0xFFFFFFFF);
  //erase(Start,Count);
  for (int j=0;j<512;j++)sd_buffer[j]=0;
  for (u32 i=FAT16->Fat1_Addr;i<Count;i+=512){
    write_to_sdcard(i,512,sd_buffer);
  }
  //-------------------------------------------------------------------以上刪除所有fat&Dir Table
   u8 FileTable[32]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                     0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x00,0x00,  //StartCluster
                     0x00,0x00,0x00,0x00}; //File_Size
   for(int i=0;i<11;i++){FileTable[i]=Label[i];}
   write_to_sdcard(FAT16->DirTable_Addr,32,FileTable);
   //-------------------------------------------------------------------以上寫入磁碟標籤
   u8 Fatdata[4]={0xF8,0xFF,0xFF,0xFF};
   //-------------------------------------------------------------------以上寫入fat1,2開頭
   write_to_sdcard(FAT16->Fat1_Addr,4,Fatdata);
   write_to_sdcard(FAT16->Fat2_Addr,4,Fatdata);
}


BOOL InitFileSystem()
{
    File_Error = 0;
    FAT16 = (FAT_Struct *)malloc(sizeof(FAT_Struct));
    if (FAT16 == 0) return 0;
    else{
        char *temp;
        temp = (char *)FAT16;
        int i = sizeof(FAT_Struct);
        while(i--) temp[i]=0;
    }
    spi_set_divisor(2);   //設定為clock /2
    BYTE i=1;
    while(initMMC() == MMC_INIT_ERROR && ++i > 0);
    if (i == 0){
        File_Error |= ERROR_SD_CARD;
        return 0;
    }
    FAT16->cardSize =  MMC_ReadCardSize();
    InitFATwithFormat("GENIUSTOMSD");  //刪除所有檔按並重新命名標籤
    return 1;
}


void GetFileInfo(SDFILE *Dfile){
   Dfile->Start_Cluster=sd_buffer[0x1A]|(u16)sd_buffer[0x1B]<<8;          //0x1A~0x1B
   Dfile->Data_Addr=(u32)(Dfile->Start_Cluster-2)*         //((start_cluster-2)*sector per cluster*512)+Data_Addr
                    (u32)FAT16->Byte_Per_Sector*
                    (u32)FAT16->Sector_Per_Cluster+
                    (u32)FAT16->Data_Addr;
   Dfile->Fat1_Addr=(u32)FAT16->Fat1_Addr+(u32)(Dfile->Start_Cluster*2);
   //Start_Cluster從2開始..所以減2..一個CLUSTER暫2BYTE..所以*2..但是扣掉一開始的4BYTE..所以抵銷了
   Dfile->Fat2_Addr=(u32)FAT16->Fat2_Addr+(u32)(Dfile->Start_Cluster*2);
   //Dfile->File_Attrib=sd_buffer[0x0B];             //0x0B..設定值0x10=資料夾)..0x00=檔案
   Dfile->File_Size=sd_buffer[0x1C]|
                    (u32)(sd_buffer[0x1D]<<8)|
                    (u32)(sd_buffer[0x1E]<<16)|
                    (u32)(sd_buffer[0x1F]<<24);              //0x1C~0x1F
}


void SetFileInfo(u16 StartCluster,SDFILE *Dfile,ULONG *DateTime)  //建立新檔的步驟，更新DIR TABLE並寫入SD卡
{
   Dfile->Start_Cluster=StartCluster;          //0x1A~0x1B
   Dfile->Data_Addr=(u32)(Dfile->Start_Cluster-2)*         //((start_cluster-2)*sector per cluster*512)+Data_Addr
                    (u32)FAT16->Byte_Per_Sector*
                    (u32)FAT16->Sector_Per_Cluster+
                    (u32)FAT16->Data_Addr;
   Dfile->Fat1_Addr=(u32)FAT16->Fat1_Addr+(u32)(Dfile->Start_Cluster*2);
   //Start_Cluster從2開始..所以減2..一個CLUSTER暫2BYTE..所以*2..但是扣掉一開始的4BYTE..所以抵銷了
   Dfile->Fat2_Addr=(u32)FAT16->Fat2_Addr+(u32)(Dfile->Start_Cluster*2);
   //Dfile->File_Attrib=0x00;             //0x0B..設定值0x10=資料夾)..0x00=檔案
   Dfile->File_Size=0x00000000;              //0x1C~0x1F
   u16 *temp;
   temp = &StartCluster;
   u8 FileTable[32]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x00,0x00,
                     0x00,0x00,0x00,0x00}; //File_Size
   FileTable[26] = (StartCluster & 0x00FF);
   FileTable[27] = StartCluster >> 8;
   ULONG *temp2;
   temp2 = (ULONG *)(FileTable+0x16);
   *temp2 = *DateTime;
   for(int i=0;i<11;i++){FileTable[i]=Dfile->File_Name[i];}
   write_to_sdcard(Dfile->DirTable_Addr,32,FileTable);
}


BOOL Ffind(u32 StartAddr,SDFILE *Dfile){ //尋找檔案..找到：傳回位址，找不到：傳回可用位址
  //u32 NewAddr=0;
  BOOL find=0;
  int i,j,k;
  //-----------------------------------------找檔案-------------------------------------
  for(i=0;i<32;i++){
    read_from_sdcard(StartAddr+(512*i),512,sd_buffer);  //每次讀取512btye
    for(j=0;j<512;j+=32){  //每32byte為一個檔
      for(k=0;k<11;k++){    //尋找指定檔名與副檔名
        if (Dfile->File_Name[k]!=sd_buffer[j+k]){break;}
        if (k==10&&(Dfile->File_Name[k]==sd_buffer[j+k])){
          find=1;
        } //找到檔案了
      }
      if(find==1){ break; }  //找到檔案了，就跳出
      if(sd_buffer[j]==0){break;}  //如果看到第1個BYTE是0，表示已經搜尋完目前所有的檔案了，也表示目前的位址是可用空間
      StartAddr+=32;
    }
    if(find==1){ break; }  //找到檔案了，就跳出
    if(sd_buffer[j]==0){break;}  //如果看到第1個BYTE是0，表示已經搜尋完目前所有的檔案了，也表示目前的位址是可用空間

  }

  //--------------------------------找完檔案，回傳可用的起始位址----------------------------
  if (find==1){  //找到檔案，並把資訊填進去
    Dfile->DirTable_Addr=StartAddr;
    GetFileInfo(Dfile);
  } //有找到檔案：為檔案的位址
  else{
    //if(NewAddr==0){NewAddr=StartAddr;}  //如果之前找的沒有"已刪除可用空間"，就回傳"未使用的空間"
    Dfile->DirTable_Addr=StartAddr;  //沒找到：以第一個00或已刪除的位址為起始位址
  }
  return find;
}


u16 FindNewCluster(){  //從FAT中尋找可用的"Cluster位址"，設定為已用，並把StartCluster傳回
  u32 StartAddr=0;   //FAT2的起始位址
  u16 ClusterValue;
  u16 Offset=0;  //偏移多少才找到0x0000
  u16 StartCluster=0;  //
  int i,j;
  u8 FindFlag=0;
  for(i=0;i<FAT16->Sector_Per_Fat;i++){ //FAT表最多有(DBR.Sector_Per_Fat)個SECTORS
    read_from_sdcard(FAT16->Fat2_Addr+StartAddr,512,sd_buffer);
    for(j=0;j<511;j+=2){
      ClusterValue=(u16)(sd_buffer[j+1]<<8)|sd_buffer[j];  //取得FAT中BUFFER的直
      if (ClusterValue==0x0000||(ClusterValue>>8==0xE5)){    //如果是可用空間
        u8 EndBuffer[2]={0xFF,0xFF};
        write_to_sdcard(FAT16->Fat1_Addr+StartAddr,2,EndBuffer);  //寫入FFFF...表示要用此空間了
        write_to_sdcard(FAT16->Fat2_Addr+StartAddr,2,EndBuffer);
        FindFlag=1;
        return Offset;  //回傳OFFSET
        break;
      }
      Offset++;
    }
    if(FindFlag==1) break;
    StartAddr+=512;
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------
BOOL FOpen(u8 *fName,SDFILE *Dfile){
  //for(int i=0;i<11;i++) Dfile->File_Name[i]=fName[i];
  Dfile->File_Name=fName;
  BOOL find=Ffind(FAT16->DirTable_Addr,Dfile); //從DATA_TABLE開始找

  if (find==0){  //找到不到檔案，APPEND與否無所謂，均建立一個新檔
    ULONG DateTime;
    SynthethizeDT(&DateTime);
    SetFileInfo(FindNewCluster(),Dfile,&DateTime); //在第一個BYTE為00或E5的欄位中覆蓋，並寫入新檔  <--OK
    Dfile->LastCluster=Dfile->Start_Cluster;  //建新檔的時候..只有一個
    Dfile->Point_Offset=0;
    return 1;
  }else return 0;
  
  //當find=1時..只能讀不能寫..否則會錯亂..不寫在這邊是因為在Ffind中就把檔案資訊載入了
}



void FWrite(u16 Length,unsigned char *buf,SDFILE *File){
   File->File_Size+=Length;     //increase Size
   u32 ClusterOffset=File->Point_Offset+Length;
   //u32 AddrOffset,AddrFat1,AddrFat2;
   File->Point_Offset=File->File_Size%(FAT16->Sector_Per_Cluster*512);
   if (ClusterOffset>=(FAT16->Sector_Per_Cluster*512)){File->LastCluster++;}
   //---------------------------------------------------------------------寫入資料到data區
   write_to_sdcard(File->Data_Addr,Length,buf);
   File->Data_Addr+=Length;   //更改目前的檔案指標
}



void FRead(u16 Length,unsigned char *buf,SDFILE *File,u32 Index){
    u32 Fileoffset=File->Data_Addr-File->File_Size+Index;
    read_from_sdcard(Fileoffset,Length,buf);
}

void FClose(SDFILE *File)
{
  u32 AddrFat1=(u32)(File->Fat1_Addr);  //FAT1的start_cluster所在位址
  u32 AddrFat2=File->Fat2_Addr; //FAT2的start_cluster所在位址
  u16 ClusterOffset=(File->Start_Cluster*2)%512; //起始cluster相對於512倍數位址的偏移量
  long ClusterSize=(File->LastCluster-File->Start_Cluster)*2;  //最後的cluster與起始的cluster在fat的間隔
//---------------------------------------------------------------------寫入檔案大小到dir區
  u32 *SizeTable = &File->File_Size;
/*  u8 SizeTable[4]={(u8)(File->File_Size&0xFF),
                 (u8)(File->File_Size>>8&0xFF),
                 (u8)(File->File_Size>>16&0xFF),
                 (u8)(File->File_Size>>24&0xFF)}; //File_Size*/
  write_to_sdcard(File->DirTable_Addr+28,4,(u8 *)SizeTable);
//---------------------------------------------------------------------寫入鏈結到fat區
  u16 Cluster=File->Start_Cluster;
  for(int i=0;i<=ClusterSize;i+=2){
     if(Cluster==File->LastCluster){
        sd_buffer[i]=0xff; sd_buffer[i+1]=0xff;
     }else{
        sd_buffer[i]=(u8)(Cluster+1)&0xFF;  //此cluster寫入下一個cluster的位址
        sd_buffer[i+1]=(u8)((Cluster+1)>>8&0xFF); //此cluster寫入下一個cluster的位址
     }
     if (i>=(512-ClusterOffset))break;
     Cluster++;
  }
  write_to_sdcard(AddrFat1,(512-ClusterOffset),sd_buffer);  //先將一開始不滿512的部分補齊
  write_to_sdcard(AddrFat2,(512-ClusterOffset),sd_buffer);  //先將一開始不滿512的部分補齊
  ClusterSize-=(512-ClusterOffset);
  AddrFat1+=(512-ClusterOffset);  AddrFat2+=(512-ClusterOffset);
  while(ClusterSize>0){
     u16 size=0;
     for(int i=0;i<512;i+=2){
        sd_buffer[i]=(u8)(Cluster+1)&0xFF;  //此cluster寫入下一個cluster的位址
        sd_buffer[i+1]=(u8)((Cluster+1)>>8&0xFF); //此cluster寫入下一個cluster的位址
        size+=2;
        if(Cluster==File->LastCluster){
           sd_buffer[i]=0xff; sd_buffer[i+1]=0xff;
           break;
        }
        Cluster++;
     }
     write_to_sdcard(AddrFat1,size,sd_buffer);  //先將一開始不滿512的部分補齊
     write_to_sdcard(AddrFat2,size,sd_buffer);  //先將一開始不滿512的部分補齊
     AddrFat1+=512;  AddrFat2+=512;
     ClusterSize-=size;
  }
}


























/*
BOOL Ffind(u32 StartAddr,FAT_Struct *FAT,SDFILE *Dfile){ //尋找檔案..找到：傳回位址，找不到：傳回可用位址
  //u32 NewAddr=0;
  BOOL find=0;
  int i,j;
  //-----------------------------------------找檔案-------------------------------------
  for(i=0;i<512;i++){   //最多有512個檔案
    read_from_sdcard(StartAddr,32,sd_buffer);
    //if(sd_buffer[0]==0xE5){NewAddr=StartAddr;}  //表示是以刪除的檔案，如果找不到檔案時可用
    for(j=0;j<11;j++){    //尋找指定檔名與副檔名
      if (Dfile->File_Name[j]!=sd_buffer[j]){break;}
      if (j==10&&(Dfile->File_Name[j]==sd_buffer[j])){
        find=1;
      } //找到檔案了
    }
    if(find==1){ break; }  //找到檔案了，就跳出
    if(sd_buffer[0]==0){break;}  //如果看到第1個BYTE是0，表示已經搜尋完目前所有的檔案了，也表示目前的位址是可用空間
    StartAddr+=32;
  }
  //--------------------------------找完檔案，回傳可用的起始位址----------------------------
  if (find==1){  //找到檔案，並把資訊填進去
    Dfile->DirTable_Addr=StartAddr;
    GetFileInfo(FAT,Dfile);
  } //有找到檔案：為檔案的位址
  else{
    //if(NewAddr==0){NewAddr=StartAddr;}  //如果之前找的沒有"已刪除可用空間"，就回傳"未使用的空間"
    Dfile->DirTable_Addr=StartAddr;  //沒找到：以第一個00或已刪除的位址為起始位址
  }
  return find;
}*/

/*
void FindNextCluster(FAT_Struct *FAT,u16 StartCluster,u16 *Next){ //傳入目前的CLUSTER..傳回下一個CLUSTER
  u32 StartAddr=FAT16->Fat2_Addr+(u32)(StartCluster*2); //計算出對應CLUSTER所在FAT2中的位址
  read_from_sdcard(StartAddr,2,sd_buffer);
  *Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8;
}
*/
/* 這版有問題..因為記憶體堆疊有限...所以不能用遞回找
void RecycleAllCluster(FAT_Struct *FAT,u16 StartCluster){
  FindNextCluster(FAT,StartCluster,&Next);
  sd_buffer[0x00]=0x00; sd_buffer[0x01]=0x00;
  write_to_sdcard((u32)FAT16->Fat2_Addr+(StartCluster*2),2,sd_buffer);
  write_to_sdcard((u32)FAT16->Fat1_Addr+(StartCluster*2),2,sd_buffer);  //寫入0000...表示要回收目前的空間了
  if(Next!=0xFFFF){  //遞迴自己，直到Next=0xFFFF為止
    RecycleAllCluster(FAT,Next);
  }
}
*/
/*
void RecycleAllCluster(FAT_Struct *FAT,u16 StartCluster){  //這版是用迴圈找..不管檔案再大..都清的完
  u32 StartAddr;
  do{
    StartAddr=FAT16->Fat2_Addr+(StartCluster*2); //計算出對應CLUSTER所在FAT2中的位址
    read_from_sdcard(StartAddr,2,sd_buffer);   //讀取下一個
    Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8; //把位址存到next
    sd_buffer[0x00]=0x00; sd_buffer[0x01]=0x00;
    write_to_sdcard((u32)FAT16->Fat2_Addr+(StartCluster*2),2,sd_buffer);
    write_to_sdcard((u32)FAT16->Fat1_Addr+(StartCluster*2),2,sd_buffer);  //寫入0000...表示要回收目前的空間了
    StartCluster=Next;
  }while (Next!=0xFFFF);
}
*/
/*
void FindLastCluster(FAT_Struct *FAT,SDFILE *Dfile){
  u32 StartAddr;
  do{
    StartAddr=FAT16->Fat2_Addr+(StartCluster*2); //計算出對應CLUSTER所在FAT2中的位址
    read_from_sdcard(StartAddr,2,sd_buffer);   //讀取下一個
    Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8; //把位址存到next
    StartCluster=Next;
  }while (Next!=0xFFFF);

   Dfile->LastCluster;   //檔尾的cluster
   Dfile->Point_Offset=Dfile->File_Size % (FAT16->Byte_Per_Sector*FAT16->Sector_Per_Cluster);
   //檔案大小 mod (每個cluster的大小)
}
*/
