#include <utility>
#include <iomanip>
#include <limits.h>
#include <math.h>
#include "streaming_processor.h"
#include "Util.h"
#include "audio_util.h"
#include "boost_util.h"

using namespace std;

void print(const char* tag, const vector<double>& buf, int size) 
{
	return;
	stringstream ss;
	bool first = true;
	for(int i=0;i<size;++i){
		if(i>0){
			ss << ", ";
		}
		ss << fixed << setprecision(8) << buf[i];
	}
	_ld << tag << endl << ss.str();
}

streaming_processor::streaming_processor(int size_buf_fft, int size_buf_tmp)
{
	size_buf_fft_ = size_buf_fft;

	buf_double_tmp_0_.resize(size_buf_tmp);
	buf_double_tmp_1_.resize(size_buf_tmp);

	buf_fft_ = std::make_shared<mydeq<double>>(size_buf_fft_);
	spectrum_0_.resize(size_buf_fft_);
	spectrum_1_.resize(size_buf_fft_);
	for(int i=0;i<spectrum_1_.size();++i){
		spectrum_0_[i] = 0;
		spectrum_1_[i] = 0;
	}
	fft_ = std::make_shared<FFT>(size_buf_fft_);
	calc_hamming_window();
}

void streaming_processor::push_data(short* buf, int num)
{
	for(int i=0;i<num;++i){
		buf_double_tmp_0_[i] = *(buf+i) / (double)SHRT_MAX;
	}
	buf_fft_->push(&buf_double_tmp_0_[0], num);
}

void streaming_processor::do_fft()
{
	if(buf_fft_->get_num_data() < size_buf_fft_) return;
	buf_fft_->get_array(&buf_double_tmp_0_[0]);
	for(int i=0;i<size_buf_fft_;++i){
		buf_double_tmp_0_[i] = buf_double_tmp_0_[i];// * buf_fft_window_[i];
	}
	print("windowed", buf_double_tmp_0_, size_buf_fft_);
	fft_->doFFT(&buf_double_tmp_0_[0]);

	memcpy(&spectrum_0_[0], &buf_double_tmp_0_[0], spectrum_0_.size()*sizeof(double));
	for(int i=0;i<size_buf_fft_;++i){
		if(abs(spectrum_1_[i]) < abs(spectrum_0_[i])){
			spectrum_1_[i] = spectrum_0_[i];
		}else{
			spectrum_1_[i] *= 0.9;
		}
	}

	/*
	for(int i=0;i<size_buf_fft_/2;++i){
		double mag = buf_double_tmp_0_[i*2+0] * buf_double_tmp_0_[i*2+0] +
		buf_double_tmp_0_[i*2+1] * buf_double_tmp_0_[i*2+1];
		spectrum_0_[i] = sqrt(mag);
		spectrum_1_[i] = spectrum_0_[i];// * 0.1 + 0.9 * spectrum_1_[i];
	}
	*/
	print("spec", spectrum_1_, spectrum_1_.size());
	
}

void streaming_processor::get_wave(std::vector<short>& buf, int offset, int len)
{
	memcpy(&buf_double_tmp_0_[0], &spectrum_1_[0], spectrum_1_.size()*sizeof(double));
	buf_double_tmp_0_[0] = 0;
	buf_double_tmp_0_[1] = 0;
	buf_double_tmp_0_[2] = 0;
	buf_double_tmp_0_[3] = 0;

	FFT::invert_fft(&buf_double_tmp_0_[0], &buf_double_tmp_1_[0], size_buf_fft_);
	print("inverted", buf_double_tmp_1_, size_buf_fft_);
	for(int i=0;i<len;++i){
		int idx = (offset+i)%size_buf_fft_;
		buf[i] = buf_double_tmp_1_[idx] * SHRT_MAX;
	}
}

void streaming_processor::calc_hamming_window()
{
		buf_fft_window_.resize(size_buf_fft_);
		for(int i=0;i<size_buf_fft_;++i){
			buf_fft_window_[i] = 0.54 - 0.46 * cos(2*M_PI*i/size_buf_fft_);
		}
}

std::pair<double, double> streaming_processor::get_quadratic_interpolation_peak(double x0, double y0, double x1, double y1, double x2, double y2)
{
	double power_x0 = 0.0;
	double power_x1 = 0.0;
	double power_x2 = 0.0;
	//0
	double m2 = (x0-x1)*(x0-x2);
	power_x2 += y0/m2;
	power_x1 += (-x1-x2)*y0/m2;
	power_x0 += (x1*x2)*y0/m2;
	//1
	m2 = (x1-x0)*(x1-x2);
	power_x2 += y1/m2;
	power_x1 += (-x0-x2)*y1/m2;
	power_x0 += (x0*x2)*y1/m2;
	//2
	m2 = (x2-x0)*(x2-x1);
	power_x2 += y2/m2;
	power_x1 += (-x0-x1)*y2/m2;
	power_x0 += (x0*x1)*y2/m2;

	if(power_x2 == 0.0f){
		return std::make_pair(x1, y1);
	}

	double px1 = power_x1/power_x2;
	double px0 = power_x0/power_x2;
	double x_peak = -px1/2;
	double y_peak = (px0-px1*px1/4)*power_x2;
	return std::make_pair(x_peak, y_peak);
}
