#include "stdafx.h"
#include "ECGSignalProcessing.h"
#include "math.h"

CECGsp::CECGsp(int s,int size,int BAT,int Resolution)
{
	sampling_rate = s;
	result_size = size;
	PeakDect = new CPeakDetection(s,size,BAT,Resolution);
	result_buffer = 0;
	result_buffer3 = 0;
	Envenlope = 0;
	FT= 0;
	Capstrum = 0;
	x = 0;
	wk1 = 0;
	wk2 = 0;


}

void CECGsp::FullDataFT(short *wave_buffer,int dwDataLength,int fft_length)
{
	int i=0,c,j,fft_power=0;
	int max_signal = 0;
	double tempflt,f;
	double max_spectrum_buffer=0,max_spectrum_buffer3=0;
	int start_point=0;
	float b;
	if (Envenlope == 0){
		Envenlope = new float[fft_length/2];
		FT= new float[fft_length/2];
		Capstrum = new float[fft_length/2];
	}
	for (int j=0 ; j<fft_length/2 ; j++){
		Envenlope[j] = 0;
		FT[j] = 0;
		Capstrum[j] = 0;
	}

	COMPLEX *result_buffer2 = new COMPLEX[fft_length];

	if (result_buffer == 0)
			result_buffer = new float[fft_length];

	if (result_buffer3 == 0)
			result_buffer3 = new float[fft_length];

	for (j=0 ; j<dwDataLength ; j++){
		max_signal = (max_signal > abs(wave_buffer[j])) ? max_signal : abs(wave_buffer[j]);
	}
	i = fft_length;
	while(i > 1)
	{
		i>>=1;
		fft_power++; 
	}
	for (start_point=0 ; (start_point + fft_length) < dwDataLength ; start_point+=(fft_length/2)){

		i=0;
		for (c = start_point ; c < fft_length + start_point;c++)
		{
			result_buffer2[i].real = double(wave_buffer[c]) / max_signal;
			i++;
		}
			for (j=0 ; j < fft_length ; j++)
			{
				if (j == dwDataLength) break;
  				b = 0.54-0.46*cos(2*pi*j/fft_length);
				result_buffer2[j].real *= b;
			}


		fft(result_buffer2,fft_power);
		max_spectrum_buffer = 0;
		for (i = 0 ; i < fft_length ; i++)
		{
			tempflt  = result_buffer2[i].real * result_buffer2[i].real;
			tempflt += result_buffer2[i].imag * result_buffer2[i].imag;
	//		result_buffer[i] += log10( (double) (sqrt(tempflt) + 1.0) );
	//		result_buffer[i] = log( (double) (sqrt(tempflt) + 1.0) );
			result_buffer[i] = log10(sqrt(tempflt)+1);
			result_buffer2[i].real = log(sqrt(tempflt)+1);
			result_buffer2[i].imag = 0;
			if (i>0) max_spectrum_buffer = (max_spectrum_buffer > fabs(result_buffer[i])) ? max_spectrum_buffer : fabs(result_buffer[i]);
		}
/*
		for (j = 0 ; j < fft_length ; j++)
		{
			result_buffer2[j].real = result_buffer[j];
			result_buffer2[j].imag = 0;
		}
*/
		for (j = 1 ; j < fft_length/2 ; j++)
		{
			FT[j] += result_buffer[j];
		}

		result_buffer2[0].real = 0;
		ifft(result_buffer2,fft_power);
/*
		j = 1;
		for (f=1;f < fft_length;f+=(float(fft_length)/result_size))
		{
			if (f > fft_length/2) Capstrum[j] = 0;
			else Capstrum[j] += (fabs(result_buffer2[int(f)].real); 
			j++;
		}*/

		for (j = 0 ; j < fft_length ; j++)
		{
			if (j > 30 && j < fft_length - 30 - 1)
				result_buffer2[j].real = 0;
			else 
				result_buffer2[j].real /= fft_length;
			
			result_buffer2[j].imag = 0;
		}

		fft(result_buffer2,fft_power);
		max_spectrum_buffer3 = 0;
		for (j = 0 ; j < fft_length ; j++)
		{	
			result_buffer2[j].real *= LOGeTOLOG10/20;
	//		result_buffer2[j].real *= (LOGeTOLOG10/20);
			if (i>0) max_spectrum_buffer3 = (max_spectrum_buffer3 > fabs(result_buffer2[j].real)) ? max_spectrum_buffer3 : fabs(result_buffer2[j].real);	
		}

		for (j = 0 ; j < fft_length/2 ; j++)
		{
			Envenlope[j] += fabs(result_buffer2[j].real);
		}
	}
	MaxFT=0;
	EvenlopeMax=0;
	CapstrumMax=0;
	for (int i=0;i< fft_length/2 ;i++){
		MaxFT = (MaxFT > FT[i]) ? MaxFT : FT[i];
	}
	for (int i=0;i< fft_length/2 ;i++){
		Envenlope[i] /= MaxFT;
		FT[i] /= MaxFT;
		Capstrum[i] /= MaxFT;
	}

	delete [] result_buffer;
	delete [] result_buffer2;
	delete [] result_buffer3;
	result_buffer = 0;
	result_buffer2 = 0;
	result_buffer3 = 0;
	
}

