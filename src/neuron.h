#ifndef __U_NEURON__
#define __U_NEURON__

#include <vector>

class neuron;

class synapse {
public:
	const neuron* input_neuron_;
	double weight_;
};

class neuron {
public:
	neuron();
	neuron(const std::vector<neuron>& input_neuron);
	double calc();

public:
	double bias_;
	double value_;
	std::vector<synapse> synapses_;
};

#endif
