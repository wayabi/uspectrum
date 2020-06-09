#ifndef __U_AUDIO_UTIL__
#define __U_AUDIO_UTIL__

#include <string>
#include <memory>
#include <vector>
#include <utility>

class audio_util {
public:
	static bool write_mp3_short(const short* data, int size, int sampling_rate, const std::string& path);
	static std::shared_ptr<std::vector<short>> double2short(std::shared_ptr<std::vector<double>> d);
	static std::shared_ptr<std::vector<float>> double2float(std::shared_ptr<std::vector<double>> d);
	static std::shared_ptr<std::vector<short>> double2short(std::shared_ptr<std::vector<double>> d, float volume);
	static std::shared_ptr<std::vector<double>> short2double(std::shared_ptr<std::vector<short>> s);
	static void normalize_and_fade(std::shared_ptr<std::vector<double>> dst, std::shared_ptr<std::vector<double>> src, int mergin_fade, double normal_value);
	static std::pair<int, double> get_f0(const std::vector<double>& d);
};

#endif
