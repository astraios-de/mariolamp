#ifndef ModuleWater_h
#define ModuleWater_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_water {

static const unsigned long move_delay = 100;
static const int num_bubbles = 10;
static const unsigned long bubble_min_delay_ms = 0;
static const unsigned long bubble_max_delay_ms = 1000;
static const int16_t bubble_min_speed = 5;
static const int16_t bubble_max_speed = 20;

static const unsigned long fish_min_delay_ms = 500;
static const unsigned long fish_max_delay_ms = 10000;
static const int16_t fish_min_speed = 5;
static const int16_t fish_max_speed = 25;



struct Bubble {
	bool foreground;
	int16_t x;
	int16_t speed; // pixels per second
	unsigned long timer;
	bool active;
};

class ModuleWater: public ModuleTemplate {
private:
	unsigned long next_move_counter = 0;
	Bubble bubbles[num_bubbles];
	void init_bubble(Bubble &bubble);
	void animate_bubble(Bubble &bubble);

	void init_fish();

	cAnimation *fish_animation;
	bool fish_spawned;
	unsigned long fish_spawn_time;
	int16_t fish_speed;
	int16_t fish_y;
	bool fish_direction;
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
