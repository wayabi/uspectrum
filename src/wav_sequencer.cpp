#include "wav_sequencer.h"
#include "wave.h"
#include <string.h>
#include <chrono>
#include <iostream>

using namespace std;

long get_ms()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

bool wav_sequencer::load_from_file(const std::string& path)
{
	Sound* sound = Read_Wave(path.c_str());
	if(sound == NULL) return false;
	int size = sound->datanum;
	data_ = make_shared<std::vector<short>>();
	data_->resize(size);
	if(sound->stereo16 != NULL){
		//load R channel as monaural channel.
		for(int i=0;i<size;++i){
			(*(data_.get()))[i] = *((short*)((sound->stereo16)+i));
		}
	}else if(sound->monaural16 != NULL){
		memcpy(&(*(data_.get()))[0], sound->monaural16, sizeof(short) * size);
	}else{
		return false;
	}
	sampling_rate_ = sound->samplingrate;
	millisec_length_ = (float)size / sampling_rate_;
	delete(sound);

	cout << "wav_sequencer::load_from_file(" << path << "). size = " << data_->size() << endl;
	return true;
}

void wav_sequencer::set_loop(bool flag)
{
	flag_loop_ = flag;
}

std::pair<int, std::shared_ptr<std::vector<short>>> wav_sequencer::get_data()
{
	if(!flag_playing_){
		return make_pair<int, shared_ptr<vector<short>>>(0, nullptr);
	}
	long current = get_ms();
	long t = current - millisec_last_get_;
	int num = int((double)t * sampling_rate_ / 1000);
	
	pair<int, shared_ptr<vector<short>>> ret = make_pair(num, make_shared<vector<short>>());

	auto d = ret.second.get();
	auto src = &(*(data_.get()))[0];
	if(flag_loop_){
		d->resize(num);
		if(idx_last_get_ + num > data_->size()){
			int num0 = data_->size() - idx_last_get_;
			int num1 = num - num0;
			memcpy(&(*d)[0], src+idx_last_get_, sizeof(short) * num0);
			memcpy(&(*d)[num0], src+0, sizeof(short) * num1);
			idx_last_get_ = num1;
		}else{
			memcpy(&(*d)[0], src+idx_last_get_, sizeof(short) * num);
			idx_last_get_ += num;
		}
	}else{
		if(idx_last_get_ + num >= data_->size()){
			int num0 = data_->size() - idx_last_get_;
			d->resize(num0);
			ret.first = num0;
			memcpy(&(*d)[0], src+idx_last_get_, sizeof(short) * num0);
			idx_last_get_ += num0;
			flag_playing_ = false;
		}else{
			d->resize(num);
			memcpy(&(*d)[0], src+idx_last_get_, sizeof(short) * num);
			idx_last_get_ += num;
		}
		
	}
	millisec_last_get_ = current;
	return ret;
}

void wav_sequencer::seek(long millisec)
{
	idx_last_get_ = millisec * sampling_rate_ / 1000;
	if(idx_last_get_ >= data_->size()){
		if(flag_loop_){
			idx_last_get_ = idx_last_get_ % data_->size();
		}else{
			flag_playing_ = false;
		}
	}
}

void wav_sequencer::start()
{
	if(!flag_playing_){
		if(data_->size() > 0){
			millisec_last_get_ = get_ms();
			flag_playing_ = true;
		}
	}
}

void wav_sequencer::pause()
{
	flag_playing_ = false;
}
