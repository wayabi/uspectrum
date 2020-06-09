#include <stdio.h>
#include <limits.h>

#include <vector>
#include <cmath>

#include "lame/lame.h"
#include "audio_util.h"
#include "Util.h"

using namespace std;

bool audio_util::write_mp3_short(const short* data, int size, int sampling_rate, const std::string& path)
{
	FILE* f = fopen(path.c_str(), "wb");
	if(f == NULL){
		printf("file open error:%s\n", path.c_str());
		return false;
	}
	int size_buf = size / sizeof(short);
	vector<unsigned char> buf_mp3;
	buf_mp3.resize(size_buf);
	lame_t lame = lame_init();
	lame_set_in_samplerate(lame, sampling_rate);
	lame_set_VBR(lame, vbr_default);
	lame_init_params(lame);

	int count = lame_encode_buffer(lame, data, data, size_buf, &buf_mp3[0], size_buf);
	fwrite(&buf_mp3[0], count, 1, f);
	if(count == size_buf){
	  printf("lame_encode_buffer return bigger buffer than source buffer\n");
	}
	count = lame_encode_flush(lame, &buf_mp3[0], size_buf);
	fwrite(&buf_mp3[0], count, 1, f);

	lame_close(lame);
	fclose(f);
}

std::shared_ptr<std::vector<short>> audio_util::double2short(std::shared_ptr<std::vector<double>> d)
{
	auto ret = std::make_shared<std::vector<short>>();
	int size = d->size();
	ret->resize(size);
	double *pd = &(*(d.get()))[0];
	short* ps = &(*(ret.get()))[0];
	for(int i=0;i<size;++i)
	{
		*(ps+i) = (short)(*(pd+i) * SHRT_MAX);
	}
	return ret;
}

std::shared_ptr<std::vector<float>> audio_util::double2float(std::shared_ptr<std::vector<double>> d)
{
	auto ret = std::make_shared<std::vector<float>>();
	int size = d->size();
	ret->resize(size);
	double *pd = &(*(d.get()))[0];
	float* ps = &(*(ret.get()))[0];
	for(int i=0;i<size;++i)
	{
		*(ps+i) = (float)(*(pd+i));
	}
	return ret;
}

std::shared_ptr<std::vector<short>> audio_util::double2short(std::shared_ptr<std::vector<double>> d, float volume)
{
	auto ret = std::make_shared<std::vector<short>>();
	int size = d->size();
	ret->resize(size);
	double *pd = &(*(d.get()))[0];
	short* ps = &(*(ret.get()))[0];
	for(int i=0;i<size;++i)
	{
		*(ps+i) = (short)(*(pd+i) * volume * SHRT_MAX);
	}
	return ret;
}

std::shared_ptr<std::vector<double>> audio_util::short2double(std::shared_ptr<std::vector<short>> s)
{
	auto ret = std::make_shared<std::vector<double>>();
	int size = s->size();
	ret->resize(size);
	short* ps = &(*(s.get()))[0];
	double* pd = &(*(ret.get()))[0];
	for(int i=0;i<size;++i)
	{
		*(pd+i) = (double)*(ps+i) / SHRT_MAX;
	}
	return ret;
}

void audio_util::normalize_and_fade(std::shared_ptr<std::vector<double>> dst, std::shared_ptr<std::vector<double>> src, int mergin_fade, double normal_value)
{
	int size = src->size();
	if(dst->size() < size){
		dst->resize(size);
	}
	double* s = &(*(src.get()))[0];
	double* d = &(*(dst.get()))[0];
	double fade = 1.0;

	double max_v = 0;
	for(int i=0;i<size;++i){
		double v = *(s+i);
		v = std::abs(v);
		if(v > max_v){
			max_v = v;
		}
	}

	normal_value /= max_v;

	for(int i=0;i<size;++i){
		if(i < mergin_fade){
			fade = (double)i/mergin_fade;
		}else if(i>= size-mergin_fade){
			fade = (double)(size-i) / mergin_fade;
		}else{
			fade = 1.0;
		}
		double v = *(s+i) * normal_value * fade;
		if(v > 0.9){
			v = 0.9;
		}else if(v < -0.9){
			v = -0.9;
		}
		*(d+i) = v;
	}
}

std::pair<int, double> audio_util::get_f0(const std::vector<double>& d)
{
	vector<double> buf;
	Util::moving_average(29, d, buf);

	vector<pair<int, double>> peaks;
	double max_mag = 0;
	for(int i=2;i<buf.size()-2;++i){
		if(
				buf[i-2] < buf[i-1] &&
				buf[i-1] < buf[i] &&
				buf[i] > buf[i+1] &&
				buf[i+1] > buf[i+2] 
				){
			peaks.push_back(make_pair(i, d[i]));
			if(d[i] > max_mag){
				max_mag = d[i];
			}
		}
	}
	int num = peaks.size();
	int idx = INT_MAX;
	double mag = 0;
	for(int i=0;i<num;++i){
		if(peaks[i].first < idx){
		//if(peaks[i].second > mag){
			idx = peaks[i].first;
			mag = peaks[i].second;
		}
	}
	if(idx == INT_MAX){
		return make_pair(-1, -1);
	}
	return make_pair(idx, mag);
}
