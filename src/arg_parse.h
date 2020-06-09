#ifndef __U_ARG_PARSE__
#define __U_ARG_PARSE__

#include <string>
#include <ostream>

#include <boost/program_options.hpp>

class arg_parse {

public:
	std::string log_level;
	int log_file;
	std::string speaker;
public:
	std::string parse(int argc, char** argv)
	{
		using namespace boost::program_options;
		options_description opt("opt");
		opt.add_options()
			("log_level", value<std::string>()->default_value("trace"), "log_level")
			("log_file", value<int>()->default_value(0), "log_file")
			("speaker", value<std::string>()->default_value(""), "speaker")
		;
		variables_map vm;
		try{
			store(parse_command_line(argc, argv, opt), vm);
			notify(vm);
			log_level = vm["log_level"].as<std::string>();
			log_file = vm["log_file"].as<int>();
			speaker = vm["speaker"].as<std::string>();
		}catch(std::exception& e){
			return std::string(e.what());
		}
		return std::string();
	}

	void print(std::ostream& o)
	{
		o << "log_level[std::string]: " << log_level << std::endl;
		o << "log_file[int]: " << log_file << std::endl;
		o << "speaker[std::string]: " << speaker << std::endl;
	}
};
#endif
