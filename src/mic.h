#ifndef __U_MIC__
#define __U_MIC__

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <memory>

#include "audio_output.h"

class mic : public audio_output {
private:
	ALCdevice* mic_;
	std::shared_ptr<std::vector<short>> buf_;
	int size_buf_;

public:
	bool open(const std::string& device_name, int sampling_rate);
	void close();
	void start_capture();
	void stop_capture();
	virtual std::pair<int, std::shared_ptr<std::vector<short>>> get_data() override;
};

#endif
