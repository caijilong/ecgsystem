#include "PeakDetection.h"


#define BUFFER_SIZE	512

#define FILTER_FIR_40_LP   0
#define FILTER_FIR_60Notch 1
#define FILTER_FIR_0_05_HP 2

#define LOGeTOLOG10 (20.0/log(10.0))
#define pi	3.141592
//lomb---------------------------------------------------------------
#define LMAX(a,b) (lmaxarg1 = (a),lmaxarg2 = (b), (lmaxarg1 > lmaxarg2 ? \
						   lmaxarg1 : lmaxarg2))
#define LMIN(a,b) (lminarg1 = (a),lminarg2 = (b), (lminarg1 < lminarg2 ? \
						   lminarg1 : lminarg2))
#define MOD(a,b)	while (a >= b) a -= b
#define MACC 4
#define SIGN(a,b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SQR(a) ((sqrarg = (a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

typedef struct {
    double real, imag;
} COMPLEX;

class CECGsp 
{
	public:
		CECGsp(int s,int size,int BAT,int Resolution);
		~CECGsp();
		CPeakDetection *PeakDect;

		float *Envenlope,*FT,*Capstrum;
		int sampling_rate;
		int FIR_40_LP(int samp);
		int FIR_60Notch(int samp);
		int FIR_0_05_HP(int samp);
		void FullDataFT(short *wave_buffer,int dwDataLength,int fft_length);
		unsigned long HRV(float *x,int length);
		float MaxFT,EvenlopeMax,CapstrumMax;

		long lmaxarg1, lmaxarg2;
		float sqrarg;
		long lminarg1, lminarg2;
		char *pname;
		float pwr;
		FILE *ifile;
		float *x, *y, *wk1, *wk2;
		unsigned long nmax;	/* Initial buffer size (must be a power of 2).
					   Note that input() will increase this value as
					   necessary by repeated doubling, depending on
					   the length of the input series. */

	private:
		void fasper(
			float x[], float y[],
			unsigned long n,
			float ofac, float hifac, float wk1[], float wk2[],
			unsigned long nwk, unsigned long *nout, unsigned long *jmax,
			float *prob);
		void avevar(float data[], unsigned long n, float *ave, float *var);
		void realft(float data[], unsigned long n, int isign);
		void four1(float data[],unsigned long nn,int isign);
		void spread(float y, float yy[], unsigned long n, float x, int m);
		void zeromean(unsigned long n,float *y);
		int result_size;
		void fft(COMPLEX *x, int m);
		void ifft(COMPLEX *x, int m);
		float *result_buffer;
		float *result_buffer3;		
		
};