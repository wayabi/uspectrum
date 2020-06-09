#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <vector>
#include <list>
#include <set>
#include <memory>
#include <map>
#include <iomanip>

#include <AL/al.h>
#include <AL/alc.h>
#include <math.h>
#include <iostream>

#include "boost_util.h"
#include "arg_parse.h"
#include "Util.h"
#include "audio_player.h"
#include "wave.h"
#include "FFT.h"

#include <boost/program_options.hpp>

using namespace std;

void print_speaker_device_list()
{
	const char* c = (const char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
	while(*c != '\0'){
		_li << c;
		c += strlen(c)+1;
	}
}

int main(int argc, char** argv)
{
	arg_parse args;
	string parse_error = args.parse(argc, argv);
	if(parse_error.size() > 0){
		_le << parse_error;
		return 1;
	}

	stringstream ss_args;
	args.print(ss_args);
	_li << ss_args.str();

	boost_util::set_log_level(args.log_level, args.log_file);

	ALCdevice* device_ = NULL;
	ALCcontext* context_ = NULL;


	Sound* sound = Read_Wave(args.wav.c_str());
	if(sound == NULL){
		_le << "wav open error:" << args.wav;
	}
	int size = sound->datanum;
	vector<short> data;
	data.resize(size);
	if(sound->stereo16 != NULL){
		//load R channel as monaural channel.
		for(int i=0;i<size;++i){
			data[i] = *((short*)((sound->stereo16)+i));
		}
	}else if(sound->monaural16 != NULL){
		memcpy(&data[0], sound->monaural16, sizeof(short) * size);
	}else{
		_le << "wav must be 16bit.";
		return 1;
	}
	int sampling_rate = sound->samplingrate;
	float duration = (float)size / sampling_rate;
	delete(sound);

	_ld << "loaded data size: " << data.size();
	_ld << "loaded data length: " << std::fixed << std::setprecision(2) << duration << " sec";
	_ld << "loaded data sampling_rate: " << sampling_rate;

	args.offset_fft;
	args.size_buf_fft;
	if(args.offset_fft + args.size_buf_fft > data.size()){
		_le << "offset_fft or size_buf_fft is too large. it should be less or equal " << data.size();
		return 1;
	}
	vector<double> buf_fft;
	buf_fft.resize(args.size_buf_fft);
	for(int i=0;i<args.size_buf_fft;++i){
		buf_fft[i] = (double)data[i] / SHRT_MAX;
	}

	FFT fft(args.size_buf_fft);
	fft.doFFT(&buf_fft[args.offset_fft]);
	FILE* f_out;
	if((f_out = fopen(args.path_out_spectrum.c_str(), "w")) == NULL){
		_le << "file open error: " << args.path_out_spectrum;
		return 1;
	}
	fwrite(&data[args.offset_fft], args.size_buf_fft*4, 1, f_out);
	fclose(f_out);
	_li << "wrote spectrum to: " << args.path_out_spectrum;

	if(args.speaker.size() > 0){
		print_speaker_device_list();
		device_ = alcOpenDevice(args.speaker.c_str());
		if(device_ == NULL){
			_le << "invalid device name:" << args.speaker;
			return 1;
		}
		context_ = alcCreateContext(device_, NULL);
		if(context_ == NULL){
			_le << "invalid AL context.";
			return 1;
		}
		alcMakeContextCurrent(context_);
	}

	//play

	if(context_ != NULL){
		alcDestroyContext(context_);
		alcCloseDevice(device_);
	}

	return 0;
}
