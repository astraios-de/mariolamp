#include "ModuleTester.h"

#include "sprite_collection_misc.h"

#define FRAME_DELAY 20
#define SPRITE_MOVE_DELAY 500

#define ANIMATION_DURATION 5000

void ModuleTester::load(cLEDMatrixBase* display, const void* persistence_data_buffer) {
  ModuleTemplate::load(display, persistence_data_buffer);

  color.h = 0;
  color.s = 255;
  color.v = 255;

  sprite = new cLEDSprite(4, 4, 1, 0, CRGB::Black);


  animation = new cAnimation(sprite_collection_misc_all_sprites[0]);
  current_animation = 0;
  current_animation_duration = 0;
  target_animation_duration = ANIMATION_DURATION;
  new_animation = false;

  sprite->DrawLine(0, 0, 3, 3, CRGB::Red);
  sprite->DrawLine(3, 0, 0, 3, CRGB::Green);

  sprite_vx = 1;
  sprite_vy = 2;
  next_sprite_move = millis() + SPRITE_MOVE_DELAY;

}

void ModuleTester::unload() {
  //free all memory!!!
	free(sprite);
	free(animation);
}

void ModuleTester::test() {
}

void ModuleTester::process_input_event(EncoderButtonEvent evt) {
	if (evt.evtButtonRight) {
		current_animation = (current_animation + 1) % sprite_collection_misc_count;
		new_animation = true;
		target_animation_duration = 0;
	}
	if (evt.evtButtonLeft) {
		if (current_animation == 0) {
			current_animation = sprite_collection_misc_count - 1;
		} else {
			current_animation--;
		}
		new_animation = true;
		target_animation_duration = 0;
	}
	if (evt.evtButtonClick) {
		target_animation_duration = ANIMATION_DURATION;
		current_animation_duration = ANIMATION_DURATION; // force immediate change
	}
}


boolean ModuleTester::render_frame(const unsigned long dt) {
	color.h++;

	//clear old position
	int16_t x1 = (13*millis()/1000)%(display->Width()*2);
	int16_t x2 = (29*millis()/1000)%(display->Width()*2);


	if (x1 >=display->Width()) x1 = display->Width()-1-x1%display->Width();
	if (x2 >=display->Width()) x2 = display->Width()-1-x2%display->Width();

	FastLED.clear();
	//display->DrawLine(x1, 0, x2, display->Height()-1, color);



	if (millis()>next_sprite_move) {
		next_sprite_move = millis() + SPRITE_MOVE_DELAY;
		sprite->x += sprite_vx;
		sprite->y += sprite_vy;

		if ((sprite_vx > 0 && sprite->x >= display->Width()-sprite->Width()) || (sprite_vx < 0 && sprite->x < 0)) {
			sprite_vx *= -1;
		}
		if ((sprite_vy > 0 && sprite->y >= display->Height()-sprite->Height()) || (sprite_vy < 0 && sprite->y < 0)) {
			sprite_vy *= -1;
		}

	}


	current_animation_duration += dt;
	if (new_animation || (target_animation_duration != 0 && current_animation_duration > target_animation_duration)) {
		free(animation);
		if (!new_animation) {
			current_animation = (current_animation + 1) % sprite_collection_misc_count;
		}
		animation = new cAnimation(sprite_collection_misc_all_sprites[current_animation]);
		current_animation_duration = 0;
		new_animation = false;
	}

	animation->render(dt);
	if (animation->preferred_background() == bt_bright) {
		display->DrawFilledRectangle(0, 0, display->Width(), display->Height(), CRGB::White);
	}
	int16_t x = (display->Width()-animation->Width())/2;
	int16_t y = (display->Height()-animation->Height())/2;
	display->DrawPattern(animation, x,y, animation->Transparency(), animation->TransparentColor());

	//sprite->drawOn(display);
	//display->DrawPattern(sprite, 7, 2);

  delay(FRAME_DELAY);
  return true;
}

module_icon_t* ModuleTester::getMenuIcon()
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


