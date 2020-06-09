#ifndef __U_STREAMING_PROCESSOR__
#define __U_STREAMING_PROCESSOR__

#include <memory>
#include <vector>
#include <utility>

#include "mydeq.h"
#include "FFT.h"

class streaming_processor {
private:
	std::shared_ptr<mydeq<double>> buf_fft_;
	std::vector<double> fft_result_;
	std::vector<double> buf_fft_window_;
	std::vector<double> buf_double_tmp_0_;
	std::vector<double> buf_double_tmp_1_;

	std::vector<double> spectrum_0_;
	std::vector<double> spectrum_1_;

	int size_buf_fft_;
	std::shared_ptr<FFT> fft_;

public:
	streaming_processor(int size_buf_fft, int size_buf_tmp);
	void push_data(short* buf, int num);
	void do_fft();
	void get_wave(std::vector<short>& buf, int offset, int len);

private:
	void calc_hamming_window();
	std::pair<double, double> get_quadratic_interpolation_peak(double x0, double y0, double x1, double y1, double x2, double y2);
};

#endif
