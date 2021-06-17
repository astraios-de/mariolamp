#include "ModuleColorLamp.h"

#include "sprite_collection_misc.h"

namespace module_color_lamp {

void ModuleColorLamp::load(cLEDMatrixBase* display,
		const void* persistence_data_buffer) {
	ModuleTemplate::load(display, persistence_data_buffer);

	state.width = display->Width() / 4;
	state.center_pos = display->Width() / 2;

	state.color_hsv = {0,255,255};
	state.setting_mode = MODE_POSITION;

	if (persistence_data_buffer != NULL) {
		ColorLampState state_tmp = *((ColorLampState*) persistence_data_buffer);
		if (state_tmp.__marker == state.__marker) {
			state = state_tmp;
		}
	}

	mode_animation_active = true;
	mode_animation_since = 0;
}

void ModuleColorLamp::unload() {
	//free all memory!!!
}

int16_t ModuleColorLamp::calc_animation_position(int16_t curr_value,
		int16_t max_deviation, int16_t min_value, int16_t max_value) {
	int16_t result = 0;

	int16_t min_result = std::max(min_value,
			static_cast<int16_t>(curr_value - max_deviation));
	int16_t max_result = std::min(max_value,
			static_cast<int16_t>(curr_value + max_deviation));
	if (max_result - min_result < 2 * max_deviation) {
		if (min_result == min_value) {
			max_result = min_value + 2 * max_deviation;
		} else {
			min_result = max_value - 2 * max_deviation;
		}
	}
	int16_t alter_plus = max_result - curr_value;
	int16_t alter_minus = curr_value - min_result;

	int16_t num_steps = 2 * (max_result - min_result);
	int16_t curr_step = (num_steps * mode_animation_since)
			/ mode_animation_duration;
	if (curr_step <= alter_plus) {
		result = curr_value + curr_step;
	} else if (curr_step <= 2 * alter_plus) {
		result = max_result - (curr_step - alter_plus);
	} else if (curr_step <= 2 * alter_plus + alter_minus) {
		result = curr_value - (curr_step - 2 * alter_plus);
	} else {
		result = min_result + (curr_step - 2 * alter_plus - alter_minus);
	}

	return result;
}

int ModuleColorLamp::persistence_data_size() {
	return sizeof(ColorLampState);
}

int ModuleColorLamp::get_persistence_data(void* buffer) {
	memcpy(buffer, (const void*)&state, persistence_data_size());
	return persistence_data_size();
}

void ModuleColorLamp::test() {
}

void ModuleColorLamp::process_input_event(EncoderButtonEvent evt) {
	if (evt.evtButtonClick) {
		//switch mode
		state.setting_mode = (SettingMode) ((state.setting_mode + 1)
				% MODE_COUNT);
		mode_animation_active = true;
		mode_animation_since = 0;
	}

	switch (state.setting_mode) {
	case MODE_POSITION:
		if (evt.evtButtonRight && state.center_pos >= state.width / 2) {
			state.center_pos--;
		}
		if (evt.evtButtonLeft
				&& state.center_pos < display->Width() - state.width / 2 - 1) {
			state.center_pos++;
		}
		break;
	case MODE_WIDTH:
		if (evt.evtButtonRight && state.width < display->Width()) {
			state.width += 2;
			//move if we exceed boundaries:
			if (state.center_pos - state.width / 2 + 1 < 0) {
				state.center_pos += 1;
			}
			if (state.center_pos + state.width / 2 >= display->Width()) {
				state.center_pos -= 1;
			}
		}
		if (evt.evtButtonLeft && state.width > 2) {
			state.width -= 2;
		}
		break;
	case MODE_COLOR: {
		int hue_tmp = state.color_hsv.h;
		if (evt.evtButtonRight) {
			hue_tmp += hue_step_size;
		}
		if (evt.evtButtonLeft) {
			hue_tmp -= hue_step_size;
		}
		state.color_hsv.h = (hue_tmp + 256) % 256;
	}
		break;
	case MODE_SATURATION: {
		if (evt.evtButtonRight) {
			if (state.color_hsv.s < 255 - sat_step_size) {
				state.color_hsv.s += sat_step_size;
			} else {
				state.color_hsv.s = 255;
			}
		}
		if (evt.evtButtonLeft) {
			if (state.color_hsv.s > sat_step_size) {
				state.color_hsv.s -= sat_step_size;
			} else {
				state.color_hsv.s = 0;
			}
		}
	}
		break;
	default:
		break;
	}
}

boolean ModuleColorLamp::render_frame(const unsigned long dt) {
	FastLED.clear();

	int16_t draw_pos = state.center_pos;
	int16_t draw_width = state.width;
	CHSV draw_hsv = state.color_hsv;

	if (mode_animation_active) {
		mode_animation_since += dt;
		if (mode_animation_since >= mode_animation_duration) {
			mode_animation_active = false;
		} else {
			switch (state.setting_mode) {
			case MODE_WIDTH:
				draw_width = calc_animation_position(state.width, 8, 2,
						display->Width());
				break;
			case MODE_POSITION:
				draw_pos = calc_animation_position(state.center_pos, 8, 0,
						display->Width() - 1);
				break;
			case MODE_COLOR:
				draw_hsv.h += (256 * mode_animation_since)
						/ mode_animation_duration;
				break;
			case MODE_SATURATION:
				draw_hsv.s = calc_animation_position(draw_hsv.s, 128, 0, 255);
				//draw_hsv.s += (256 * mode_animation_since) / mode_animation_duration;
				break;
			default:
				break;
			}
		}
	}

	hsv2rgb_rainbow(draw_hsv, tmp_color_rgb);
	display->DrawFilledRectangle(draw_pos - draw_width / 2 + 1, 0,
			draw_pos + draw_width / 2, display->Height(), tmp_color_rgb);

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

module_icon_t* ModuleColorLamp::getMenuIcon() {
	module_icon_t* res = (module_icon_t*) malloc(sizeof(module_icon_t));
	res->width = 3;
	res->height = 3;
	res->data = (uint8_t*) calloc(9, 3);
	//create green chess pattern:
	for (uint8_t i = 0; i < 9; i += 2) {
		res->data[3 * i + 1] = 255;
	}
	return res;
}

}
