#include "ModulePacman.h"

#include "sprite_collection_misc.h"

namespace module_pacman {

#define FRAME_DELAY 20

void ModulePacman::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);

  pacman = {new cAnimation((char*)&sprite_misc_pacman8),0,0,true, 99, false, ROT_NONE};
  ghosts[0] = {new cAnimation((char*)&sprite_misc_ghost_red8),0,0,false, 99, false, ROT_NONE};
  ghosts[1] = {new cAnimation((char*)&sprite_misc_ghost_green8),0,0,false, 99, false, ROT_NONE};
  ghosts[2] = {new cAnimation((char*)&sprite_misc_ghost_blue8),0,0,false, 99, false, ROT_NONE};

  all_items[0] = &pacman;
  for (int i=0; i<num_ghosts; i++) {
	  all_items[i+1] = &ghosts[i];
  }

  for (int i=0; i<waypoint_count; i++) {
	  waypoints[i] = {0,0};
  }

  next_move_counter = 0;
}

void ModulePacman::unload() {
	//free all memory!!!
	free(pacman.animation);
	for (int i=0; i<num_ghosts; i++) {
		free(ghosts[i].animation);
	}
}

void ModulePacman::test() {
}

void ModulePacman::process_input_event(EncoderButtonEvent evt) {
	//no user input necessary for this module
}

void ModulePacman::initialize_items() {
	mix_ghosts();
	curr_ghost_count = random(num_ghosts) + 1;
	//define first waypoint (position of direction change on screen):
	waypoints[0].x = random(waypoint_margin,
			display->Width() - pacman.animation->Width() - waypoint_margin);
	waypoints[0].y = random(waypoint_margin,
			display->Height() - pacman.animation->Height() - waypoint_margin);
	//define from which border to approach the first waypoint:
	int origin = random(4);
	//initialize item positions and target waypoint:
	for (int i = 0; i <= curr_ghost_count; i++) {
		all_items[i]->waypoint_index = 0;
		switch (origin) {
		case BORDER_LEFT:
			all_items[i]->x = -all_items[i]->animation->Width();
			for (int j = i + 1; j <= num_ghosts; j++) {
				all_items[i]->x -= (all_items[j]->animation->Width()
						+ item_distance);
			}
			all_items[i]->y = waypoints[0].y;
			break;
		case BORDER_TOP:
			all_items[i]->x = waypoints[0].x;
			all_items[i]->y = -all_items[i]->animation->Height();
			for (int j = i + 1; j <= num_ghosts; j++) {
				all_items[i]->y -= (all_items[j]->animation->Height()
						+ item_distance);
			}
			break;
		case BORDER_RIGHT:
			all_items[i]->x = display->Width();
			for (int j = i + 1; j <= num_ghosts; j++) {
				all_items[i]->x += (all_items[j]->animation->Width()
						+ item_distance);
			}
			all_items[i]->y = waypoints[0].y;
			break;
		case BORDER_BOTTOM:
			all_items[i]->x = waypoints[0].x;
			all_items[i]->y = display->Height();
			for (int j = i + 1; j <= num_ghosts; j++) {
				all_items[i]->y += (all_items[j]->animation->Height()
						+ item_distance);
			}
			break;
		}
	}
	//determine to which direction to leave the screen (must be != origin):
	int destination = random(3);
	if (destination == origin) {
		destination++;
	}
	//define waypoint for this destination
	switch (destination) {
	case BORDER_LEFT:
		waypoints[1].x = -pacman.animation->Width();
		waypoints[1].y = waypoints[0].y;
		break;
	case BORDER_TOP:
		waypoints[1].x = waypoints[0].x;
		waypoints[1].y = -pacman.animation->Height();
		break;
	case BORDER_RIGHT:
		waypoints[1].x = display->Width();
		waypoints[1].y = waypoints[0].y;
		break;
	case BORDER_BOTTOM:
		waypoints[1].x = waypoints[0].x;
		waypoints[1].y = display->Height();
		break;
	}
}

boolean ModulePacman::render_frame(const unsigned long dt) {
	FastLED.clear();

	//do we need to start a new animation?
	if (pacman.waypoint_index >= waypoint_count) {
		initialize_items();
		next_move_counter = move_delay;
	}

	//move items if necessary:
	if (dt >= next_move_counter) {
		if ((move_delay + next_move_counter) >= dt) {
		next_move_counter = (move_delay + next_move_counter) - dt;
		}
		else {
			next_move_counter = 0;
		}

		for (int i=0; i<=curr_ghost_count; i++) {
			if (all_items[i]->waypoint_index >= waypoint_count) {
				continue;
			}
			Waypoint wp = waypoints[all_items[i]->waypoint_index];
			if (all_items[i]->x < wp.x) {
				all_items[i]->x++;
				all_items[i]->flip = false;
				all_items[i]->rotation = ROT_NONE;
			} else if (all_items[i]->x > wp.x) {
				all_items[i]->x--;
				all_items[i]->flip = true;
				all_items[i]->rotation = ROT_NONE;
			}
			if (all_items[i]->y < wp.y) {
				all_items[i]->y++;
				all_items[i]->rotation = ROT_90_RIGHT;
			} else if (all_items[i]->y > wp.y) {
				all_items[i]->y--;
				all_items[i]->rotation = ROT_90_LEFT;
			}
			//have we reached the waypoint?
			if (all_items[i]->x == wp.x && all_items[i]->y == wp.y) {
				all_items[i]->waypoint_index++;
			}
		}
	} else {
		next_move_counter -= dt;
	}

	//draw scene:
	display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::Black);
	for (int i=0; i<=curr_ghost_count; i++) {
		if (all_items[i]->waypoint_index >= waypoint_count) {
			continue;
		}
		all_items[i]->animation->render(dt);
		RotationType_t r = all_items[i]->rotate ? all_items[i]->rotation : ROT_NONE;
		display->DrawPattern(all_items[i]->animation,
				all_items[i]->x,
				all_items[i]->y,
				all_items[i]->animation->Transparency(),
				all_items[i]->animation->TransparentColor(),
				r,
				r == ROT_NONE and all_items[i]->flip, //flip and rotate will look weird on pacman and is not applicable on ghosts
				false);
	}

	delay(FRAME_DELAY);
	return true;
}

void ModulePacman::mix_ghosts() {
	for (int i=0; i<3; i++) {
		int a = random(num_ghosts);
		int b = (a + random(num_ghosts-1) + 1) % num_ghosts;
		cAnimation *tmp = ghosts[a].animation;
		ghosts[a].animation = ghosts[b].animation;
		ghosts[b].animation = tmp;
	}
}

module_icon_t* ModulePacman::getMenuIcon()
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
