#ifndef _FILE_REQ_DEMO_H_
#define _FILE_REQ_DEMO_H_

#include "woopsi.h"
#include "gadgeteventhandler.h"
#include "textbox.h"

using namespace WoopsiUI;

class FileReqDemo : public Woopsi, public GadgetEventHandler {
public:
	void handleValueChangeEvent(const GadgetEventArgs& e);
	
private:
	TextBox* _textbox;
	
	void startup();
	void shutdown();
};

#endif
