#ifndef __U_AUDIO_OUTPUT__
#define __U_AUDIO_OUTPUT__

#include <vector>
#include <memory>
#include <utility>

class audio_output {
public:
	//pair.scecond->size() is not data size valid. it's just allocated size.
	//pair.first is valid data size.
	virtual std::pair<int, std::shared_ptr<std::vector<short>>> get_data()
	{
		return std::make_pair<int, std::shared_ptr<std::vector<short>>>(0, nullptr);
	}
};

#endif
