#include "audio_data.h"

#include <iostream>
#include <string>
#include <iomanip>

#include "wave.h"
#include "boost_util.h"

using namespace std;

audio_data::audio_data()
{
	sampling_rate_ = 0;
}

bool audio_data::load(const char* path)
{
	Sound* sound = Read_Wave(path);
	if(sound == NULL){
		_le << "wav open error:" << path;
		return false;
	}
	int size = sound->datanum;
	buf_s_.resize(size);
	if(sound->stereo16 != NULL){
		//load R channel as monaural channel.
		for(int i=0;i<size;++i){
			buf_s_[i] = *((short*)((sound->stereo16)+i));
		}
	}else if(sound->monaural16 != NULL){
		memcpy(&buf_s_[0], sound->monaural16, sizeof(short) * size);
	}else{
		_le << "wav must be 16bit.";
		return false;
	}
	sampling_rate_ = sound->samplingrate;
	delete(sound);

	_ld << "loaded data size: " << buf_s_.size();
	_ld << "loaded data length: " << std::fixed << std::setprecision(2) << (buf_s_.size() / (double)sampling_rate_) << " sec";
	_ld << "loaded data sampling_rate: " << sampling_rate_;
	return true;
}

void audio_data::s2d()
{
	int size = buf_s_.size();
	buf_d_.resize(size);
	for(int i=0;i<size;++i){
		buf_d_[i] = (double)buf_s_[i] / SHRT_MAX;
	}
}

void audio_data::d2s()
{
	int size = buf_d_.size();
	buf_s_.resize(size);
	for(int i=0;i<size;++i){
		buf_s_[i] = (short)(buf_d_[i] * SHRT_MAX);
	}
}

double audio_data::get_sec()
{
	if(sampling_rate_ <= 0) return 0;
	return (double)buf_s_.size() / sampling_rate_;
}