void CECGsp::fft(COMPLEX *x, int m)
{
    static COMPLEX *w;           /* used to store the w complex array */
    static int mstore = 0;       /* stores m for future reference */
    static int n = 1;            /* length of fft stored for future */

    COMPLEX u,temp,tm;
    COMPLEX *xi,*xip,*xj,*wptr;

    int i,j,k,l,le,windex;

    double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;

    if(m != mstore) {

/* free previously allocated storage and set new m */

        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       /* if m=0 then done */

/* n = 2**m = fft length */

        n = 1 << m;
        le = n/2;

/* allocate the storage for w */

        w = (COMPLEX *) calloc(le-1,sizeof(COMPLEX));
        if(!w) {
            printf("\nUnable to allocate complex W array\n");
            exit(1);
        }

/* calculate the w values recursively */

        arg = 4.0*atan(1.0)/le;         /* PI/le calculation */
        wrecur_real = w_real = cos(arg);
        wrecur_imag = w_imag = -sin(arg);
        xj = w;
        for (j = 1 ; j < le ; j++) {
            xj->real = (float)wrecur_real;
            xj->imag = (float)wrecur_imag;
            xj++;
            wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
            wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
            wrecur_real = wtemp_real;
        }
    }

/* start fft */

    le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++) {
        le = le/2;

/* first iteration with no multiplies */

        for(i = 0 ; i < n ; i = i + 2*le) {
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }

/* remaining iterations use stored w */

        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) {
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) {
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
                tm.imag = xi->imag - xip->imag;
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }

/* rearrange data by bit reversing */

    j = 0;
    for (i = 1 ; i < (n-1) ; i++) {
        k = n/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }
}


void CECGsp::ifft(COMPLEX *x, int m)
{
    static COMPLEX *w;           /* used to store the w complex array */
    static int mstore = 0;       /* stores m for future reference */
    static int n = 1;            /* length of fft stored for future */

    COMPLEX u,temp,tm;
    COMPLEX *xi,*xip,*xj,*wptr;

    int i,j,k,l,le,windex;

    double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;

    if(m != mstore) {

/* free previously allocated storage and set new m */

        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       /* if m=0 then done */

/* n = 2**m = fft length */

        n = 1 << m;
        le = n/2;

/* allocate the storage for w */

        w = (COMPLEX *) calloc(le-1,sizeof(COMPLEX));
        if(!w) {
            printf("\nUnable to allocate complex W array\n");
            exit(1);
        }

/* calculate the w values recursively */

        arg = 4.0*atan(1.0)/le;         /* PI/le calculation */
        wrecur_real = w_real = cos(arg);
        wrecur_imag = w_imag = sin(arg);
        xj = w;
        for (j = 1 ; j < le ; j++) {
            xj->real = (float)wrecur_real;
            xj->imag = (float)wrecur_imag;
            xj++;
            wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
            wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
            wrecur_real = wtemp_real;
        }
    }

/* start fft */

    le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++) {
        le = le/2;

/* first iteration with no multiplies */

        for(i = 0 ; i < n ; i = i + 2*le) {
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }

/* remaining iterations use stored w */

        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) {
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) {
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
                tm.imag = xi->imag - xip->imag;
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }

/* rearrange data by bit reversing */

    j = 0;
    for (i = 1 ; i < (n-1) ; i++) {
        k = n/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }
}


