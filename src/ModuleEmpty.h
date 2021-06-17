#ifndef ModuleEmpty_h
#define ModuleEmpty_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_empty {

static const unsigned long move_delay = 100;

class ModuleEmpty: public ModuleTemplate {
private:
	unsigned long next_move_counter = 0;

public:
	void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
	void unload(); //free all memory!!!

	boolean render_frame(const unsigned long dt);
	void process_input_event(EncoderButtonEvent evt);
	static module_icon_t* getMenuIcon();

	void test();
};

}

#endif
