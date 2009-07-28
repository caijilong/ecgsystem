// ecgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ecg.h"
#include "ecgDlg.h"
#include "math.h"
#include "Globol.h"
#include "DlgAnylsisSel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcgDlg dialog

CEcgDlg::CEcgDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEcgDlg::IDD, pParent)
	, m_ComWord(_T(""))
	, m_ComNum(1)
	, m_Threshold(1)
	, beat_interval(0)
	, m_BeatAverageTimes(15)
	, m_rs232_status(_T("Disconnect!"))
	, m_RecordName(_T("Hawkeye"))
	, m_RecordSex(_T("Male"))
	, m_RecordAge(_T("25"))
	, m_SamplingFreq(360)
	, m_Resolution(12)
	, m_Male(TRUE)
	, m_QT(0)
	, m_ST(0)
	, m_QRS(0)
	, m_PS(0)
	, m_PR(0)
	, m_60HzHP(FALSE)
	, m_005HP(FALSE)
	, m_40LP(FALSE)
	, m_Freq(_T("0"))
{
	//{{AFX_DATA_INIT(CEcgDlg)


	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	sample_count = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	PeakArrary = 0;
}

void CEcgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEcgDlg)

	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_UARTSTATUS, m_rs232_status);
	DDX_Text(pDX, IDC_EDIT3, m_ComWord);
	DDX_Text(pDX, IDC_EDIT2, m_ComNum);
	DDX_Control(pDX, IDC_EDIT4, m_BeatPerSec);
	DDX_Control(pDX, IDC_EDIT5, m_BeatNum);
	DDX_Text(pDX, IDC_EDIT8, m_Threshold);
	DDX_Text(pDX, IDC_EDIT9, beat_interval);
	DDX_Control(pDX, IDC_TAB_SPEC_FUNC, m_SpecialTab);
	DDX_Control(pDX, IDC_ECGSCOPE, ECGViewer);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_TIMES, m_BeatAverageTimes);
	//	DDX_Control(pDX, IDC_SLIDER1, m_WaveSlideBar);
	DDX_Control(pDX, IDC_SLIDER1, m_WaveSlideBar);

	DDX_Text(pDX, IDC_EDIT7, m_RecordName);
	DDX_Text(pDX, IDC_EDIT11, m_RecordAge);
	DDX_Text(pDX, IDC_EDIT12, m_SamplingFreq);
	DDX_Text(pDX, IDC_EDIT13, m_Resolution);
	DDX_Check(pDX, IDC_40HzLPF, m_Male);
	DDX_Control(pDX, IDC_EDIT1, m_BeatInterval);
	DDX_Text(pDX, IDC_EDIT10, m_QT);
	DDX_Text(pDX, IDC_EDIT14, m_ST);
	DDX_Text(pDX, IDC_EDIT15, m_QRS);
	DDX_Text(pDX, IDC_EDIT16, m_PS);
	DDX_Text(pDX, IDC_EDIT17, m_PR);
	DDX_Check(pDX, IDC_60HzNotch, m_60HzHP);
	DDX_Check(pDX, IDC_50HzHPF, m_005HP);
	DDX_Check(pDX, IDC_40HzLPF, m_40LP);

	DDX_Text(pDX, IDC_FREQ, m_Freq);
}

BEGIN_MESSAGE_MAP(CEcgDlg, CDialog)
	//{{AFX_MSG_MAP(CEcgDlg)
	ON_WM_HSCROLL()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_LTX, OnBnClickedButtonLtx)
	ON_BN_CLICKED(IDC_BUTTON_SendCom, OnBnClickedButtonSendcom)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON4, &CEcgDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CEcgDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CEcgDlg::OnBnClickedButton5)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP


	ON_BN_CLICKED(IDC_SEX_SEL, &CEcgDlg::OnBnClickedSexSel)
	ON_BN_CLICKED(IDC_RADIO2, &CEcgDlg::OnBnClickedRadio2)

	ON_BN_CLICKED(IDC_40HzLPF, &CEcgDlg::OnBnClicked40hzlpf)
	ON_BN_CLICKED(IDC_60HzNotch, &CEcgDlg::OnBnClicked60hznotch)
	ON_BN_CLICKED(IDC_50HzHPF, &CEcgDlg::OnBnClicked50hzhpf)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SPEC_FUNC, &CEcgDlg::OnTcnSelchangeTabSpecFunc)
	ON_EN_CHANGE(IDC_EDIT12, &CEcgDlg::OnEnChangeEdit12)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcgDlg message handlers

