#include "ModuleSnake.h"

#include "sprite_collection_misc.h"


namespace module_snake {

void ModuleSnake::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
	ModuleTemplate::load(display, persistence_data_buffer);

	max_length = display->Width()*display->Height();
	snake = (Coordinate*)calloc(max_length, sizeof(Coordinate));

	reset_snake();

	spawn_apple();
	apple_hit = false;

	autoplay = false;

	next_move_counter = 0;
}

void ModuleSnake::unload() {
  free(snake);
}

void ModuleSnake::test() {
}

void ModuleSnake::process_input_event(EncoderButtonEvent evt) {
	if (evt.evtButtonRight) {
		switch (direction) {
		case RIGHT:
			direction = DOWN;
			break;
		case DOWN:
			direction = LEFT;
			break;
		case LEFT:
			direction = UP;
			break;
		case UP:
			direction = RIGHT;
			break;
		}
		no_input_since = 0;
	} else if (evt.evtButtonLeft) {
		switch (direction) {
		case RIGHT:
			direction = UP;
			break;
		case DOWN:
			direction = RIGHT;
			break;
		case LEFT:
			direction = DOWN;
			break;
		case UP:
			direction = LEFT;
			break;
		}
		no_input_since = 0;
	}
}


boolean ModuleSnake::render_frame(const unsigned long dt) {
	no_input_since += dt;
	if (next_move_counter <= dt) {
		if (autoplay || no_input_since >= auto_play_delay) {
			no_input_since = auto_play_delay; // avoid overflow in case all humanity vanishes from earth, but the game keeps on running
			ai();
		}


		move(apple_hit);
		//did we hit the apple?
		apple_hit = snake_tile(0) == apple;
		if (apple_hit) {
			spawn_apple();
		}

		draw();

		if (dt >= move_delay) {
			next_move_counter = 0;
		} else {
			next_move_counter = (move_delay + next_move_counter) - dt;
		}
	} else {
		next_move_counter -= dt;
	}

	return true;
}

module_icon_t* ModuleSnake::getMenuIcon()
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

Coordinate ModuleSnake::snake_tile(int idx) {
	int buf_idx = (head_pos + idx) % max_length;
	return snake[buf_idx];
}

bool ModuleSnake::is_part_of_snake(Coordinate c) {
	bool result = false;
	for (int i=0; i<snake_length; i++) {
		if (c == snake_tile(i)) {
			result = true;
			break;
		}
	}
	return result;
}

void ModuleSnake::spawn_apple() {
	apple = snake_tile(0); // initialize with snake head to ensure collision on first while evaluation
	while (is_part_of_snake(apple)) {
		apple.x = random(display->Width());
		apple.y = random(display->Height());
	}
}

void ModuleSnake::move(bool grow) {
	Coordinate new_pos = snake_tile(0);
	switch (direction) {
	case RIGHT:
		new_pos.x = (new_pos.x + 1) % display->Width();
		break;
	case LEFT:
		if (new_pos.x == 0) {
			new_pos.x = display->Width() - 1;
		} else {
			new_pos.x--;
		}
		break;
	case DOWN:
		new_pos.y = (new_pos.y + 1) % display->Height();
		break;
	case UP:
		if (new_pos.y == 0) {
			new_pos.y = display->Height() - 1;
		} else {
			new_pos.y--;
		}
		break;
	default:
		break;
	}

	if (is_part_of_snake(new_pos)) {
		//COLLISION
		reset_snake();
		return;
	}

	if (head_pos > 0) {
		head_pos--;
	} else {
		head_pos = max_length - 1;
	}
	snake[head_pos] = new_pos;

	if (grow && (snake_length < max_length)) {
		snake_length++;
	}
}

void ModuleSnake::ai() {
	if (apple.x > snake_tile(0).x && direction != LEFT) {
		direction = RIGHT;
	} else if (apple.x < snake_tile(0).x && direction != RIGHT) {
		direction = LEFT;
	} else if (apple.y > snake_tile(0).y && direction != UP) {
		direction = DOWN;
	} else if (apple.y < snake_tile(0).y && direction != DOWN) {
		direction = UP;
	}
}

void ModuleSnake::draw() {
	FastLED.clear();

	display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::Black);

	display->DrawPixel(apple.x, apple.y, color_apple);
	Coordinate tile;
	for (int i=1; i<snake_length; i++) {
		tile = snake_tile(i);
		display->DrawPixel(tile.x, tile.y, color_snake_body);
	}
	tile = snake_tile(0);
	display->DrawPixel(tile.x, tile.y, color_snake_head);

}

}

void module_snake::ModuleSnake::reset_snake() {
	head_pos = 0;
	snake[0] = {0,static_cast<int16_t>(display->Height()/2)};
	snake_length = 1;
	direction = RIGHT;
}
