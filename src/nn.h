#ifndef __U_NN__
#define __U_NN__

#include <vector>
#include <functional>
#include "neuron.h"

class nn {
public:
	nn(std::function<double(double)> f_activate);
	void init(std::vector<int> layers);
	bool load(const char* path);
	std::vector<double> forward_propagate(const std::vector<double>& input);

	static double sigmoid(double x);
	static double relu(double x);

public:
	std::vector<std::vector<neuron>> layers_;

private:
	std::function<double(double)> f_activate_;
};

#endif
