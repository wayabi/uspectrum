#ifndef __U_WAV_SEQUENCER__
#define __U_WAV_SEQUENCER__

#include <string>
#include <memory>
#include <vector>

#include "audio_output.h"

class wav_sequencer : public audio_output {
public:
	bool load_from_file(const std::string& path);
	void set_loop(bool flag);
	virtual std::pair<int, std::shared_ptr<std::vector<short>>> get_data() override;
	void seek(long millisec);
	void start();
	void pause();

private:
	int idx_last_get_ = 0;
	long millisec_last_get_;
	long millisec_length_;
	bool flag_playing_ = false;
	bool flag_loop_ = true;
	std::shared_ptr<std::vector<short>> data_;
	int sampling_rate_;

};
#endif
