#include "nn.h"
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>

using namespace std;

nn::nn(std::function<double(double)> f_activate)
{
	f_activate_ = f_activate;
}

void nn::init(std::vector<int> layers)
{
	int num = layers.size();
	for(int i=0;i<num;++i){
		vector<neuron> v;
		if(i == 0){
			for(int j=0;j<layers[i];++j){
				neuron n;
				v.push_back(n);
			}
		}else{
			for(int j=0;j<layers[i];++j){
				neuron n(layers_[i-1]);
				v.push_back(n);
			}
		}
		layers_.push_back(v);
	}
}

std::vector<double> nn::forward_propagate(const std::vector<double>& input)
{
	for(int i=0;i<layers_.size();++i){
		vector<neuron>& layer = layers_[i];
		if(i == 0){
			for(int j=0;j<layer.size();++j){
				layer[j].value_ = input[j];
			}
		}else if(i == layers_.size()-1){
			//softmax
			double max = SHRT_MIN;
			for(int j=0;j<layer.size();++j){
				double v = layer[j].calc();
				layer[j].value_ = v;
				if(v > max) max = v;
			}
			double sum = 0;
			for(int j=0;j<layer.size();++j){
				double a = exp(layer[j].value_ - max);
				sum += a;
			}
			for(int j=0;j<layer.size();++j){
				double a = exp(layer[j].value_ - max);
				layer[j].value_ = a / sum;
			}
		}else{
			for(int j=0;j<layer.size();++j){
				layer[j].value_ = f_activate_(layer[j].calc());
			}
		}
	}
	vector<double> ret;
	auto layer = layers_[layers_.size()-1];
	for(auto ite = layer.begin();ite != layer.end();++ite){
		ret.push_back(ite->value_);
	}
	return ret;
}

double nn::sigmoid(double x)
{
	return (double)(1.0 / (1.0 + exp(-x)));
}

double nn::relu(double x)
{
	return (x < 0 ? 0 : x);
}

bool nn::load(const char* path)
{
	FILE* f = fopen(path, "rb");
	if(f == NULL){
		cerr << "file open failed:" << path << endl;
		return false;
	}

	int num_layer;
	fread(&num_layer, 1, 4, f);
	cout << "num_layer:" << num_layer << endl;
	vector<int> layers;
	for(int i=0;i<num_layer;++i){
		int a;
		fread(&a, 1, 4, f);
		cout << "layer[" << i << "]: " << a << endl;
		layers.push_back(a);
	}
	init(layers);

	for(int i=0;i<num_layer;++i){
		if(layers[i] != layers_[i].size()){
			cerr << "invalid layer size: needs: " << layers_[i].size() << " file's: " << layers[i] << endl;
			fclose(f);
			return false;
		}
	}

	for(int i=1;i<num_layer;++i){
		int num_layer0 = layers[i-1];
		int num_layer1 = layers[i];
		for(int j=0;j<num_layer1;++j){
			double b;
			fread(&b, 1, 4, f);
			layers_[i][j].bias_ = b;
			//cout << b << ", ";
		}
		//cout << endl;

		for(int j=0;j<num_layer0;++j){
			for(int k=0;k<num_layer1;++k){
				double b;
				fread(&b, 1, 4, f);
				layers_[i][k].synapses_[j].weight_ = b; 
				//cout << b << ", ";
			}
			//cout << endl;
		}
	}

	fclose(f);
	return true;
}
