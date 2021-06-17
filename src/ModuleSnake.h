#ifndef ModuleSnake_h
#define ModuleSnake_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_snake {

static const unsigned long move_delay = 100;
static const unsigned long auto_play_delay = 5000;

static const CRGB color_apple = CRGB::Red;
static const CRGB color_snake_head = CRGB::Orange;
static const CRGB color_snake_body = CRGB::Green;

struct Coordinate {
	int16_t x;
	int16_t y;
};

inline bool operator==(const Coordinate& lhs, const Coordinate& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

enum Direction {
	RIGHT,
	DOWN,
	LEFT,
	UP
};

class ModuleSnake: public ModuleTemplate {
private:
	unsigned long next_move_counter = 0;

	unsigned long no_input_since = 0;

	int16_t max_length;
	Coordinate *snake;
	int head_pos;
	int snake_length;
	Direction direction;

	Coordinate apple;
	bool apple_hit;

	bool autoplay;

	void reset_snake();
	Coordinate snake_tile(int idx);
	bool is_part_of_snake(Coordinate c);
	void spawn_apple();

	void move(bool grow);
	void ai();
	void draw();


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
