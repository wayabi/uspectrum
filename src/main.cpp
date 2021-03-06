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
#include "StdInThread.h"
#include "audio_data.h"
#include "audio_player.h"
#include "timeseries_spectrum.h"

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

int size_ = 0;
vector<short> data_;



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

	print_speaker_device_list();

	ALCdevice* device_ = NULL;
	ALCcontext* context_ = NULL;
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

	StdInThread stdin_thread;
	stdin_thread.ss.push_back("l,a.wav");
	stdin_thread.start(NULL);
	audio_data ad_;
	int range_offset_ = 0;
	int range_length_ = 0;
	double volume_ = 1.0;

	while(true){
		usleep(10*1000);

		audio_player::update();

		stdin_thread.ss.lock();
		vector<char> c = stdin_thread.ss.head();
		stdin_thread.ss.erase();
		stdin_thread.ss.unlock();

		string s = Util::trim(string(&c[0], c.size()), "\n");
		vector<string> command = Util::split(s, ',');
		if(command.size() > 0){
			if(command[0] == "q"){
				break;
			}else if(command.size() >= 2 && command[0] == "l"){
				audio_data ad;
				if(!ad.load(command[1].c_str())){
					_le << "wav load failed: " << command[1];
					continue;
				}
				ad_ = ad;
				range_offset_ = 0;
				range_length_ = ad_.buf_s_.size();
			}else if(command[0] == "h"){
				_li << "q : quit";
				_li << "l <path_wav> : load";
				_li << "h : help";
				_li << "s : status";
				_li << "r <range_offset> <range_length> : set range";
				_li << "p : play";
				_li << "fo <path_out> : fft output";
				_li << "fp : fft play";
				_li << "ft : fft time serices save";
				_li << "v <volume> : set volume play";
				_li << "lf <path> : load spectrum data";
			}else if(command[0] == "s"){
				_li << "length: " << ad_.buf_s_.size();
				_li << "sampling_rate: " << ad_.sampling_rate_;
			}else if(command[0] == "r"){
				if(ad_.buf_s_.size() == 0) continue;
				if(command.size() < 2){
					_li << "r <range_offset> <range_length> : set range";
					continue;
				}
			
				int offset = atoi(command[1].c_str());
				int len = 256;
				if(command.size() >= 3){
					len = atoi(command[2].c_str());
				}
				if(offset + len > ad_.buf_s_.size() || offset < 0 || len < 0){
					_le << "invalid args, offset + len should be less or equal [" << ad_.buf_s_.size() << "]";
					continue;
				}
				range_offset_ = offset;
				range_length_ = len;
				_li << "range_offset_ = " << range_offset_;
				_li << "range_length_ = " << range_length_;

				vector<short> buf;
				buf.resize(range_length_);
				memcpy(&buf[0], &(ad_.buf_s_)[range_offset_], range_length_ * sizeof(short));
				_li << "playing: " << buf.size();
				audio_player::play(buf, volume_, ad_.sampling_rate_);

			}else if(command[0] == "p"){
				vector<short> buf;
				buf.resize(range_length_);
				memcpy(&buf[0], &(ad_.buf_s_)[range_offset_], range_length_ * sizeof(short));
				_li << "playing: " << buf.size();
				audio_player::play(buf, volume_, ad_.sampling_rate_);
			}else if(command[0] == "fp"){
				if(!FFT::is2exponentiation(range_length_)){
					_le << "range_length_ should be 2_exponentiation. e.g. " << 512;
					continue;
				}
				if(ad_.buf_s_.size() == 0){
					_le << "ad not be loaded yet.";
					continue;
				}
				vector<double> buf_fft0;
				vector<double> buf_fft1;
				vector<short> buf_wave;
				buf_fft0.resize(range_length_);
				buf_fft1.resize(range_length_);
				int num = (ad_.sampling_rate_ * 3) / range_length_;
				int size_wave = num * range_length_;
				buf_wave.resize(size_wave);
				for(int i=0;i<range_length_;++i){
					buf_fft0[i] = (double)ad_.buf_s_[i+range_offset_] / SHRT_MAX;
				}
				FFT fft(range_length_);
				fft.doFFT(&buf_fft0[0]);
				//high pass filter
				for(int i=0;i<8;++i){
					buf_fft0[i] = 0;
				}
				
				for(int i=0;i<range_length_/2;++i){
					//buf_fft0[i*2+1] = sqrt(buf_fft0[i*2+0]*buf_fft0[i*2+0]+buf_fft0[i*2+1]*buf_fft0[i*2+1]);
					//buf_fft0[i*2+0] = 0;
				}
				

				FFT::invert_fft(&buf_fft0[0], &buf_fft1[0], range_length_);
				for(int i=0;i<num;++i){
					for(int j=0;j<range_length_;++j){
						buf_wave[i*range_length_+j] = (short)(buf_fft1[j] * SHRT_MAX);
					}
				}

				if(false){
					Sound s;
					s.monaural16 = &buf_wave[0];
					s.channelnum = 1;
					s.samplingrate = 8000;
					s.bit_per_sample = 16;
					s.datanum = buf_wave.size();
					Write_Wave("./z.wav", &s);
				}


				audio_player::play(buf_wave, volume_, ad_.sampling_rate_);
			}else if(command[0] == "fo"){
				if(!FFT::is2exponentiation(range_length_)){
					_le << "range_length_ should be 2_exponentiation. e.g. " << 512;
					continue;
				}
				if(command.size() < 2){
					_le << "fo needs <path_out>";
					continue;
				}
				FILE* f_out;
				if((f_out = fopen(command[1].c_str(), "wb")) == NULL){
					_le << "file open error: " << command[1];
					continue;
				}

				vector<double> buf_fft;
				buf_fft.resize(range_length_);
				for(int i=0;i<range_length_;++i){
					buf_fft[i] = (double)ad_.buf_s_[i+range_offset_] / SHRT_MAX;
				}
				FFT fft(range_length_);
				fft.doFFT(&buf_fft[0]);
				//high pass filter
				for(int i=0;i<8;++i){
					buf_fft[i] = 0;
				}
				for(int i=0;i<range_length_/2;++i){
					//buf_fft[i*2+1] = sqrt(buf_fft[i*2+0]*buf_fft[i*2+0]+buf_fft[i*2+1]*buf_fft[i*2+1]);
					//buf_fft[i*2+0] = 0;
				}

				fwrite(&buf_fft[0], range_length_ * sizeof(double), 1, f_out);
				fclose(f_out);
				_li << "wrote spectrum to: " << command[1];

				stringstream ss_fft;
				for(auto ite = buf_fft.begin();ite != buf_fft.end();++ite){
					ss_fft << *ite << ", ";
				}
				_lt << ss_fft.str();
				_li << "save data. size: " << range_length_;

			}else if(command[0] == "ft"){
				if(command.size() < 3){
					_le << "ft needs <path_out> <size_fft>";
					continue;
				}
				vector<double> buf;
				buf.resize(range_length_);
				for(int i=0;i<range_length_;++i){
					buf[i] = (double)ad_.buf_s_[i] / SHRT_MAX;
				}
				timeseries_spectrum ts;
				int size_fft = atoi(command[2].c_str());
				if(!FFT::is2exponentiation(size_fft)){
					_le << "<size_fft> should be 2 exponentiation.";
					continue;
				}
				ts.calc_from_audio(buf, size_fft);
				ts.save(command[1].c_str());
				_li << "save: " << command[1];
			}else if(command[0] == "v" && command.size() >= 2){
				double v = atof(command[1].c_str());
				if(v >= 0.0 && v <= 1.0){
					_li << "set volume: " << v;
					volume_ = v;
				}
			}else if(command[0] == "lf"){
				if(command.size() < 2){
					_le << "lf <path_spectrum>";
					continue;
				}
				FILE* f;
				if((f = fopen(command[1].c_str(), "rb")) == NULL){
					_le << "file open error: " << command[1];
					continue;
				}
				int size_file = Util::getSizeFile(command[1].c_str());
				if(!FFT::is2exponentiation(size_file)){
					_le << "file size must be 2 exponentiation. current:" << size_file;
					fclose(f);
					continue;
				}
				vector<double> spectrum;
				int size_fft = size_file/sizeof(double);
				_ld << "size_fft: " << size_fft;
				spectrum.resize(size_fft);
				fread(&spectrum[0], 1, size_file, f);
				fclose(f);

				int sampling_rate = 8000;
				range_offset_ = 0;
				range_length_ = sampling_rate * 3;
				vector<double> buf0;
				buf0.resize(size_fft);
				vector<short> buf1;
				buf1.resize(range_length_);
				FFT::invert_fft(&spectrum[0], &buf0[0], size_fft);

				/*
				vector<double> buf2;
				buf2.resize(size_fft);
				for(int i=0;i<size_fft;++i){
					double a = (i>size_fft/2? -2*i/(double)size_fft +2: 2*i/(double)size_fft);
					int j=(i+size_fft/2)%size_fft;
					buf2[i] = a*buf0[i] + (1-a)*buf0[j];
				}
				buf0 = buf2;
				*/
				int num = range_length_ / size_fft + (range_length_%size_fft==0?0:1);
				for(int i=0;i<num;++i){
					int size = (i==num-1?range_length_-i*size_fft:size_fft);
					for(int j=0;j<size;++j){
						buf1[i*size_fft+j] = (short)(buf0[j]*SHRT_MAX);
					}
				}

				audio_data ad;
				ad.buf_s_ = buf1;
				ad.sampling_rate_ = sampling_rate;
				ad_ = ad;
				_li << command[1] << " loaded as spectrum.";
			}
		}
	}




	//play

	if(context_ != NULL){
		alcDestroyContext(context_);
		alcCloseDevice(device_);
	}

	return 0;
}
