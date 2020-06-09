#ifndef __U_AUDIO_DATA__
#define __U_AUDIO_DATA__

#include <vector>

class audio_data {
public:
	audio_data();
	bool load(const char* path);
	void s2d();
	void d2s();
	double get_sec();

public:
	std::vector<short> buf_s_;
	std::vector<double> buf_d_;
	int sampling_rate_;
};

#endif
