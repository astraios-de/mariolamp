#include "ModuleMarioWalk.h"

#include "sprite_collection_misc.h"


namespace module_mario_walk {

#define FRAME_DELAY 20
#define SPRITE_MOVE_DELAY 500


void ModuleMarioWalk::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);

  animation_walk = new cAnimation((char*)&sprite_misc_mario_walk16);
  animation_stand = new cAnimation((char*)&sprite_misc_mario_stationary16);

  left = false;

  x_pos = -animation_walk->Width();
  x_target = (display->Width()-animation_walk->Width())/2;
  next_move_counter = 0;
  no_input_counter = no_input_auto_move_delay_max;

  input = false;
}

void ModuleMarioWalk::unload() {
  //free all memory!!!
	free(animation_walk);
	free(animation_stand);
}

void ModuleMarioWalk::test() {
}

void ModuleMarioWalk::process_input_event(EncoderButtonEvent evt) {
	if (evt.evtButtonLeft) {
		x_target++;
	} else if (evt.evtButtonRight) {
		x_target--;
	}
	input = true;
	Serial.println(x_target);
}


boolean ModuleMarioWalk::render_frame(const unsigned long dt) {
	FastLED.clear();

	if (x_pos != x_target) {
		left = (x_pos > x_target);
		if (dt >= next_move_counter) {
			if (left) {
				x_pos--;
			} else {
				x_pos++;
			}
			if ((walk_delay + next_move_counter) >= dt) {
			next_move_counter = (walk_delay + next_move_counter) - dt;
			}
			else {
				next_move_counter = 0;
			}

			//did we leave the display?
			if (x_pos <= -animation_walk->Width()) {
				x_target += display->Width()-1-x_pos;
				x_pos = display->Width()-1;
			} else if (x_pos >= display->Width()) {
				x_target -= (display->Width() + animation_walk->Width()-1);
				x_pos = -animation_walk->Width()+1;
			}

		} else {
			next_move_counter -= dt;
		}
		curr_loop_counter = animation_walk->loopCounter; // let this loop finish before stopping the animation
	} else {
		next_move_counter = 0;

		if (input) {
			no_input_counter = random(no_input_auto_move_delay_min, no_input_auto_move_delay_max);
			input = false;
		} else {
			if (no_input_counter < dt) {
				no_input_counter = 0;
			} else {
				no_input_counter -= dt;
			}
		}
		if (no_input_counter == 0) {
			int16_t new_x = x_target;
			while (abs(new_x - x_target) < no_input_auto_move_min_distance) {
				new_x = random16(display->Width()-animation_walk->Width());
			}
			x_target = new_x;
			no_input_counter = random(no_input_auto_move_delay_min, no_input_auto_move_delay_max);
		}
	}

	if (animation_walk->preferred_background() == bt_bright) {
		display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::White);
	}

	if (x_pos != x_target || animation_walk->loopCounter == curr_loop_counter) {
		animation_walk->render(dt);
		display->DrawPattern(animation_walk, x_pos, y_pos, animation_walk->Transparency(), animation_walk->TransparentColor(), left, false);
	} else {
		animation_stand->render(dt);
		display->DrawPattern(animation_stand, x_pos, y_pos, animation_stand->Transparency(), animation_stand->TransparentColor(), left, false);
	}

/*
	if (x_pos != x_pos_prev || x_target != x_target_prev || next_move_counter != next_move_counter_prev) {
		Serial.print(x_pos);
		Serial.print(", ");
		Serial.print(x_target);
		Serial.print(", ");
		Serial.println(next_move_counter);
		x_pos_prev = x_pos;
		x_target_prev = x_target;
		next_move_counter_prev = next_move_counter;
	}*/

  delay(FRAME_DELAY);
  return true;
}

module_icon_t* ModuleMarioWalk::getMenuIcon()
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