BOOL CEcgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	 f = new CFont();//顯示平均心跳的字型
	 
	 f->CreateFont(20,                        // nHeight
		   0,                         // nWidth
		   0,                         // nEscapement
		   0,                         // nOrientation
		   FW_BOLD,                 // nWeight
		   TRUE,                     // bItalic
		   FALSE,                     // bUnderline
		   0,                         // cStrikeOut
		   ANSI_CHARSET,              // nCharSet
		   OUT_DEFAULT_PRECIS,        // nOutPrecision
		   CLIP_DEFAULT_PRECIS,       // nClipPrecision
		   DEFAULT_QUALITY,           // nQuality
		   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		   _T("Arial"));                 // lpszFac
	 m_BeatPerSec.SetFont(f);
	 m_BeatNum.SetFont(f);
	 m_BeatPerSec.SetWindowText("0");
	 m_BeatNum.SetWindowText("0");
	 m_BeatInterval.SetWindowText("0");

	 GetDlgItem(IDC_EDIT4)->SetFont(f);
	 GetDlgItem(IDC_EDIT5)->SetFont(f);
	 GetDlgItem(IDC_EDIT1)->SetFont(f);

	
	 m_SpecialTab.InsertItem(VIEW_ALOG_DEBUG,_T("Debug"));
	 m_SpecialTab.InsertItem(VIEW_HRV_TIME,_T("HRV_time"));
	 m_SpecialTab.InsertItem(VIEW_HRV_FREQ,_T("HRV_Freq"));
	 m_SpecialTab.InsertItem(VIEW_FFT,_T("FFT"));
	 m_SpecialTab.InsertItem(VIEW_CAP_ENVE,_T("Cap&Enve"));
	 m_SpecialTab.InsertItem(VIEW_ECG_TEMPLETE,_T("ECG aver."));

	 m_WaveSlideBar.SetRange(0,0);
	 
	 ECG = 0;
	 FFT = 0;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEcgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEcgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEcgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CEcgDlg::SignalProcViewInit()
{
	CRect rect_temp;
	if (ECG == 0){
		CWnd* pWnd = GetDlgItem(IDC_ECGSCOPE);  
		pWnd->GetClientRect(&rect_top_view);

		pWnd = GetDlgItem(IDC_TAB_SPEC_FUNC);  
		pWnd->GetClientRect(&rect_temp);
		rect_low_view.SetRect(rect_top_view.left,
							  rect_temp.top+25,
							  rect_top_view.right,
							  rect_temp.bottom);//caculate the rect of lower viewer
	//	rect_low_view.CopyRect(&rect_temp);
		rect_low_view.MoveToX(3);
//--------------------------------以下創造各種波形的物件------------------------------
		ECG = new CDrawWave(&rect_top_view,RGB(0,0,0),TYPE_SIGN);
		HR_FreqDomain = new CDrawWave(&rect_low_view,RGB(0,255,0),TYPE_UNSIGN);
		HFComponent = new CDrawWave(&rect_low_view,RGB(0,0,0),TYPE_UNSIGN);
		Peak = new CDrawWave(&rect_top_view,RGB(0,0,255),TYPE_SIGN);
		
		SlidingWindowMax = new CDrawWave(&rect_low_view,RGB(50,0,255),TYPE_UNSIGN);
		RRInterval = new CDrawWave(&rect_low_view,RGB(0,0,255),TYPE_UNSIGN);
		TempleteMatch = new CDrawWave(&rect_low_view,RGB(255,0,0),TYPE_UNSIGN);
		TimeSlot = new CDrawWave(&rect_low_view,RGB(50,255,50),TYPE_UNSIGN);
		SlidingWindowThreshold = new CDrawWave(&rect_low_view,RGB(255,0,0),TYPE_UNSIGN);

		Beat_aver = new CECGFeature(&rect_low_view,RGB(0,0,0),TYPE_SIGN,m_SamplingFreq);
		
		Normalize = 1 << (m_Resolution - 1);
		SigProc = new CECGsp(m_SamplingFreq/DOWNSAMPLING,ECG->wave_rect.Width(),m_BeatAverageTimes,Normalize);

		SigProc->PeakDect->ChangeThreshold(m_Threshold);

		 filter = new CFilter(m_SamplingFreq);			//初始化濾波器
		 filter->LoadFilterCoeff(m_SamplingFreq);

		FFT = new CDrawWave(&rect_low_view,RGB(255,0,0),TYPE_UNSIGN);
		Capstrum = new CDrawWave(&rect_low_view,RGB(0,255,0),TYPE_UNSIGN);
		Envenlope = new CDrawWave(&rect_low_view,RGB(10,128,100),TYPE_UNSIGN);

		PreviousPeakPos = 0;
		return 1;
	}else{

		delete SigProc;
		SigProc = new CECGsp(m_SamplingFreq/DOWNSAMPLING,rect_top_view.Width(),m_BeatAverageTimes,Normalize);

		return 0;
	}
}
void CEcgDlg::Uartopen() 
{
	int status;



	UpdateData(true);
	status = uart.Open("COM" + ComPort_Number + ":", baudrate , 8, NOPARITY, ONESTOPBIT, GENERIC_ALL);

	if (status == 1)
	{
		Queue_buffer = new BYTE[BUFFER_MAX];

		SignalProcViewInit();
		sample_count = 0;
		UART_Enable = TRUE;
		m_rs232_status = "Connect!";
		in_data = 0;
		out_data=0;
		WaitingByte = 0;
		isTimerActive = 0;
		m_Timer = SetTimer(1, 5, 0);
	}
	else
	{
		UART_Enable = false;
		m_rs232_status = "Disconnect!";
	}
	UpdateData(false);
}

