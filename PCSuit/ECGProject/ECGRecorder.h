
class CECGRecorder : public CObject
{
public:
	CECGRecorder();

	CString		Name;
	CString		Sex;
	CString		Age;
	int		SamplimgFreq;
	int		Resolution;
	UINT	BeatNum;

	CWordArray	AverageECG;			//使用short int
	CWordArray	AverageECG_Index;	//使用short int
	CWordArray  HR_Freq;			//使用浮點表示
	CWordArray	RRInterval;			//使用浮點表示
	CWordArray	ecg_data;			//使用short int
	CWordArray  peak_pos_ary;		//使用short int
	
	void SerializeAll(CArchive *ecg_archive);
	void Remove();
public:
	~CECGRecorder(void);
};
