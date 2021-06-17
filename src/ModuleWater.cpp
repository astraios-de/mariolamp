#include "ModuleWater.h"

#include "sprite_collection_misc.h"
#include <Arduino.h>

namespace module_water {

void ModuleWater::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);

  for (Bubble &bubble: bubbles) {
	  init_bubble(bubble);
  }

  fish_animation = NULL;
  init_fish();
  fish_spawn_time = millis(); // first fish instantly
}

void ModuleWater::unload() {
  //free all memory!!!
}

void ModuleWater::test() {
}

void ModuleWater::process_input_event(EncoderButtonEvent evt) {
	if (evt.evtButtonClick && !fish_spawned) {
		fish_spawn_time = millis(); // spawn a fish now!
	}
}

void ModuleWater::init_fish() {
	if (fish_animation != NULL) {
		free(fish_animation);
	}
	int fish_selection = random(5);

	switch (fish_selection) {
	case 0:
		fish_animation = new cAnimation((const char*)&sprite_misc_fish_a11);
		break;
	case 1:
		fish_animation = new cAnimation((const char*)&sprite_misc_fish_b14);
		break;
	case 2:
		fish_animation = new cAnimation((const char*)&sprite_misc_fish_c14);
		break;
	case 3:
		fish_animation = new cAnimation((const char*)&sprite_misc_fish_d14);
		break;
	case 4:
		fish_animation = new cAnimation((const char*)&sprite_misc_fish_e12);
		break;
	default:
		fish_animation = NULL;
	}

	if (fish_animation != NULL) {
		fish_spawned = false;
		fish_direction = random8() & 1;
		fish_speed = random16(fish_min_speed, fish_max_speed);
		fish_spawn_time = millis() + random(fish_min_delay_ms, fish_max_delay_ms);
		fish_y = random16(display->Height()-fish_animation->Height());
	}

}

void ModuleWater::init_bubble(Bubble &bubble) {
	bubble.active = false;
	bubble.foreground = random8() & 1;
	bubble.timer = millis() + random(bubble_min_delay_ms, bubble_max_delay_ms); // time when bubble will be activated
	bubble.speed = random16(bubble_min_speed, bubble_max_speed);
	bubble.x = random16(0, display->Width());
}

void ModuleWater::animate_bubble(Bubble &bubble) {
	// activate bubble if timer is expired
	if (!bubble.active && millis() >= bubble.timer) {
		bubble.active = true;
		bubble.timer = millis(); // start time
	}

	// calc position and draw
	if (bubble.active) {
		unsigned long dt = millis() - bubble.timer;
		uint16_t y  = bubble.speed * dt / 1000;
		if (y < display->Height()) {
			// draw the bubble
			display->DrawPixel(bubble.x, display->Height() - 1 - y, CRGB::White);
		} else {
			// bubble left the screen --> re-initialize
			init_bubble(bubble);
		}
	}
}

boolean ModuleWater::render_frame(const unsigned long dt) {
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
	display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::Blue);

	for (Bubble &bubble: bubbles) {
		if (!bubble.foreground) {
			animate_bubble(bubble);
		}
	}

	if (!fish_spawned) {
		if (millis() >= fish_spawn_time) {
			// spawn the fish!
			fish_spawned = true;
		}
	}
	if (fish_spawned) {
		fish_animation->render(dt);
		// draw the fish!
		unsigned long dt = millis() - fish_spawn_time;
		int16_t x  = fish_speed * dt / 1000;
		if (x < display->Width() + fish_animation->Width()) {
			if (fish_direction) {
				display->DrawPattern(fish_animation, x - fish_animation->Width(), fish_y, fish_animation->Transparency(), fish_animation->TransparentColor());
			} else {
				// flip and from right
				display->DrawPattern(fish_animation, display->Width() - x, fish_y, fish_animation->Transparency(), fish_animation->TransparentColor(), ROT_NONE, true, false);
			}

		} else {
			// fish is finished, get a new one:
			init_fish();
		}
	}

	for (Bubble &bubble: bubbles) {
		if (bubble.foreground) {
			animate_bubble(bubble);
		}
	}

	return true;
}

module_icon_t* ModuleWater::getMenuIcon()
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
