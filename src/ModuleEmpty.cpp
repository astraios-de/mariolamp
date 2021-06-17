#include "ModuleEmpty.h"

#include "sprite_collection_misc.h"


namespace module_empty {

void ModuleEmpty::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);
}

void ModuleEmpty::unload() {
  //free all memory!!!
}

void ModuleEmpty::test() {
}

void ModuleEmpty::process_input_event(EncoderButtonEvent evt) {
}


boolean ModuleEmpty::render_frame(const unsigned long dt) {
	FastLED.clear();

	if (next_move_counter <= dt) {
		// DO WHAT NEEDS TO BE DONE


		if (dt >= move_delay) {
			next_move_counter = 0;
		} else {
			next_move_counter = (move_delay + next_move_counter) - dt;
		}
	} else {
		next_move_counter -= dt;
	}

	// DRAW FRAME

	return true;
}

module_icon_t* ModuleEmpty::getMenuIcon()
{
  module_icon_t* res = (module_icon_t*)malloc(sizeof(module_icon_t));
  res->width = 3;
  res->height = 3;
  res->data = (uint8_t*)calloc(9,3);
  //create green chess pattern:
  for (uint8_t i=0; i<9; i+=2)
  {
    res->data[3*i+1] = 255;
  }
  return res;
}


}
