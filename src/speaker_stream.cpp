#include "speaker_stream.h"
#include <memory.h>
#include <stdio.h>
#include "boost_util.h"
#include <sstream>

using namespace std;

speaker_stream::speaker_stream(int sampling_rate)
{
	sampling_rate_ = sampling_rate;
}

void speaker_stream::set_volume(double volume)
{
	alSourcef(source_, AL_GAIN, volume);
}

void speaker_stream::start()
{
	alGenSources(1, &source_);

	for(int i=0;i<num_buf_;++i){
		play_buf pb;
		alGenBuffers(1, &(pb.buf_));
		pb.in_queue_ = false;
		play_buf_.push_back(pb);
		_ld << "pb.buf_:" << pb.buf_;
		alBufferData(pb.buf_, AL_FORMAT_MONO16, NULL, 0, sampling_rate_);
		alSourceQueueBuffers(source_, 1, &(pb.buf_));
	}
	alSourcePlay(source_);
}

void speaker_stream::push_data(const short* buf, int size)
{
	std::shared_ptr<vector<short>> d = make_shared<vector<short>>();
	d->resize(size);
	memcpy(&(*(d.get()))[0], buf, size * sizeof(short));
	data_.push_back(d);
}

void speaker_stream::update()
{
	int processed;
	alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);
	while(processed--){
		ALuint buf;
		alSourceUnqueueBuffers(source_, 1, &buf);
		_ld << "processed buf id:" << buf;
		for(auto ite = play_buf_.begin();ite != play_buf_.end();++ite){
			if(buf == ite->buf_){
				ite->in_queue_ = false;
				break;
			}
		}
	}

	for(auto ite = data_.begin();ite != data_.end();){
		int idx = get_available_buf_idx();
		if(idx < 0){
			++ite;
			_le << "get_available_buf_idx() full";
			break;
		}
		_ld << "size:" << (*ite)->size();
		_ld << "play_buf_:" << play_buf_[idx].buf_;

/*	
		int len = (*ite)->size();
		vector<short> bb;
		static int offset0 = 0;
		for(int i=0;i<len;++i){
			bb.push_back((SHRT_MAX / 2) * cos((i+offset0)*0.5));
		}
		offset0 += len;
		alBufferData(play_buf_[idx].buf_, AL_FORMAT_MONO16, &bb[0], bb.size() * sizeof(short), sampling_rate_);
*/

		alBufferData(play_buf_[idx].buf_, AL_FORMAT_MONO16, &(*(ite->get()))[0], (*ite)->size() * sizeof(short), sampling_rate_);
		alSourceQueueBuffers(source_, 1, &play_buf_[idx].buf_);
		play_buf_[idx].in_queue_ = true;
		ite = data_.erase(ite);
	}

	ALint state;
	alGetSourcei(source_, AL_SOURCE_STATE, &state);
	char cc[80];
	sprintf(cc, "%X", state);
	_ld << "state:" << cc;
	if(state != AL_PLAYING){
		_ld << "replay";
		alSourcePlay(source_);
	}
}

int speaker_stream::get_available_buf_idx()
{
	for(int i=0;i<num_buf_;++i){
		if(!play_buf_[i].in_queue_){
			return i;
		}
	}
	return -1;
}