void CEcgDlg::Uartclose() 
{
	// TODO: Add your control notification handler code here
	UART_Enable = 0;
	uart.Close();
	UART_Enable = FALSE;
	m_rs232_status = "Disconnect!";
	KillTimer(m_Timer);
	UpdateData(false);


}
void CEcgDlg::AnalyseSignal(int samp,int Task,int Mode)
{
	if (Mode == RT){			//Real time mode 心電圖資料以一個sample為主的輸入並分析
		UpdateData(true);
		sample_count++;			//計算sample的總數，用來計時
		SigProc->PeakDect->ChangeThreshold(m_Threshold);//m_Threshold為設定的閥值，但目前沒用
		SigProc->PeakDect->InsertValue(samp);//丟入sample到R波偵測程序

//		m_Threshold = SigProc->PeakDect->MaxMinThreshold;
	
		if (Task & ANA_TASK_BEAT_DETECT){

			int BeatPos = SigProc->PeakDect->GetPeakPosition();//減查看是否有R波出現

			if (BeatPos > -1){			//若沒有，就傳回-1，有，傳回目前R波所在位置

			Peak->ChangeValue(-1,BeatPos);//將此位置顯示

			RRInterval->SetGrid(CScale(1,RRInterval->wave_rect.Width()-1,0,1),100,0.2,RGB(255,124,45),"Num","s");//設定格點
			RRInterval->InsertValue(beat_interval);//插入要顯示的心率，就是HRV_time裡出現的波形
			recorder.RRInterval.Add(beat_interval*1000);//將心率插入RR interval
			UINT PresentPeakPos;

			PresentPeakPos = sample_count - SigProc->PeakDect->GetPeakDeviation();//由於抓到R波時，目前的時間已超過實際R波出現後0.3秒左右，因此目前時間與實際R波的時間有一個差值
			this->beat_interval = SigProc->PeakDect->BeatInterval;//取得平均心跳

			recorder.BeatNum++;

			if (this->recorder.peak_pos_ary.GetSize() == 0)	//因為使用archive來存取檔案，只能用WORD的格式來存，因此只能存2byte，所以取R波位置的差值就能夠以2byte來記錄R波在第幾個sample出現
				this->recorder.peak_pos_ary.Add(PresentPeakPos);//第一個R波位置是實際的位置
			else 
				this->recorder.peak_pos_ary.Add(WORD(PresentPeakPos - PreviousPeakPos));//其他的都是差值


			PreviousPeakPos = PresentPeakPos;//紀錄這次R波發生的位置，才能在下一次相減

			if (recorder.peak_pos_ary.GetSize() != SigProc->PeakDect->BeatNumber)
				SigProc->PeakDect->BeatNumber = 0;
			//this->recorder.peak_pos_ary.Add(sample_count - SigProc->PeakDect->GetPeakDeviation());

				if ((Task & ANA_TASK_BEAT_DETECT_NODIS) == 0){//如果設定ANA_TASK_BEAT_DETECT_NODIS，代表偵測心跳但不顯示
					CString beat;
					beat.Format("%d",int(floor(SigProc->PeakDect->BeatPerMin)));
					m_BeatPerSec.SetWindowText(beat);

					beat.Format("%d",SigProc->PeakDect->BeatNumber);
					m_BeatNum.SetWindowText(beat);

					beat.Format("%fs",SigProc->PeakDect->BeatInterval_ms);
					m_BeatInterval.SetWindowText(beat);
				}

			}
			Peak->InsertValue(1);//與前面的Peak->InsertValue(-1)組成一條粗線，代表R波位置
		}

			if (Task & ANA_TASK_DEBUG_VIEW){//插入每一個波形的值，並設定格點
				ECG->SetGrid(CScale(float(sample_count)/m_SamplingFreq,float(sample_count)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45),"s","mV");
				ECG->InsertValue(float(samp)/Normalize);

		//		TimeSlot->SetGrid(CScale(float(Pos)/m_SamplingFreq,float(Pos)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45));
				TimeSlot->DumpValue((float *)SigProc->PeakDect->TraceWindowSlot,SigProc->PeakDect->max_count);

		//		SlidingWindowMax->SetGrid(CScale(float(Pos)/m_SamplingFreq,float(Pos)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45));
				SlidingWindowMax->InsertValue(float(SigProc->PeakDect->DiffFrontAdd)/8000);

		//		HFComponent->SetGrid(CScale(float(Pos)/m_SamplingFreq,float(Pos)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45));
				HFComponent->InsertValue(float(SigProc->PeakDect->HighFreqComp)/8000);

		//		TempleteMatch->SetGrid(CScale(float(Pos)/m_SamplingFreq,float(Pos)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45));
				TempleteMatch->InsertValue(abs(float(SigProc->PeakDect->Decision))/20);

				SlidingWindowThreshold->SetGrid(CScale(float(sample_count)/m_SamplingFreq,float(sample_count)/m_SamplingFreq + (SigProc->PeakDect->max_count/this->m_SamplingFreq),0,8000),0.2,2000,RGB(255,200,150),"s","mV");
				SlidingWindowThreshold->InsertValue(float(SigProc->PeakDect->SlidingWindowThreshold)/8000);
				
				//DecisionThreshold->InsertValue(abs(float(SigProc->PeakDect->DecisionThreshold))/20);
			}

			if (Task & ANA_TASK_TEMPLETE){//顯示平均新店圖
				float *temp_aver,f;

				if (SigProc->PeakDect->Is_Templet_Finish() == 1) {

				//	temp_aver = new float[SigProc->PeakDect->Templet_max_count];
					int j;
/*
					Beat_aver->DisplaySlotNum = 2;
					Beat_aver->DisplaySlot = Beat_aver->wave_rect.Width()/Beat_aver->DisplaySlotNum;
		//			for(j=0;j<SigProc->PeakDect->Templet_max_count;j++) temp_aver[j] = 0;
					for (float i=0;i<SigProc->PeakDect->Templet_max_count; i+=(float(SigProc->PeakDect->Templet_max_count)/Beat_aver->DisplaySlot)){
						j = SigProc->PeakDect->TempleteIndex-1;
						if (j < 0) j = SigProc->PeakDect->BeatAverageTimes-1;
						Beat_aver->InsertValue(float(SigProc->PeakDect->Peak_Shape[j][int(i)])/(Normalize*2));
					}
					if ((SigProc->PeakDect->BeatNumber % (Beat_aver->DisplaySlotNum-1)) == 0){
						Beat_aver->display_count = 0;
					}
					Beat_aver->InsertValue(-1);
					Beat_aver->InsertValue(1);
*/
					temp_aver = SigProc->PeakDect->BeatAverage();

					Beat_aver->SetGrid(CScale(0,(this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45),"s","mV");
					Beat_aver->DumpValue(temp_aver,SigProc->PeakDect->Templet_max_count);//把平均心電圖丟進去
					if (SigProc->PeakDect->IsTempleteStable()){
						Beat_aver->DetectECGFeature(temp_aver,SigProc->PeakDect->Templet_max_count);//偵測PQRST
						m_PR = Beat_aver->PRInterval;
						m_QT = Beat_aver->QTInterval;
						m_ST = Beat_aver->STInterval;
						m_QRS = Beat_aver->QRSInterval;
						m_PS = Beat_aver->PSInterval;
					}

							//	int a,i;
							//	for (i=0;i<SigProc->PeakDect->Templet_max_count; i++) {
							//			if (SigProc->PeakDect->TempleteIndex-1 < 0) a = 4;
							//			else a = SigProc->PeakDect->TempleteIndex-1;
							//			temp[i] = float(SigProc->PeakDect->Peak_Shape[a][i])/Normalize;
							//	}
							//	delete temp_aver;
							}
			}//if (Task & ANA_TASK_TEMPLETE){
	}else if(Mode == OFFLINE){	//離線模式，心電圖紀錄完後可使用此模式分析
		int i;
			if (Task & ANA_TASK_TEMPLETE){

				int sample,i,j=0,FoundPos=0,k=0;
				float *AverageECG;

				Peak->display_count = 0;
				float interval1 = this->m_SamplingFreq * 0.3;//向左取0.3秒
				float interval2 = this->m_SamplingFreq * 0.5;//向又取0,5秒
				AverageECG = new float[interval1+interval2];//配置0.8秒的記憶體
				for(i=0;i<m_SamplingFreq*0.8;i++) AverageECG[i] = 0;
				short *data;
				data = (short *)recorder.ecg_data.GetData();//取得心電圖sample的pointer
				for(i=0;i<recorder.BeatNum;i++){
					k=0;
					for (j=PeakArrary[i] - interval1; j < PeakArrary[i] + interval2; j++)  //以存在紀錄檔的心跳位置為準，向左取0.3...
					{
						if (j < 0 || j >= recorder.ecg_data.GetSize()) break;
						else AverageECG[k++] += data[j];//全部給他統計平均
					}
				}
				for(i=0;i<m_SamplingFreq*0.8;i++){
					AverageECG[i] /= recorder.peak_pos_ary.GetSize();
				}

				WORD *pointer;
				pointer = (WORD *)AverageECG;
				for(int i=0;i<m_SamplingFreq*0.8;i++){
					recorder.AverageECG.Add(*pointer);
					pointer++;
					recorder.AverageECG.Add(*pointer);
					pointer++;
				}

				for(i=0;i<m_SamplingFreq*0.8;i++)
 				{
					AverageECG[i] /= 2048;		//正規化
				}
				Beat_aver->SetGrid(CScale(0,0.8,-1,1),0.2,0.5,RGB(255,124,45),"s","mV");
				Beat_aver->DumpValue(AverageECG,m_SamplingFreq*0.8);//顯示

				Beat_aver->DetectECGFeature(AverageECG,m_SamplingFreq*0.8);//偵測PQRST
				m_PR = Beat_aver->PRInterval;
				m_QT = Beat_aver->QTInterval;
				m_ST = Beat_aver->STInterval;
				m_QRS = Beat_aver->QRSInterval;
				m_PS = Beat_aver->PSInterval;

				delete AverageECG;
			}

			if (Task & ANA_TASK_FFT){
				SigProc->FullDataFT((short *)recorder.ecg_data.GetData(),recorder.ecg_data.GetSize(),2048);

				FFT->SetGrid(CScale(0,m_SamplingFreq/2,0,SigProc->MaxFT),20,50,RGB(255,124,45),"f","db");
				FFT->DumpValue(SigProc->FT,1024);

//				Capstrum->DumpValue(SigProc->Capstrum,rect_low_view.Width());
				Envenlope->SetGrid(CScale(0,m_SamplingFreq/2,0,SigProc->MaxFT),20,50,RGB(255,124,45),"f","db");
				Envenlope->DumpValue(SigProc->Envenlope,1024);
			}
			if (Task & ANA_TASK_HRV){
				float *tmp;
				unsigned long FreqMax;
				tmp = new float[recorder.BeatNum];
				for(int i=0;i<recorder.BeatNum;i++) tmp[i] = (float)recorder.RRInterval.GetAt(i)/1000;//存在檔案裡的是ms為單位,因此要/1000變成秒

				RRInterval->SetGrid(CScale(1,recorder.BeatNum-1,0.5,1.5),100,0.2,RGB(255,124,45),"Num","s");
				RRInterval->DumpValue(tmp,recorder.BeatNum);//顯示RR sequence

				FreqMax = SigProc->HRV(tmp,recorder.BeatNum);  //Lomb method
				WORD *pointer;//用一個WORD的pointer來分割float
				pointer = (WORD *)SigProc->wk2;//頻譜能量
				for(int i=0;i<FreqMax/2;i++){		//wk2的頻譜對稱，使用WORD模式float
					recorder.HR_Freq.Add(*pointer);
					pointer++;
					recorder.HR_Freq.Add(*pointer);
					pointer++;
				}

				pointer = (WORD *)SigProc->wk1;//每個頻譜能量相對的頻率
				for(int i=0;i<FreqMax;i++){
					recorder.AverageECG_Index.Add(*pointer);
					pointer++;
					recorder.AverageECG_Index.Add(*pointer);
					pointer++;
				}

				float max =0 ;
				for(int i=0;i<FreqMax;i++) max = max > SigProc->wk2[i]? max:SigProc->wk2[i];
				for(int i=0;i<FreqMax;i++) SigProc->wk2[i]/=max;

				HR_FreqDomain->SetGrid(CScale(0,0.5,0,max),0.1,max/4,RGB(200,150,150),"f","db");
				HR_FreqDomain->DumpValue(SigProc->wk2,FreqMax/2);

				delete tmp;
			}
	}
	UpdateData(false);
}


void CEcgDlg::StoreSampling(int samp)
{
	recorder.ecg_data.Add(samp);//存入紀錄檔

}

void CEcgDlg::ECGScope(void)
{


//=============Draw higher paint area==============

	CWnd* pWnd = GetDlgItem(IDC_ECGSCOPE);  //Draw on static
	pDC = pWnd->GetDC();	

	CBitmap Bitmap;
	CBitmap* pbmOld = NULL;//Pointer to the CBitmap object
	dcMem.CreateCompatibleDC(pDC);//Greate painting object
	Bitmap.CreateCompatibleBitmap(pDC,rect_top_view.Width(),rect_top_view.Height());
	pbmOld = dcMem.SelectObject(&Bitmap);
	dcMem.BitBlt(0,0,rect_top_view.right,rect_top_view.bottom,pDC,0,0,WHITENESS);

	ECG->DrawWave(&dcMem);
	Peak->DrawWave(&dcMem);
	//DrawGrid(&dcMem,&rect_top_view);

	pDC->BitBlt(0,0,rect_top_view.right,rect_top_view.bottom,&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pbmOld);

	dcMem.DeleteDC();
	Bitmap.DeleteObject();
	pDC->DeleteDC();

//=============Draw lower paint area==============

	//pWnd = GetDlgItem(IDC_ECGSCOPE2);  //Draw on static
	pWnd = GetDlgItem(IDC_TAB_SPEC_FUNC);  //Draw on static
	
	pDC = pWnd->GetDC();

//	m_SpecialTab.GetClientRect(&rect);
	
	pbmOld = NULL;//Pointer to the CBitmap object
	dcMem.CreateCompatibleDC(pDC);//Greate painting object
	Bitmap.CreateCompatibleBitmap(pDC,rect_low_view.Width(),rect_low_view.Height()+23);
	pbmOld = dcMem.SelectObject(&Bitmap);
	dcMem.BitBlt(0,0,rect_low_view.right,rect_low_view.bottom,pDC,0,0,SRCCOPY);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,255));
	dcMem.FillRect(rect_low_view,&brush);

	switch (m_SpecialTab.GetCurSel())
	{
	case VIEW_ALOG_DEBUG:
			HFComponent->DrawWave(&dcMem);	

			TimeSlot->DrawWave(&dcMem);
			SlidingWindowMax->DrawWave(&dcMem);
			SlidingWindowThreshold->DrawWave(&dcMem);
			break;
	case VIEW_HRV_TIME:
		RRInterval->DrawWave(&dcMem);
		break;
	case VIEW_HRV_FREQ:
		HR_FreqDomain->DrawWave(&dcMem);	
		break;
	case VIEW_FFT:
		FFT->DrawWave(&dcMem);
		Envenlope->DrawWave(&dcMem);
		Capstrum->DrawWave(&dcMem);
		break;
	case VIEW_CAP_ENVE:
		break;
	case VIEW_ECG_TEMPLETE:
		Beat_aver->DrawWave(&dcMem);
		Beat_aver->DrawFeature(&dcMem); 
		break;
	}
	
