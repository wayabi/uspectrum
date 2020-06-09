#ifndef __U_E_MIC__
#define __U_E_MIC__

#include <string>

#include "sm_state.h"

class e_mic_init : public sm_event {
public:
	e_mic_init(const std::string& n){name_mic_device_ = n;}
	std::string name_mic_device_;
};
class e_mic_start_rec : public sm_event {
};
class e_mic_stop_rec : public sm_event {
};
class e_mic_close : public sm_event {
};
#endif
