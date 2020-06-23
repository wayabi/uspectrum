#ifndef __RP_FFT__
#define __RP_FFT__

#include <vector>

class FFT {
public:
	FFT(int size_buf_);
	virtual ~FFT(void);
	void doFFT(double *buf);
	static int get_nearest_2_exponentiation(int a);
	static void invert_fft(double* spectrum, double* out, int size_buf);
	static bool is2exponentiation(int a);

private:
	int size_buf;
	int *ip;
	double *w;
	std::vector<double> hanning_window;

	void rdft(int n, int isgn, double *a, int *ip, double *w);
	


};

#endif