//	pWnd->RedrawWindow();
//	pWnd->GetClientRect(&rect);


	pDC->BitBlt(0,0,rect_low_view.right,rect_low_view.bottom,&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pbmOld);

	dcMem.DeleteDC();
	Bitmap.DeleteObject();
	pDC->DeleteDC();
}

void CEcgDlg::DrawGrid(CDC *pDC,CRect *rect)
{
	float NormalPixelPerVolte,Original,NormalPixelPerSec,v;
	int d=2;

	NormalPixelPerVolte = rect->Height() / REFVOL;

	NormalPixelPerSec = m_SamplingFreq;

	Original = rect->Height()/2;

	CPen lpenR(PS_SOLID,1,RGB(255,10,12));//Select color green
	CPen* pOldpen = pDC->SelectObject(&lpenR);
	pDC->SelectObject(&lpenR);

	//draw horizontal lines

	for(v=0;v<=REFVOL/2;v=v+0.5)
	{
		pDC->MoveTo(rect->left,Original + int(v * NormalPixelPerVolte));
		pDC->LineTo(rect->right,Original +int(v * NormalPixelPerVolte));
	}
	for(v=0;v>=-REFVOL/2;v=v-0.5)
	{
		pDC->MoveTo(rect->left,Original + int(v * NormalPixelPerVolte));
		pDC->LineTo(rect->right,Original +int(v * NormalPixelPerVolte));
	}
	//draw vertical lines each one is equal to 1 sec
	for(float t=0;t<=(0.2*rect->Width());t=t+0.2)
	{
		pDC->MoveTo(int(t * NormalPixelPerSec),rect->top);
		pDC->LineTo(int(t * NormalPixelPerSec),rect->bottom);
	}
	//draw rect

	pDC->SelectObject(pOldpen);
}

