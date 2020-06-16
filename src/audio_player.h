#ifndef __U_AUDIO_PLAYER__
#define __U_AUDIO_PLAYER__

#include <AL/al.h>
#include <AL/alc.h>
#include <memory>
#include <vector>
#include <list>
#include <functional>

#include "boost_util.h"

using namespace std;


class play_info {
public:
	ALuint buffer_;
	ALuint source_;
	long time_end_;
	bool freed_ = false;

	bool free(long t)
	{
		if(t > time_end_){
			alSourceStop(source_);
			alDeleteSources(1, &source_);
			alDeleteBuffers(1, &buffer_);
			freed_ = true;
			return true;
		}
		return false;
	}

	void stop()
	{
		if(!freed_){
			alSourceStop(source_);
		}
	}
};

class audio_player {
public:
	static std::shared_ptr<play_info> play(const std::vector<short>& buf, float volume, int samplingrate)
	{
		if(instance_.get() == nullptr){
			instance_ = std::make_shared<audio_player>();
		}
		return instance_->_play(buf, volume, samplingrate);
	}

	static void update()
	{
		if(instance_.get() == nullptr){
			instance_ = std::make_shared<audio_player>();
		}
		instance_->_update();
	}

private:
	static std::shared_ptr<audio_player> instance_;
	std::shared_ptr<play_info> _play(const std::vector<short>& buf, float volume, int samplingrate)
	{
		ALuint buffer;
		ALuint source;
		alGenBuffers(1, &buffer);
		alGenSources(1, &source);
		alBufferData(buffer, AL_FORMAT_MONO16, &buf[0], buf.size() * sizeof(short), samplingrate);
		alSourcef(source, AL_GAIN, volume);
		alSourcei(source, AL_BUFFER, buffer);
		alSourcePlay(source);
		//printf("time = %d\n", time(NULL));
		long t_end = buf.size()/samplingrate + 3 + time(NULL);
		shared_ptr<play_info> pi = make_shared<play_info>();
		pi->buffer_ = buffer;
		pi->source_ = source;
		pi->time_end_ = t_end;
		playing_.push_back(pi);
		return pi;
	}

	void _update()
	{
		for(auto ite = playing_.begin();ite != playing_.end();){
			if((*ite)->free(time(NULL))){
				ite = playing_.erase(ite);
				//_ld << "stop playing.";
			}else{
				++ite;
			}
		}
	}
	
private:
	std::list<std::shared_ptr<play_info>> playing_;
};

#endif