void CECGsp::fasper(
float x[], float y[],
unsigned long n,
float ofac, float hifac, float wk1[], float wk2[],
unsigned long nwk, unsigned long *nout, unsigned long *jmax,
float *prob)
{
    unsigned long j, k, ndim, nfreq, nfreqt;
    float ave, ck, ckk, cterm, cwt, den, df, effm, expy, fac, fndim, hc2wt,
          hs2wt, hypo, pmax, sterm, swt, var, xdif, xmax, xmin;

    *nout = 0.5*ofac*hifac*n;
    nfreqt = ofac*hifac*n*MACC;
    nfreq = 64;
    while (nfreq < nfreqt) nfreq <<= 1;
    ndim = nfreq << 1;
    if (ndim > nwk)
		nwk = 0;
//		error("workspaces too small\n");
    avevar(y, n, &ave, &var);
    xmax = xmin = x[1];
    for (j = 2; j <= n; j++) {
	if (x[j] < xmin) xmin = x[j];
	if (x[j] > xmax) xmax = x[j];
    }
    xdif = xmax - xmin;
    for (j = 1; j <= ndim; j++) wk1[j] = wk2[j] = 0.0;
    fac = ndim/(xdif*ofac);
    fndim = ndim;
    for (j = 1; j <= n; j++) {
	ck = (x[j] - xmin)*fac;
	MOD(ck, fndim);
	ckk = 2.0*(ck++);
	MOD(ckk, fndim);
	++ckk;
	spread(y[j] - ave, wk1, ndim, ck, MACC);
	spread(1.0, wk2, ndim, ckk, MACC);
    }
    realft(wk1, ndim, 1);
    realft(wk2, ndim, 1);
    df = 1.0/(xdif*ofac);
    pmax = -1.0;
    for (k = 3, j = 1; j <= (*nout); j++, k += 2) {
	hypo = sqrt(wk2[k]*wk2[k] + wk2[k+1]*wk2[k+1]);
	hc2wt = 0.5*wk2[k]/hypo;
	hs2wt = 0.5*wk2[k+1]/hypo;
	cwt = sqrt(0.5+hc2wt);
	swt = SIGN(sqrt(0.5-hc2wt), hs2wt);
	den = 0.5*n + hc2wt*wk2[k] + hs2wt*wk2[k+1];
	cterm = SQR(cwt*wk1[k] + swt*wk1[k+1])/den;
	sterm = SQR(cwt*wk1[k+1] - swt*wk1[k])/(n - den);
	wk1[j] = j*df;
	wk2[j] = (cterm+sterm)/(2.0*var);
	if (wk2[j] > pmax) pmax = wk2[(*jmax = j)];
    }
    expy = exp(-pmax);
    effm = 2.0*(*nout)/ofac;
    *prob = effm*expy;
 //   if (*prob > 0.01) *prob = 1.0 - pow(1.0 - expy, effm);
}

void CECGsp::spread(float y, float yy[], unsigned long n, float x, int m)
{
    int ihi, ilo, ix, j, nden;
    static int nfac[11] = { 0, 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };
    float fac;

    if (m > 10)
		m = 0;
	//error("factorial table too small");
    ix = (int)x;
    if (x == (float)ix) yy[ix] += y;
    else {
	ilo = LMIN(LMAX((long)(x - 0.5*m + 1.0), 1), n - m + 1);
	ihi = ilo + m - 1;
	nden = nfac[m];
	fac = x - ilo;
	for (j = ilo + 1; j <= ihi; j++) fac *= (x - j);
	yy[ihi] += y*fac/(nden*(x - ihi));
	for (j = ihi-1; j >= ilo; j--) {
	    nden = (nden/(j + 1 - ilo))*(j - ihi);
	    yy[j] += y*fac/(nden*(x - j));
	}
    }
}

void CECGsp::avevar(float data[], unsigned long n, float *ave, float *var)
{
    unsigned long j;
    float s, ep;

    for (*ave = 0.0, j = 1; j <= n; j++) *ave += data[j];
    *ave /= n;
    *var = ep = 0.0;
    for (j = 1; j <= n; j++) {
	s = data[j] - (*ave);
	ep += s;
	*var += s*s;
    }
    *var = (*var - ep*ep/n)/(n-1);
    pwr = *var;
}

void CECGsp::realft(float data[], unsigned long n, int isign)
{
    unsigned long i, i1, i2, i3, i4, np3;
    float c1 = 0.5, c2, h1r, h1i, h2r, h2i;
    double wr, wi, wpr, wpi, wtemp, theta;

    theta = 3.141592653589793/(double)(n>>1);
    if (isign == 1) {
	c2 = -0.5;
	four1(data, n>>1, 1);
    } else {
	c2 = 0.5;
	theta = -theta;
    }
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0 + wpr;
    wi = wpi;
    np3 = n+3;
    for (i = 2; i <= (n>>2); i++) {
	i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
	h1r =  c1*(data[i1] + data[i3]);
	h1i =  c1*(data[i2] - data[i4]);
	h2r = -c2*(data[i2] + data[i4]);
	h2i =  c2*(data[i1] - data[i3]);
	data[i1] =  h1r + wr*h2r - wi*h2i;
	data[i2] =  h1i + wr*h2i + wi*h2r;
	data[i3] =  h1r - wr*h2r + wi*h2i;
	data[i4] = -h1i +wr*h2i + wi*h2r;
	wr = (wtemp = wr)*wpr - wi*wpi + wr;
	wi = wi*wpr + wtemp*wpi + wi;
    }
    if (isign == 1) {
	data[1] = (h1r = data[1]) + data[2];
	data[2] = h1r - data[2];
    } else {
	data[1] = c1*((h1r = data[1]) + data[2]);
	data[2] = c1*(h1r - data[2]);
	four1(data, n>>1, -1);
    }
}