float t=0;


void CEcgDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default	
	isTimerActive = 1;

//======================Simulation=======================
	if (UART_Enable != FALSE) {
	/*	float val;
		val = 0.3*sin(2*3.14159*60*t);
		if (Simulation > 0){
			val += 0.8*sin(2*3.14159*10*t);
			Simulation--;
		}
		ECG->InsertValue(val);
		SigProc->PeakDect->InsertValue(val*2048);

	//	if (this->sample_count % 50 == 0) {
	//		ECG->InsertValue(-1);
	//		SigProc->PeakDect->InsertValue(-2048);			
	//	}else{
	//		ECG->InsertValue(0);
	//		SigProc->PeakDect->InsertValue(0);		
	//	}

		HFComponent->InsertValue(-1*float(SigProc->PeakDect->HighFreqComp)/5000);
		Threshold->InsertValue(-1*float(SigProc->PeakDect->GetThreshold())/5000);
		sample_count++;
		if(sample_count == ECG->wave_rect.Width()){
		sample_count = 0;
		}
	//		if (SigProc->PeakDect->IsBeatPresent() == 1){
	//			UpdateData(true);
//
//				Peak->ChangeValue(-1,SigProc->PeakDect->BeatPosition);
//				CString beat;
//
//				beat.Format("%d",int(floor(SigProc->PeakDect->BeatPerMin)));
//				m_BeatPerSec.SetWindowText(beat);
//
//				beat.Format("%d",SigProc->PeakDect->BeatNumber);
//				m_BeatNum.SetWindowText(beat);
//
//				UpdateData(false);
//			}
//		Peak->InsertValue(1);
		t+=1.0/360;
		StoreSampling(val*2048);
		ECGScope();
*/

	Read();
	}
	ECGScope();
	CDialog::OnTimer(nIDEvent);
	isTimerActive = 0;
}


void CEcgDlg::Read()
{
	int samp,i=0,amount;
	float filtered;
	unsigned char Buffer[512];
    amount = uart.Read(Buffer, sizeof(Buffer));//讀取目前在buffer中的心電圖資料

	WaitingByte += amount;	//計算等待中的資料個數
	for(i=0;i<amount;i++)
	{
		Queue_buffer[(in_data++ & (BUFFER_MAX - 1))] = Buffer[i];//一個一個丟入Queue_buffer
	}
//	if ((index & 1) == 0 && index > 0)	//index is even
//	{
	while(WaitingByte > 1)//如果等待數量大於1
		{

			if (m_Resolution > 8){
				samp = int((Queue_buffer[(out_data & (BUFFER_MAX - 1))] ) //組合2個byte成為一個int
						  | Queue_buffer[(out_data+1 & (BUFFER_MAX - 1))] << 8);
				if (samp & 0x00008000) samp |= 0xffff0000;

				filtered = samp;
				if (m_60HzHP == true) filtered = filter->FIR_60Notch(filtered);
				if (m_40LP == true) filtered = filter->FIR_40_LP(filtered);
				if (m_005HP == true) filtered = filter->FIR_0_05_HP(filtered);

//				filtered = filter->FixFilter(filtered);
   				StoreSampling(filtered);

				out_data += 2;
				WaitingByte -= 2;
			}else if(m_Resolution == 8){
				samp = int(Queue_buffer[(out_data & (BUFFER_MAX - 1))] );
				//if (samp & 0x00000080) samp |= 0xffffff00;
   				//StoreSampling(samp);

				out_data ++;
				WaitingByte--;
			}

			if ((sample_count % DOWNSAMPLING) == 0){//可控制降取樣的比率
				AnalyseSignal(filtered,ANA_TASK_BEAT_DETECT | ANA_TASK_TEMPLETE | ANA_TASK_DEBUG_VIEW,RT);
			}
			
		}
//	}
}


void CEcgDlg::OnBnClickedButtonLtx()//傳送command word
{
	if (m_rs232_status.Compare("Connect!") == 0){
		char ComBuffer[30];
		int ComLength,i,k=0;
		ComLength = m_ComWord.GetLength();
		UpdateData(true);
		for(i=2;i<ComLength;i+=6){
			ComBuffer[k++] = ((Asc2Hex(m_ComWord.GetAt(i)) << 4) | Asc2Hex(m_ComWord.GetAt(i+1)));
		}
		if (ComLength) uart.Write(ComBuffer,k);
	}
}

