#ifndef __U_SM_STATE__
#define __U_SM_STATE__

#include "sm.h"

class sm;

class sm_event {
public:
	virtual ~sm_event(){}
};

class sm_state {
public:
	const std::string TAG;
	sm_state(std::shared_ptr<sm> p){sm_ = p;}
	virtual void on_entry(std::shared_ptr<sm_event> e){};
	virtual void on_exit(std::shared_ptr<sm_event> e){};
	virtual void on_update(){};
	virtual ~sm_state(){}
	virtual bool event_acceptable(std::shared_ptr<sm_event> e){return true;}

protected:
	std::shared_ptr<sm> sm_;

};

#endif


