; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEcgDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ecg.h"

ClassCount=3
Class1=CEcgApp
Class2=CEcgDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_ECG_DIALOG

[CLS:CEcgApp]
Type=0
HeaderFile=ecg.h
ImplementationFile=ecg.cpp
Filter=N

[CLS:CEcgDlg]
Type=0
HeaderFile=ecgDlg.h
ImplementationFile=ecgDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_UNIT

[CLS:CAboutDlg]
Type=0
HeaderFile=ecgDlg.h
ImplementationFile=ecgDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_ECG_DIALOG]
Type=1
Class=CEcgDlg
ControlCount=15
Control1=IDUARTOPEN,button,1342242817
Control2=IDUARTCLOSE,button,1342242816
Control3=IDC_ECGSCOPE,static,1342308352
Control4=IDC_UARTSTATUS,static,1342308352
Control5=IDC_STATIC,button,1342177287
Control6=IDC_40HzLPF,button,1342242819
Control7=IDC_60HzNotch,button,1342242819
Control8=IDC_50HzHPF,button,1342242819
Control9=IDC_RealTime,button,1342242819
Control10=IDC_BUTTON2,button,1342242816
Control11=IDC_BUTTON3,button,1342242816
Control12=IDC_STATIC,button,1342177287
Control13=IDC_BlueTooth,button,1342177289
Control14=IDC_UART,button,1342177289
Control15=IDC_UNIT,static,1342308352