void CEcgDlg::OnBnClickedButtonSendcom()
{
	char	LT1[9] = {8,'T','K',0x01,0x01,0x90,0x01,0x00,0x00};
	char	LT2[4] = {3,'T','K',0x02};
	char	LT3[4] = {3,'T','K',0x03};
	char	LT80[14] = {13,'T','K',0x80,'0x05','0x10','0x06','0x12','0x00','0x00',0x0a,0x00,0x00,0x00};
	char	LT4[10] = {9,'T','K',0x04,'0x05','0x10','0x06','0x12','0x00','0x00'};

	
	UpdateData(true);
		int i = ((m_ComNum / 10) * 16 + (m_ComNum % 16));
		switch(i)
		{
		case 1:
			TransCommandWord(LT1);
		break;

		case 2:
			TransCommandWord(LT2);
		break;

		case 3:
			TransCommandWord(LT3);
		break;

		case 4:
			GetDateTime(LT4);
			TransCommandWord(LT4);
		break;

		case 0x80:
			GetDateTime(LT80);
			TransCommandWord(LT80);
		break;

		case 0x81:
		break;
		}
	UpdateData(false);
}


void CEcgDlg::TransCommandWord(char *b)
{
	int i;
	m_ComWord = "";
	for(i=1;i<=b[0];i++)
	{
			m_ComWord += "0x";
			m_ComWord += Hex2Asc((b[i] & 0xf0) >> 4);
			m_ComWord += Hex2Asc(b[i] & 0x0f);
			m_ComWord += ", ";
	}

}

void CEcgDlg::GetDateTime(char *lt)
{
			int i;
			struct tm *tm_ptr;
			time_t t;
			::time(&t);
			tm_ptr = localtime(&t);
			tm_ptr->tm_year -= 80;
	//		tm_ptr->tm_year %= 100;
			tm_ptr->tm_mon++;			
			for (i=1;i<=6;i++){
				lt[i+3] = ((int *) tm_ptr)[6-i];
		}
//			for (i=1;i<=6;i++){
//				lt[i+3] = (((((int *) tm_ptr)[6-i] / 0x0a) << 4 | (((int *) tm_ptr)[6-i] % 0x0a)));
//		}
}

UINT CEcgDlg::ECG_Sample_Encode(UINT Sample)//Encode與decode是用來rs232傳輸時，將12bit取樣數值00001111 11111111改成00111111 00111111，如此能夠偵測漏掉的byte
{
    BYTE High_Byte,Low_Byte;
    Sample <<= 2;
    High_Byte = ((Sample & 0xff00) >> 8);
    Low_Byte = (Sample & 0x00ff) >> 2;
    High_Byte |= 64;
    Sample = High_Byte;
    Sample <<= 8;
    Sample |= Low_Byte;
    return Sample;
}

UINT CEcgDlg::ECG_Sample_Decode(BYTE High_Byte, BYTE Low_Byte)
{
    UINT Sample;
    High_Byte &= ~64;
    Sample = High_Byte;
    Sample <<= 6;
    Sample |= Low_Byte;
    return Sample;
}

char CEcgDlg::Hex2Asc(char h2a)
{
	if (h2a >= 10){
		h2a += 0x57;
	}else{
		h2a += 0x30; 	
	}
	return h2a;
}

char CEcgDlg::Asc2Hex(char a2h)
{
	if (a2h >= 0x60){
		a2h -= 0x57;
	}else{
		a2h -= 0x30; 	
	}
	return a2h;
}

void CEcgDlg::OnBnClickedButton1()
{
	Simulation = 10;
}

void CEcgDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) //
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(true);
	int	Max,Min;
	m_WaveSlideBar.GetRange(Min,Max);
	if (Max > 0){
		int Pos = m_WaveSlideBar.GetPos();
		int sample,i,j=0,FoundPos=0;

		Peak->display_count = 0;
		ECG->SetGrid(CScale(float(Pos)/m_SamplingFreq,float(Pos)/m_SamplingFreq + (this->rect_top_view.Width()/this->m_SamplingFreq),-1,1),0.2,0.5,RGB(255,124,45),"s","mV");
		for ( i=0 ; i< this->rect_low_view.Width(); i++) {
			Peak->InsertValue(1);
			if((i+Pos) < recorder.ecg_data.GetSize()){
				sample = recorder.ecg_data.GetAt(i+Pos);
				if (sample & 0x00008000) sample |= 0xffff0000;
			//	AnalyseSignal(sample,ANA_TASK_DEBUG_VIEW,RT);
				ECG->InsertValueAt(i,float (sample) / this->Normalize);

				for (j=FoundPos; j < recorder.BeatNum && this->m_BeatNum > 0; j++)  //find the position of peak 
				{
		//			if ((i+Pos) == recorder.peak_pos_ary.GetAt(j)){
					if ((i+Pos) == PeakArrary[j]){
						FoundPos = j+1;
						Peak->ChangeValue(-1,i);
						break;
					}
				}
			}

		}
		
		ECGScope();
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEcgDlg::OnLButtonDblClk(UINT nFlags,CPoint point)
{

}

void CEcgDlg::OnBnClickedButton4()
{
	int result = dlg_setting.DoModal();

	if (result == IDOK){
		this->UpdateData(true);
		Normalize = m_Resolution;
		ComPort_Number = dlg_setting.m_ComPort_Number;
		baudrate = dlg_setting.m_BaudRate;
		recorder.Age = m_RecordAge;
		recorder.Name = m_RecordName;
		recorder.SamplimgFreq = m_SamplingFreq;
		recorder.Resolution = m_Resolution;

		if (this->m_Male){
			recorder.Sex = "Male";
			m_RecordSex = "Male";
		}else{
			recorder.Sex = "Female";
			m_RecordSex = "Female";
		}
		this->Uartopen();
	}
}


void CEcgDlg::OnBnClickedButton6()
{	
	this->Uartclose();

	if (recorder.peak_pos_ary.GetSize() > 0){
		PeakArrary = new UINT[recorder.peak_pos_ary.GetSize()];
		PeakArrary[0] = recorder.peak_pos_ary.GetAt(0);
		for(int i=1;i<recorder.peak_pos_ary.GetSize();i++){
			PeakArrary[i] = PeakArrary[i-1] + recorder.peak_pos_ary.GetAt(i);
		}
		AnalyseSignal(0,ANA_TASK_HRV | ANA_TASK_TEMPLETE,OFFLINE);
		delete [] PeakArrary;
	}


	CFile sFile(".\\ECG recorder\\"+recorder.Name+".ecg",CFile::modeCreate|CFile::modeWrite);
	
	sFile.Seek(0,CFile::begin);
	CArchive	ecg_archive(&sFile,CArchive::store);
	
	recorder.BeatNum = SigProc->PeakDect->BeatNumber;
	
	recorder.SerializeAll(&ecg_archive);
	ecg_archive.Close();
	
	recorder.ecg_data.RemoveAll();
	recorder.peak_pos_ary.RemoveAll();
	recorder.RRInterval.RemoveAll();
	recorder.AverageECG.RemoveAll();
	recorder.AverageECG_Index.RemoveAll();
	recorder.HR_Freq.RemoveAll();

	while(this->isTimerActive);
	if (ECG != 0) {
		delete Beat_aver;
		delete Queue_buffer;
		delete HR_FreqDomain;
		delete Peak;
		delete ECG;
		delete RRInterval;
		delete SlidingWindowMax;
		delete TimeSlot;
		delete HFComponent;
		delete SigProc;
		delete TempleteMatch;
		delete SlidingWindowThreshold;
		delete FFT;
		delete Capstrum;
		delete Envenlope;
		ECG = 0;
	}
}

//讀取心電圖檔案,分2種
//第一種：即時從藍芽傳送,在PC紀錄且將所有分析做完並存入紀錄檔
//第二種：從SD card讀取，並在此函式中分析並存入紀錄檔
void CEcgDlg::OnBnClickedButton5()
{
	CDlgAnylsisSel dlg_Feature;
	int result = dlg_Feature.DoModal();
	delete [] PeakArrary;
	PeakArrary = 0;
	PreviousPeakPos = 0;
	if(result == IDOK){
		if (!dlg_Feature.m_ReadSD && (dlg_Feature.m_HRV || dlg_Feature.m_FeaturePoint)){
			CString FileName;
			CFileDialog m_lFile(TRUE);
			m_lFile.m_ofn.lpstrFilter = "*.ecg";
			m_lFile.m_ofn.lpstrDefExt = "*.ecg";
			if(m_lFile.DoModal()==IDOK)
			{
				FileName = m_lFile.GetFileName();
				CFile f(FileName,CFile::modeRead);
				CArchive	ecg_archive(&f,CArchive::load);
				recorder.SerializeAll(&ecg_archive);
				
				m_RecordSex = recorder.Sex;
				m_RecordAge = recorder.Age;
				m_RecordName = recorder.Name;
				m_Resolution = recorder.Resolution;
				m_SamplingFreq = recorder.SamplimgFreq;
				f.Close();
				this->UpdateData(false);
				CString beat;
				beat.Format("%d",recorder.BeatNum);
				m_BeatNum.SetWindowText(beat);

				this->m_BeatAverageTimes = recorder.BeatNum;

				SignalProcViewInit();
				m_WaveSlideBar.SetRangeMax(recorder.ecg_data.GetSize() - rect_low_view.Width());
		
				PeakArrary = new UINT[recorder.peak_pos_ary.GetSize()];
				PeakArrary[0] = recorder.peak_pos_ary.GetAt(0);

				for(int i=1;i<recorder.peak_pos_ary.GetSize();i++){
					PeakArrary[i] = PeakArrary[i-1] + recorder.peak_pos_ary.GetAt(i);
				}

				CProcessingBar *ProcBar;

				ProcBar = new CProcessingBar(this);

				ProcBar->Create(IDD_PROCESSING,this);
				ProcBar->ShowWindow(SW_SHOWNORMAL);
				int sample=0;
				if (dlg_Feature.m_FeaturePoint){
					ProcBar->m_Progress.SetRange(0,recorder.ecg_data.GetSize());
					this->UpdateData(false);

					AnalyseSignal(sample,ANA_TASK_HRV | ANA_TASK_TEMPLETE,OFFLINE);

					sample_count = 0;
			/*		for (int i=0 ; i< recorder.ecg_data.GetSize(); i++) {
							sample = recorder.ecg_data.GetAt(i);
							if (sample & 0x00008000) sample |= 0xffff0000;
							AnalyseSignal(sample,ANA_TASK_BEAT_DETECT | ANA_TASK_TEMPLETE,RT);
							ProcBar->m_Progress.SetPos(i);
							this->UpdateData(true);
					}*/
			/*
					for (int i=0;i<recorder.ecg_data.GetSize();i++) AnalyseSignal(recorder.ecg_data.GetAt(i));

					int sample;
					for (int i=0 ; i< this->rect_low_view.Width(); i++) {
						if(i >= recorder.ecg_data.GetSize()) break;
						sample = recorder.ecg_data.GetAt(i);
						if (sample & 0x00008000) sample |= 0xffff0000;
						ECG->InsertValueAt(i,float (sample) / this->m_Resolution);
					}
					ECGScope();*/
				}
				
				if (dlg_Feature.m_HRV){
					AnalyseSignal(sample,ANA_TASK_FFT,OFFLINE);
				}
				ProcBar->DestroyWindow();
				delete ProcBar;
				ECGScope();
			}
		}else if (dlg_Feature.m_ReadSD){		//從SD card讀取
		CString FileName;
		CString PathName;
		CFileDialog m_lFile(TRUE);
		m_lFile.m_ofn.lpstrFilter = "*.raw";
		m_lFile.m_ofn.lpstrDefExt = "*.raw";
		if(m_lFile.DoModal()==IDOK)
		{
			CString FileName;
			FileName = m_lFile.GetFileName();
			PathName = m_lFile.GetPathName();
			CFile f(FileName,CFile::modeRead);
			BYTE *buf;
			recorder.Remove();
			int k=0,size;
			size = f.GetLength()/2;
			FileName = f.GetFileName();
			
			buf = new BYTE[f.GetLength()];
			f.Read(buf,f.GetLength());
			recorder.ecg_data.SetSize(size);
			recorder.peak_pos_ary.SetSize(2000);
			recorder.RRInterval.SetSize(2000);
			short int data;


			m_RecordSex = "Male";
			m_RecordAge = "24";
			m_RecordName = "Hawkeye";
			m_Resolution = 12;
			m_SamplingFreq = 360;
			m_BeatAverageTimes = 5;
			SignalProcViewInit();

			for(int i=0;i<f.GetLength();i+=2){		//讀取取樣資料,並放入recorder.ecg_data
				data = buf[i] | short(buf[i+1] << 8);
/*				float filtered;
				filtered = (float)data;
				filtered = filter->FIR_60Notch(filtered);
				filtered = filter->FIR_40_LP(filtered);
				filtered = filter->FIR_0_05_HP(filtered);*/

				recorder.ecg_data.SetAt(k++,data);
			}
			delete [] buf;
			f.Close();
			
			this->UpdateData(false);
			CString beat;
			beat.Format("%d",recorder.BeatNum);
			m_BeatNum.SetWindowText(beat);

			m_WaveSlideBar.SetRangeMax(recorder.ecg_data.GetSize()- rect_low_view.Width());

			CProcessingBar *ProcBar;

			ProcBar = new CProcessingBar(this);

			ProcBar->Create(IDD_PROCESSING,this);
			ProcBar->ShowWindow(SW_SHOWNORMAL);
			int sample=0;
			if (dlg_Feature.m_FeaturePoint){				//分析心電圖資料,R波抓取,QRS平均計算
				ProcBar->m_Progress.SetRange(0,recorder.ecg_data.GetSize());

				this->UpdateData(false);

				//AnalyseSignal(sample,ANA_TASK_HRV | ANA_TASK_TEMPLETE,OFFLINE);

				sample_count = 0;
				int len = recorder.ecg_data.GetSize();
				for (int i=0 ; i< len; i++) {
						sample = (int)recorder.ecg_data.GetAt(i);
						if (sample & 0x00008000) sample |= 0xffff0000;
						sample_count++;
						SigProc->PeakDect->InsertValue(sample);
					
						int BeatPos = SigProc->PeakDect->GetPeakPosition();
						if (BeatPos > -1){

							recorder.RRInterval.SetAt(SigProc->PeakDect->BeatNumber,SigProc->PeakDect->BeatInterval*1000);

							UINT PresentPeakPos;
							PresentPeakPos = sample_count - SigProc->PeakDect->GetPeakDeviation();

							if (this->recorder.peak_pos_ary.GetSize() == 0)
								this->recorder.peak_pos_ary.SetAt(SigProc->PeakDect->BeatNumber,PresentPeakPos);
							else 
								this->recorder.peak_pos_ary.SetAt(SigProc->PeakDect->BeatNumber,WORD(PresentPeakPos - PreviousPeakPos));

							PreviousPeakPos = PresentPeakPos;
						}
				}

			}

			recorder.BeatNum = SigProc->PeakDect->BeatNumber;
			recorder.Name = m_RecordName;
			recorder.Age = m_RecordAge;
			recorder.Resolution = m_Resolution;
			recorder.Sex = m_RecordSex;
			recorder.SamplimgFreq = m_SamplingFreq;
			recorder.peak_pos_ary.FreeExtra();
			recorder.RRInterval.FreeExtra();

			if (recorder.BeatNum > 0){
				PeakArrary = new UINT[recorder.BeatNum];
				PeakArrary[0] = recorder.peak_pos_ary.GetAt(0);
				for(int i=1;i<recorder.BeatNum;i++){
					PeakArrary[i] = PeakArrary[i-1] + recorder.peak_pos_ary.GetAt(i);
				}
				//delete PeakArrary;
				AnalyseSignal(sample,ANA_TASK_HRV | ANA_TASK_TEMPLETE,OFFLINE);
			}
			
			AnalyseSignal(sample,ANA_TASK_FFT,OFFLINE);


			beat.Format("%d",SigProc->PeakDect->BeatNumber);
			m_BeatNum.SetWindowText(beat);
			ProcBar->DestroyWindow();
			delete ProcBar;
			ECGScope();
			this->UpdateData(false);
		
			CFile sFile(PathName,CFile::modeCreate|CFile::modeWrite);		
			sFile.Seek(0,CFile::begin);
			CArchive	ecg_archive(&sFile,CArchive::store);
			
			recorder.BeatNum = SigProc->PeakDect->BeatNumber;
			
			recorder.SerializeAll(&ecg_archive);
			ecg_archive.Close();
		}
		}
	}
}

void CEcgDlg::OnBnClickedSexSel()
{
	// TODO: Add your control notification handler code here
	this->m_Male = 1;
}

void CEcgDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	this->m_Male = 0;
}

