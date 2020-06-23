#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <vector>
#include <utility>

#include "timeseries_spectrum.h"

using namespace std;

int main(int argc, char** args)
{
	timeseries_spectrum ts_;
	int size_fft = 128;
	vector<double> buf;
	double a = 0;
	buf.resize(size_fft);

	a = 1.0;
	memset(&buf[0], 0, size_fft * sizeof(double));
	buf[5] = 0.1;
	buf[13] = 0.8;
	buf[25] = 0.6;
	buf[37] = 0.4;
	buf[73] = 0.2;
	for(int i=0;i<size_fft;++i) buf[i] *= a;
	ts_.data_.push_back(make_pair(a, buf));

	a = 0.5;
	memset(&buf[0], 0, size_fft * sizeof(double));
	buf[5] = 0.4;
	buf[13] = 0.8;
	buf[25] = 0.6;
	buf[37] = 0.4;
	buf[73] = 0.2;
	buf[125] = 0.6;
	for(int i=0;i<size_fft;++i) buf[i] *= a;
	ts_.data_.push_back(make_pair(a, buf));

	a = 0.2;
	memset(&buf[0], 0, size_fft * sizeof(double));
	for(int i=0;i<size_fft;++i) buf[i] *= a;
	ts_.data_.push_back(make_pair(a, buf));

	a = 0;
	memset(&buf[0], 0, size_fft * sizeof(double));
	for(int i=0;i<size_fft;++i) buf[i] *= a;
	ts_.data_.push_back(make_pair(a, buf));

	ts_.save("./out/a");
	return 0;
}
