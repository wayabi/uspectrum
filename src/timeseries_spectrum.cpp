#include "timeseries_spectrum.h"
#include <iostream>
#include <string>

#include "FFT.h"
#include "boost_util.h"

using namespace std;

timeseries_spectrum::timeseries_spectrum()
{

}

std::vector<double> timeseries_spectrum::get(double t)
{
	vector<double> ret;
	if(data_.size() == 0) return ret;
	int size = data_[0].second.size();
	ret.resize(size);
	auto ite0 = data_.begin();
	auto ite1 = data_.begin();
	for(auto ite = data_.begin();ite != data_.end();++ite){
		if(ite->first <= t){
			ite0 = ite;
		}
		if(ite->first >= t){
			ite1 = ite;
			break;
		}
	}
	double a = 0;
	if(ite1->first > ite0->first){
		a = (t - ite0->first) / (ite1->first - ite0->first);
	}
	for(int i=0;i<size;++i){
		ret[i] = ite0->second[i] * a + ite1->second[i] * (1-a);
	}
	return ret;
}

void timeseries_spectrum::calc_from_audio(const std::vector<double>& data, int size_fft)
{
	int num = data.size() / size_fft;
	double unit = 1.0 / num;
	vector<double> buf;
	buf.resize(size_fft);
	FFT fft(size_fft);
	for(int i=0;i<num;++i){
		memcpy(&buf[0], &data[i*size_fft], size_fft);
		fft.doFFT(&buf[0]);
		for(int j=0;j<size_fft/2;++j){
			buf[j*2+1] = sqrt(buf[j*2+0]*buf[j*2+0]+buf[j*2+1]*buf[j*2+1]);
			buf[j*2+0] = 0;
		}
		data_.push_back(make_pair(i*unit, buf));
	}
	memset(&buf[0], 0, buf.size());
	data_.push_back(make_pair(1, buf));
}

bool timeseries_spectrum::load(const char* path)
{
	FILE* f;
	if((f = fopen(path, "rb")) == NULL){
		_le << "file open error: " << path;
		return false;
	}
	int num;
	fread(&num, 1, sizeof(int), f);
	int size_fft;
	fread(&size_fft, 1, sizeof(int), f);
	data_.clear();
	for(int i=0;i<num;++i){
		double a;
		fread(&a, 1, sizeof(double), f);
		vector<double> buf;
		buf.resize(size_fft);
		fread(&buf[0], 1, size_fft*sizeof(double), f);
		data_.push_back(make_pair(a, buf));
	}

	fclose(f);
	return true;
}

bool timeseries_spectrum::save(const char* path)
{
	if(data_.size() == 0){
		_le << "timeseries_spectrum.data_.size() == 0";
		return false;
	}
	FILE* f;
	if((f = fopen(path, "wb")) == NULL){
		_le << "file open error: " << path;
		return false;
	}
	int num = data_.size();
	int size_fft = data_[0].second.size();
	fwrite(&num, 1, sizeof(int), f);
	fwrite(&size_fft, 1, sizeof(int), f);
	for(int i=0;i<num;++i){
		fwrite(&(data_[i].first), 1, sizeof(double), f);
		fwrite(&(data_[i].second)[0], 1, size_fft*sizeof(double), f);
	}
	fclose(f);
	return true;
}

