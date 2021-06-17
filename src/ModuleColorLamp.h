#ifndef ModuleColorLamp_h
#define ModuleColorLamp_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_color_lamp {

static const unsigned long move_delay = 40;

static const uint8_t hue_step_size = 8;
static const uint8_t sat_step_size = 16;

static const unsigned long mode_animation_duration = 600;

enum SettingMode {
	MODE_POSITION=0,
	MODE_WIDTH=1,
	MODE_COLOR=2,
	MODE_SATURATION=3,
	MODE_COUNT=4
};

struct ColorLampState {
	uint16_t __marker = 0xABAB;
	CHSV color_hsv;
	int16_t center_pos;
	int16_t width;
	SettingMode setting_mode;
};

class ModuleColorLamp: public ModuleTemplate {
private:
	unsigned long next_move_counter = 0;

	ColorLampState state;

	CRGB tmp_color_rgb;

	unsigned long mode_animation_since;
	bool mode_animation_active;

	int16_t calc_animation_position(int16_t curr_value, int16_t max_deviation, int16_t min_value, int16_t max_value);

public:
	void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
	void unload(); //free all memory!!!

	boolean render_frame(const unsigned long dt);
	void process_input_event(EncoderButtonEvent evt);

    int persistence_data_size();
    int get_persistence_data(void* buffer);

	static module_icon_t* getMenuIcon();

	void test();
};

}

#endif
