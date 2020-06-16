#include <stdio.h>

#include <string>
#include <iomanip>

#include "boost_util.h"
#include "arg_parse.h"

using namespace std;

int main(int argc, char** argv)
{
	boost_util::set_log_level("trace", 0);

	arg_parse args;
	string parse_error = args.parse(argc, argv);
	if(parse_error.size() > 0){
		_le << parse_error;
		return 1;
	}

	FILE* f;
	if((f = fopen(args.f_in.c_str(), "rb")) == NULL){
		_le << "file open error:" << args.f_in;
		return 1;
	}

	vector<double> buf;
	int size = 1024*1024;
	buf.resize(size);
	int s = fread(&buf[0], 1, size, f);
	buf.resize(s/sizeof(double));
	fclose(f);

	_li << "loaded buf size: " << buf.size();
	
	int size_out = (buf.size() - args.offset) / 2;
	vector<double> buf_out;
	buf_out.resize(size_out);
	_ld << "size_out:" << size_out;
	for(int i=0;i<size_out;++i){
		double d0 = buf[i*2+args.offset+0];
		double d1 = buf[i*2+args.offset+1];
		buf_out[i] = sqrt(d0*d0+d1*d1);
		_ld << "[" << i+args.offset/2 << "] " << buf_out[i];
	}


	int num_attenuation = 100;
	for(int j=0;j<num_attenuation;++j){
		stringstream ss_f;
		ss_f << args.f_out << "_" << j;
		if((f = fopen(ss_f.str().c_str(), "w")) == NULL){
			_le << "file open error:" << args.f_out;
			return 1;
		}

		stringstream ss;
		ss << std::fixed << std::setprecision(12);
		bool first = true;
		for(int i=0;i<size_out;++i){
			if(first){
				first = false;
			}else{
				ss << ",";
			}
			ss << buf_out[i] * ((double)(i+1)/num_attenuation);
		}
		
		fwrite(ss.str().c_str(), 1, ss.str().size(), f);
		fclose(f);
	}

	return 0;
}
