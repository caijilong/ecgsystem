#include "stdafx.h"
#include "ECGRecorder.h"

CECGRecorder::CECGRecorder()
{
	BeatNum = 0;
}

void CECGRecorder::SerializeAll(CArchive *ar)
{


	if(ar->IsStoring())
		*ar<<Name<<Sex<<Age<<SamplimgFreq<<Resolution<<BeatNum;
	else
		*ar>>Name>>Sex>>Age>>SamplimgFreq>>Resolution>>BeatNum;
	
	CObject::Serialize(*ar);

	HR_Freq.Serialize(*ar);
	AverageECG.Serialize(*ar);
	AverageECG_Index.Serialize(*ar);
	ecg_data.Serialize(*ar);
	RRInterval.Serialize(*ar);
	peak_pos_ary.Serialize(*ar);
}
void CECGRecorder::Remove()
{
	HR_Freq.RemoveAll();
	AverageECG.RemoveAll();
	AverageECG_Index.RemoveAll();
	ecg_data.RemoveAll();
	RRInterval.RemoveAll();
	peak_pos_ary.RemoveAll();
}

CECGRecorder::~CECGRecorder(void)
{

}
