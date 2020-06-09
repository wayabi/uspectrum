#ifndef __U_ARG_PARSE__
#define __U_ARG_PARSE__

#include <string>
#include <ostream>

#include <boost/program_options.hpp>

class arg_parse {

public:
	std::string log_level;
	int log_file;
	std::string wav;
	int size_buf_fft;
	int offset_fft;
	std::string speaker;
	std::string path_out_spectrum;
public:
	std::string parse(int argc, char** argv)
	{
		using namespace boost::program_options;
		options_description opt("opt");
		opt.add_options()
			("log_level", value<std::string>()->default_value("trace"), "log_level")
			("log_file", value<int>()->default_value(0), "log_file")
			("wav", value<std::string>()->default_value(""), "wav")
			("size_buf_fft", value<int>()->required(), "size_buf_fft")
			("offset_fft", value<int>()->default_value(0), "offset_fft")
			("speaker", value<std::string>()->default_value(""), "speaker")
			("path_out_spectrum", value<std::string>()->required(), "path_out_spectrum")
		;
		variables_map vm;
		try{
			store(parse_command_line(argc, argv, opt), vm);
			notify(vm);
			log_level = vm["log_level"].as<std::string>();
			log_file = vm["log_file"].as<int>();
			wav = vm["wav"].as<std::string>();
			size_buf_fft = vm["size_buf_fft"].as<int>();
			offset_fft = vm["offset_fft"].as<int>();
			speaker = vm["speaker"].as<std::string>();
			path_out_spectrum = vm["path_out_spectrum"].as<std::string>();
		}catch(std::exception& e){
			return std::string(e.what());
		}
		return std::string();
	}

	void print(std::ostream& o)
	{
		o << "log_level[std::string]: " << log_level << std::endl;
		o << "log_file[int]: " << log_file << std::endl;
		o << "wav[std::string]: " << wav << std::endl;
		o << "size_buf_fft[int]: " << size_buf_fft << std::endl;
		o << "offset_fft[int]: " << offset_fft << std::endl;
		o << "speaker[std::string]: " << speaker << std::endl;
		o << "path_out_spectrum[std::string]: " << path_out_spectrum << std::endl;
	}
};
#endif
