#include <stdio.h>
#include <string>
#include <iostream>

#include "mic.h"

using namespace std;

bool mic::open(const std::string& device_name, int sampling_rate)
{
	buf_ = std::make_shared<std::vector<short>>();
	size_buf_ = 1024*1024;
	buf_->resize(size_buf_);
	mic_ = alcCaptureOpenDevice(device_name.size()==0?NULL:device_name.c_str(), sampling_rate, AL_FORMAT_MONO16, size_buf_);
	return mic_ != NULL;
}

void mic::close()
{
	alcCaptureCloseDevice(mic_);
}

std::pair<int, std::shared_ptr<std::vector<short>>> mic::get_data()
{
	ALint sample;
	alcGetIntegerv(mic_, ALC_CAPTURE_SAMPLES, 1, &sample);
	if(sample < 0){
		return std::make_pair(0, buf_);
	}
	if(buf_->size() < sample){
		std::cerr << "mic::get_data buf size is small. expand it. it take cost." << std::endl;
		buf_->resize(sample*2);
	}
	alcCaptureSamples(mic_, (ALCvoid*)(&(*(buf_.get()))[0]), sample);
	int len = (int)sample;
	return std::make_pair((int)sample, buf_);
}

void mic::start_capture()
{
	alcCaptureStart(mic_);
}

void mic::stop_capture()
{
	alcCaptureStop(mic_);
}
