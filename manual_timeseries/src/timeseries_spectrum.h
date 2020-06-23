#ifndef __U_TIMESERIES_SPECTRUM__
#define __U_TIMESERIES_SPECTRUM__

#include <vector>
#include <utility>

class timeseries_spectrum {
public:
	timeseries_spectrum();
	std::vector<double> get(double t);
	void calc_from_audio(const std::vector<double>& data, int size_fft);
	bool load(const char* path);
	bool save(const char* path);

public:
	std::vector<std::pair<double, std::vector<double>>> data_;
};
#endif