void CECGsp::four1(float data[],unsigned long nn,int isign)
{
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
	if (j > i) {
	    SWAP(data[j], data[i]);
	    SWAP(data[j+1], data[i+1]);
	}
	m = n >> 1;
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }
    mmax = 2;
    while (n > mmax) {
	istep = mmax << 1;
	theta = isign*(6.28318530717959/mmax);
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 1; m < mmax; m += 2) {
	    for (i = m;i <= n;i += istep) {
		j = i + mmax;
		tempr = wr*data[j] - wi*data[j+1];
		tempi = wr*data[j+1] + wi*data[j];
		data[j] = data[i] - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }
	    wr = (wtemp = wr)*wpr - wi*wpi + wr;
	    wi = wi*wpr + wtemp*wpi + wi;
	}
	mmax = istep;
    }
}


unsigned long CECGsp::HRV(float *y,int length)
{
	unsigned long nout, jmax;
	float prob;
	nmax = 1;
	while(nmax < length) nmax <<= 1;
    zeromean(length,y);
	if (x == 0){
		x = new float[nmax];
		for(int i=0;i<length;i++) x[i]=i;
	}
	if (wk1 == 0) wk1 = new float[64*nmax];
	if (wk2 == 0) wk2 = new float[64*nmax];



    /* Compute the Lomb periodogram. */
    fasper(x-1, y-1, length, 4.0, 2.0, wk1-1, wk2-1, 64*nmax, &nout, &jmax, &prob);
	

	return  nout;
}



/* Read input data, allocating and filling x[] and y[].  The return value is
   the number of points read.

   This function allows the input buffers to grow as large as necessary, up to
   the available memory (assuming that a long int is large enough to address
   any memory location). */
/*
unsigned long CECGsp::input()
{
    unsigned long npts = 0L;

    if ((x = (float *)malloc(nmax * sizeof(float))) == NULL ||
	(y = (float *)malloc(nmax * sizeof(float))) == NULL ||
	(wk1 = (float *)malloc(64 * nmax * sizeof(float))) == NULL ||
	(wk2 = (float *)malloc(64 * nmax * sizeof(float))) == NULL) {
	if (x) (void)free(x);
	fclose(ifile);
	error("insufficient memory");
    }

    while (fscanf(ifile, "%f%f", &x[npts], &y[npts]) == 2) {
        if (++npts >= nmax) {	// double the size of the input buffers 
	    float *xt, *yt, *w1t, *w2t;
	    unsigned long nmaxt = nmax << 1;

	    if (64 * nmaxt * sizeof(float) < nmax) {
		fprintf(stderr,
		      "%s: insufficient memory, truncating input at row %d\n",
		      pname, npts);
	        break;
	    }
	    if ((xt = (float *)realloc(x, nmaxt * sizeof(float))) == NULL) {
		fprintf(stderr,
		      "%s: insufficient memory, truncating input at row %d\n",
		      pname, npts);
	        break;
	    }
	    x = xt;
	    if ((yt = (float *)realloc(y, nmaxt * sizeof(float))) == NULL) {
		fprintf(stderr,
		      "%s: insufficient memory, truncating input at row %d\n",
		      pname, npts);
	        break;
	    }
	    y = yt;
	    if ((w1t = (float *)realloc(wk1,64*nmaxt*sizeof(float))) == NULL){
		fprintf(stderr,
		      "%s: insufficient memory, truncating input at row %d\n",
		      pname, npts);
	        break;
	    }
	    wk1 = w1t;
	    if ((w2t = (float *)realloc(wk2,64*nmaxt*sizeof(float))) == NULL){
		fprintf(stderr,
		      "%s: insufficient memory, truncating input at row %d\n",
		      pname, npts);
	        break;
	    }
	    wk2 = w2t;
	    nmax = nmaxt;
	}
    }

    fclose(ifile);
    if (npts < 1) error("no data read");
    return (npts);
}*/

/* This function calculates the mean of all sample values and subtracts it
   from each sample value, so that the mean of the adjusted samples is zero. */

void CECGsp::zeromean(unsigned long n,float *y)
{
    unsigned long i;
    double ysum = 0.0;

    for (i = 0; i < n; i++)
	ysum += y[i];
    ysum /= n;
    for (i = 0; i < n; i++)
	y[i] -= ysum;
}


CECGsp::~CECGsp()
{
	if (PeakDect != 0) delete PeakDect;
	if (Envenlope != 0) delete [] Envenlope;
	if (FT != 0) delete [] FT;
	delete [] x;
	delete [] wk1;
	delete [] wk2;
	x = 0;
	wk1 = 0;
	wk2 = 0;
}
