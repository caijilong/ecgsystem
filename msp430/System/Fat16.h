
#include "globol.h"

/*
typedef struct{  //DBR的前62byte...以下註解有**的代表可調
    BYTE  JumpCode[3];                  //固定EB 3C 90
    BYTE  OemName[8];                   //MSDOS5.0
    UINT  BytesPerSector;               //固定512
    BYTE  SectorsPer;
    //每個Cluster有幾個Sector 1:<32M,2:<64M,4:<128M,8:<256M,16:<512M,32:<1G,64:<2G
    UINT  ReservedSectors;              //固定為1
    BYTE  FATNumber;                    //固定為2
    UINT  MaxRoot;                      //根目錄的最大檔案+資料夾數
    UINT  SectorsSmaller32MB;           //++<32MB的sd卡才使用
    BYTE  MediaDescriptor;              //固定為F8(硬碟)
    WORD  SectorsPerFAT;
    //++電腦利用FATNumber+SectorsPerFAT+HiddenSectors..來決定根目錄由哪裡開始
    WORD  SectorsPerTrack;              //固定為63
    WORD  NumberofHeads;                //固定為255
    DWORD HiddenSectors;                //SD卡一般都是0
    DWORD SectorsLarger32MB;            //++>32MB的sd卡才使用
    WORD  PartitionNumber;
    BYTE  ExtendedSignature;
    DWORD PartitionSerialNumber;
    BYTE  VolumeName[11];
    BYTE  FATName[8];
} DBR_Table;*/
/*
typedef struct   //讀取dbr只要這幾個資訊
{
   u16  Byte_Per_Sector;        //0x0B
   u8   Sector_Per_Cluster;     //0x0D
   u16  Reserve_Sector;         //0x0E
   //u8   Num_of_Fat;             //0x10
   //u16  Root_files;             //0x11
   u16  Sector_Per_Fat;         //0x16
   //u32  Hidden_Sector;          //0x1C
} FAT16_DBR;
*/
/*
typedef struct    //單位均為Address
{
   //u16  DBR_Addr;
   u32  Fat1_Addr;
   u32  Fat2_Addr;
   u32  DirTable_Addr;
   u32  Data_Addr;
} FAT_Offset;
*/
typedef struct
{
   u8  *File_Name;   //0x00~0x0A
   u16 Start_Cluster;          //0x1A~0x1B
   u32 Data_Addr;             //(start_cluster*sector per cluster*512)+Data_Addr
   u32 DirTable_Addr;          //在檔案表裡面的位址
   u32 Fat1_Addr;
   u32 Fat2_Addr;
   //u8 File_Attrib;             //0x0B..設定值0x10=資料夾)..0x00=檔案
   u32 File_Size;              //0x1C~0x1F
   u16 LastCluster;            //檔尾的cluster
   u16 Point_Offset;           //檔案大小 mod (每個cluster的大小)
} SDFILE;

typedef struct    //單位均為sectors
{
   u16  Byte_Per_Sector;        //0x0B
   u8   Sector_Per_Cluster;     //0x0D
   u16  Reserve_Sector;         //0x0E
   u16  Sector_Per_Fat;         //0x16
   //FAT16_DBR    DBR;
   //FAT_Offset   Offset;
   u32  Fat1_Addr;
   u32  Fat2_Addr;
   u32  DirTable_Addr;
   u32  Data_Addr;
   u32  cardSize;
} FAT_Struct;

#define ERROR_SD_CARD 1
