#ifndef __U_SPEAKER_STREAM__
#define __U_SPEAKER_STREAM__

#include <AL/al.h>
#include <AL/alc.h>

#include <list>
#include <memory>
#include <vector>

class speaker_stream {
public:
	class play_buf {
	public:
		ALuint buf_;
		bool in_queue_;
	};

	speaker_stream(int sampling_rate);
	void start();
	void push_data(const short* buf, int size);
	void update();
	void set_volume(double volume);

private:
	int get_available_buf_idx();

private:
	ALuint source_;
	ALuint size_buf_;
	int num_buf_ = 10;
	std::vector<play_buf> play_buf_;
	std::list<std::shared_ptr<std::vector<short>>> data_;
	int sampling_rate_;

};

#endif
