#ifndef __U_ARG_PARSE__
#define __U_ARG_PARSE__

#include <string>
#include <ostream>

#include <boost/program_options.hpp>

class arg_parse {

public:
	std::string f_in;
	std::string f_out;
	int offset;
public:
	std::string parse(int argc, char** argv)
	{
		using namespace boost::program_options;
		options_description opt("opt");
		opt.add_options()
			("f_in", value<std::string>()->required(), "f_in")
			("f_out", value<std::string>()->required(), "f_out")
			("offset", value<int>()->required(), "offset")
		;
		variables_map vm;
		try{
			store(parse_command_line(argc, argv, opt), vm);
			notify(vm);
			f_in = vm["f_in"].as<std::string>();
			f_out = vm["f_out"].as<std::string>();
			offset = vm["offset"].as<int>();
		}catch(std::exception& e){
			return std::string(e.what());
		}
		return std::string();
	}

	void print(std::ostream& o)
	{
		o << "f_in[std::string]: " << f_in << std::endl;
		o << "f_out[std::string]: " << f_out << std::endl;
		o << "offset[int]: " << offset << std::endl;
	}
};
#endif