void CEcgDlg::OnBnClicked40hzlpf()
{
	this->UpdateData(true);
	this->UpdateData(false);
}

void CEcgDlg::OnBnClicked60hznotch()
{
	this->UpdateData(true);
	this->UpdateData(false);
}

void CEcgDlg::OnBnClicked50hzhpf()
{
	this->UpdateData(true);
	this->UpdateData(false);
}

void CEcgDlg::OnMouseMove(UINT nFlags,CPoint point)
{
	CWnd* pWnd = GetDlgItem(IDC_TAB_SPEC_FUNC);
//	SetCapture();
	CRect wndRect,Client;
	pWnd->GetWindowRect(&wndRect);
	pWnd->GetClientRect(&Client);
	ScreenToClient(&wndRect);

	switch (m_SpecialTab.GetCurSel())
	{
	case VIEW_ALOG_DEBUG:
		break;

	case VIEW_FFT:


		  if (wndRect.PtInRect(point))	
		  {  // Test if the pointer is on the window
	//			CRect rect;
	//			rect.SetRect(wndRect.right-50,wndRect.top+50,wndRect.left,wndRect.top+60);
				
	//			dcMem->DrawText("Q",1,&rect,DT_CENTER);
			  if (FFT != 0){
				  point.SetPoint(point.x-wndRect.left,point.y);
				  if (FFT->wave_rect.PtInRect(point)){
					m_Freq.Format("%fHz",(float(point.x) / FFT->wave_rect.Width()) * (this->m_SamplingFreq));
				  }
				  UpdateData(false);
			  }
			//	m_Freq = str;
		  } 
		  else
		  {

		  }


		break;
	case VIEW_CAP_ENVE:

		break;
	case VIEW_ECG_TEMPLETE:
		break;
	}
	CWnd::OnMouseMove(nFlags, point);

}
void CEcgDlg::OnTcnSelchangeTabSpecFunc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
		
	if (ECG != 0) this->ECGScope();	
}

void CEcgDlg::OnEnChangeEdit12()
{
	this->UpdateData(true);
}
