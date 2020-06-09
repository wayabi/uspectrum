#include "neuron.h"

using namespace std;

neuron::neuron()
{
}

neuron::neuron(const std::vector<neuron>& input_neuron)
{
	for(auto ite = input_neuron.begin();ite != input_neuron.end();++ite){
		synapse s;
		s.input_neuron_ = &(*ite);
		synapses_.push_back(s);
	}
}

double neuron::calc()
{
	double sum = 0;
	for(auto ite = synapses_.begin();ite != synapses_.end();++ite){
		sum += ite->weight_ * ite->input_neuron_->value_;
	}
	return sum + bias_;
}
