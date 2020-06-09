#ifndef __U_SM_MIC__
#define __U_SM_MIC__

#include <functional>

#include "sm.h"
#include "audio_output.h"
#include "e_mic.h"

#include "s_mic_wait_rec.h"
#include "s_mic_recording.h"

#include "streaming_processor.h"
#include "audio_data.h"

class sm_mic : public sm {
public:
	void init(std::shared_ptr<audio_output> ao, std::function<void(int, int, int, int, int)> func_on_audio_event, std::function<void(std::shared_ptr<audio_data>)> func_on_audio_data){
		func_on_audio_event_ = func_on_audio_event;
		func_on_audio_data_ = func_on_audio_data;
		audio_output_ = ao;

		idx_audio_ = 0;

		add_event<s_mic_wait_rec, e_mic_start_rec, s_mic_recording>();
		add_event<s_mic_recording, e_mic_stop_rec, s_mic_wait_rec>();

		buf_voice_ = std::make_shared<mydeq<short>>(size_buf_);
		sp_ = std::make_shared<streaming_processor>(size_buf_, (int)(constants::mergin_voice_start_sec * constants::sampling_rate), (int)(constants::mergin_voice_end_sec * constants::sampling_rate));
		set_initial_state<s_mic_wait_rec>();
	}

	std::shared_ptr<audio_output> audio_output_;
	std::shared_ptr<streaming_processor> sp_;
	std::shared_ptr<mydeq<short>> buf_voice_;
	//buf mergin 1sec
	const int size_buf_ = (int)constants::sampling_rate * (constants::max_rec_length + 1.0f);
	std::shared_ptr<tcp_sender> tcp_sender_;
	std::function<void(int, int, int, int, int)> func_on_audio_event_;
	std::function<void(std::shared_ptr<audio_data>)> func_on_audio_data_;
	int idx_audio_;
};

#endif
