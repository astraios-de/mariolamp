#include "ModuleCandle.h"

#include "sprite_collection_misc.h"

#define FRAME_DELAY 20
#define SPRITE_MOVE_DELAY 500

void ModuleCandle::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);

  animation = new cAnimation((char*)&sprite_misc_candle16);

}

void ModuleCandle::unload() {
  //free all memory!!!
	free(animation);
}

void ModuleCandle::test() {
}

void ModuleCandle::process_input_event(EncoderButtonEvent evt) {
}


boolean ModuleCandle::render_frame(const unsigned long dt) {
	FastLED.clear();

	animation->render(dt);
	if (animation->preferred_background() == bt_bright) {
		display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::White);
	}
	int16_t x = (display->Width()-animation->Width())/2;
	int16_t y = (display->Height()-animation->Height())/2;
	display->DrawPattern(animation, x,y, animation->Transparency(), animation->TransparentColor());

  delay(FRAME_DELAY);
  return true;
}

module_icon_t* ModuleCandle::getMenuIcon()
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


