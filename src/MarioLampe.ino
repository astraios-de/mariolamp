#include <EEPROM.h>

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <LEDMatrix.h>
#include <EncoderButton.h>

#include "ModuleTester.h"
#include "ModuleColorLamp.h"
#include "ModuleMarioWalk.h"
#include "ModulePacman.h"
#include "ModuleSnake.h"
#include "ModuleCandle.h"
#include "ModuleWater.h"

//#include <NESController.h>

#define VOLTAGE 5
#define MAX_MILLIAMPS 3000

#define LED_PIN        13
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define POWER_PIN 12

// declare matrix
#define MATRIX_WIDTH        32 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_HEIGHT       8 // height of each matrix
#define MATRIX_TILE_H       1  // number of matrices arranged horizontally
#define MATRIX_TILE_V       2  // number of matrices arranged vertically
#define MATRIX_TYPE    		VERTICAL_ZIGZAG_MATRIX
#define MATRIX_SIZE        (MATRIX_WIDTH*MATRIX_HEIGHT)

/*
 connecting Rotary encoder
 CLK (A pin) - to any microcontroler intput pin with interrupt -> in this example pin 32
 DT (B pin) - to any microcontroler intput pin with interrupt -> in this example pin 21
 SW (button pin) - to any microcontroler intput pin -> in this example pin 25
 VCC - to microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) or in this example pin 25
 GND - to microcontroler GND
 */
//#define ROTARY_ENCODER_A_PIN 32
//#define ROTARY_ENCODER_B_PIN 21
//#define ROTARY_ENCODER_BUTTON_PIN 25
#define ENC2_A_PIN 19
#define ENC2_B_PIN 23
#define ENC2_BTN_PIN 18

#define ENC1_A_PIN 21
#define ENC1_B_PIN 32
#define ENC1_BTN_PIN 25

const static uint8_t brightness_initial = 11;
const static uint8_t brightness_stepsize = 5;
const static uint8_t brightness_min = 1;
const static uint8_t brightness_max = 255;

static const int module_count = 7;

static const unsigned long persistence_write_check_interval = 1000;
static const unsigned long persistence_write_min_age = 5000; // only after this time without changes the data will be stored.

// create our matrix based on matrix definition
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE, MATRIX_TILE_H,
MATRIX_TILE_V> display;

ModuleTemplate *module = NULL;

unsigned long t_prev = 0;
unsigned long t0 = 0;

bool down_rotated = false; // true when module was switched by down+left/right - set to false on button release

unsigned long initial_input_ignore_delay = 500;

unsigned long long_press_delay = 500;
EncoderButton encBrightness;
EncoderButton encControl;

//total size: 128bytes
struct GeneralPersistenceData {
	uint16_t marker;
	uint16_t version;
	int module_index;
	uint8_t brightness;
	uint8_t reserved[128 - 9];
};

GeneralPersistenceData general_state;
static const uint16_t persistence_marker = 0xABCD;
static const uint16_t persistence_version = 1;

int module_persistence_data_offsets[module_count];
int max_module_persistence_data_size = 0;
void *module_persistence_data;

void init_persistence_data() {
	//calculate offsets and total size
	int offset = sizeof(GeneralPersistenceData);

	ModuleTemplate *tmp;
	for (int i = 0; i < module_count; i++) {
		module_persistence_data_offsets[i] = offset;
		tmp = get_module_instance(i);
		int size = 0;
		if (tmp != NULL) {
			size = tmp->persistence_data_size();
			free(tmp);
		}
		if (size > max_module_persistence_data_size) {
			max_module_persistence_data_size = size;
		}
		offset += size;
	}

	module_persistence_data = calloc(max_module_persistence_data_size, 1);

	EEPROM.begin(offset); // offset now reflects total size

	EEPROM.readBytes(0, &general_state, sizeof(GeneralPersistenceData));
	bool valid_data = false;
	if (general_state.marker == persistence_marker) {
		Serial.println("Persistence data has valid marker.");
		if (general_state.version == persistence_version) {
			Serial.print("Persistence data version matches: ");
			Serial.println(persistence_version);
			valid_data = true;
		} else {
			Serial.print("Persistence data version does not match. Expected: ");
			Serial.print(persistence_version);
			Serial.print(" Actual: ");
			Serial.println(general_state.version);
		}
	} else {
		Serial.println(
				"Persistence data has no valid marker -> ignoring data.");
	}
	if (!valid_data) {
		Serial.println("Initializing general_state to all zeros.");
		general_state = {persistence_marker, persistence_version}; // re-initialize rest to zero
	}


	if (general_state.brightness != brightness_initial) {
		Serial.println("Overriding stored brightness by initial value due to safety reasons.");
		general_state.brightness = brightness_initial;
		//FIXME: when will we overcome this?
	}

	if (general_state.brightness > brightness_max) {
		Serial.println("Limiting brightness to max value.");
	}
	if (general_state.brightness < brightness_min) {
		Serial.println("Limiting brightness to min value.");
	}

}

/**
 * Checks if data to write is old enough to persist it.
 * If force==true, data will be written anyway.
 */
void write_persistence_data(bool force) {
	static unsigned long last_check = 0;
	static unsigned long general_unchanged_since = 0;
	static unsigned long module_unchanged_since = 0;
	static bool general_stored = false;
	static bool module_stored = false;
	static int prev_module_index = -1;
	static GeneralPersistenceData tmp_general;
	static void* tmp_module_persistence_data;

	unsigned long now = millis();

	if (last_check == 0) {
		Serial.println("First call on write_persistence_data().");
		// first call
		tmp_general = general_state;
		general_unchanged_since = now;
		general_stored = true;

		tmp_module_persistence_data = calloc(max_module_persistence_data_size,
				1);

		if (module != NULL) {
			memcpy(tmp_module_persistence_data, module_persistence_data,
					module->persistence_data_size());
		} else {
			Serial.println(
					"write_persistence_data: module is NULL! Cannot initialize module persistent data history.");
		}
		module_unchanged_since = now;
		module_stored = true;

		last_check = now;
	}

	if (now - last_check > persistence_write_check_interval) {
		last_check = now;

		// check general_state data:
		if (memcmp(&tmp_general, &general_state, sizeof(GeneralPersistenceData))
				!= 0) {
			// this data is different!
			Serial.println("general_state changed.");
			tmp_general = general_state;
			general_unchanged_since = now;
			general_stored = false;
		}
		bool need_commit = false;
		if (!general_stored
				&& now - general_unchanged_since > persistence_write_min_age) {
			Serial.println("Storing general state data.");
			EEPROM.writeBytes(0, &general_state,
					sizeof(GeneralPersistenceData));
			need_commit = true;
			general_stored = true;
		}

		// check module data:
		if (module != NULL && module->persistence_data_size() > 0) {
			module->get_persistence_data(tmp_module_persistence_data);

			if (memcmp(tmp_module_persistence_data, module_persistence_data,
					module->persistence_data_size()) != 0) {
				//data is different!
				Serial.println("module data changed.");
				memcpy(module_persistence_data, tmp_module_persistence_data,
						module->persistence_data_size());
				module_unchanged_since = now;
				module_stored = false;
			}

			if (!module_stored
					&& now - module_unchanged_since
							> persistence_write_min_age) {
				Serial.println("Storing module data.");
				EEPROM.writeBytes(
						module_persistence_data_offsets[general_state.module_index],
						module_persistence_data,
						module->persistence_data_size());
				need_commit = true;
				module_stored = true;
			}
		}

		if (need_commit) {
			EEPROM.commit();
		}
	}

}

void setup() {
	Serial.begin(115200);
	Serial.println("It's a me, MarioLamp!");

	init_persistence_data();

	// initial FastLED by using CRGB led source from our matrix class
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(display[0], display.Size());
	FastLED.setMaxPowerInVoltsAndMilliamps(VOLTAGE, MAX_MILLIAMPS);

	FastLED.setBrightness(brightness_initial);
	FastLED.clear(true);

	load_module(general_state.module_index);

	FastLED.show();

	pinMode(POWER_PIN, OUTPUT); // digitalWrite(POWER_PIN, HIGH); to power off
	digitalWrite(POWER_PIN, LOW);

	encBrightness.setup(ENC1_A_PIN, ENC1_B_PIN, ENC1_BTN_PIN, long_press_delay,
			true, [] {encBrightness.read_encoder_ISR();});
	encControl.setup(ENC2_A_PIN, ENC2_B_PIN, ENC2_BTN_PIN, long_press_delay,
			true, [] {encControl.read_encoder_ISR();});

	t0 = millis();
}

//int module_under_test = 0;
//int counts = 0;

void loop() {
	while (Serial.available() > 0) {
		int b = Serial.read();
		if (b == 114) {
			//"r"
			Serial.println("RESET requested via serial command!");
			ESP.restart();
		}
	}

	unsigned long now = millis();
	unsigned long dt;

	// calculate delta time for module rendering:
	if (t_prev == 0) {
		dt = 0;
	} else {
		dt = now - t_prev;
	}
	t_prev = now;

	// Check if we need to change the brightness:
	encBrightness.evaluate_pins();
	encControl.evaluate_pins();
	if (initial_input_ignore_delay == 0 || millis() - t0 > initial_input_ignore_delay) {
		initial_input_ignore_delay = 0;
		while (encBrightness.has_event()) {
			EncoderButtonEvent evt = encBrightness.get_next_event();
			printEncEvent("brightness", evt);

			if (evt.evtButtonRight) {
				// increase brightness
				if (general_state.brightness
						<= brightness_max - brightness_stepsize) {
					general_state.brightness += brightness_stepsize;
				}
			}
			if (evt.evtButtonLeft) {
				// decrease brightness
				if (general_state.brightness
						>= brightness_min + brightness_stepsize) {
					general_state.brightness -= brightness_stepsize;
				}
			}
		}

		// Check if there was input on the control encoder:
		while (encControl.has_event()) {
			EncoderButtonEvent evt = encControl.get_next_event();
			printEncEvent("control", evt);

			if (evt.evtButtonUp) {
				down_rotated = false;
			}

			if (evt.evtButtonDownRight) {
				next_module();
				down_rotated = true;
			} else if (evt.evtButtonDownLeft) {
				previous_module();
				down_rotated = true;
			} else if (evt.evtButtonDownLong and !down_rotated) {
				// we need to switch to the next display mode:
				next_module();
			} else {
				// pass this event to the current active module:
				if (module != NULL) {
					module->process_input_event(evt);
				}
			}
		}
	}

	if (general_state.brightness != FastLED.getBrightness()) {
		Serial.print("Setting brightness to ");
		Serial.println(general_state.brightness);
	}
	FastLED.setBrightness(general_state.brightness);

	if (module != NULL && module->render_frame(dt)) {
		FastLED.show();
	}

	write_persistence_data(false);

}

ModuleTemplate *get_module_instance(int module_index) {
	ModuleTemplate *result;
	switch (module_index) {
	case 0:
		result = new module_color_lamp::ModuleColorLamp();
		break;
	case 1:
		result = new module_mario_walk::ModuleMarioWalk();
		break;
	case 2:
		result = new module_pacman::ModulePacman();
		break;
	case 3:
		result = new module_snake::ModuleSnake();
		break;
	case 4:
		result = new ModuleTester();
		break;
	case 5:
		result = new ModuleCandle();
		break;
	case 6:
		result = new module_water::ModuleWater();
		break;
	default:
		Serial.print("No class for module id ");
		Serial.println(general_state.module_index);
		result = NULL;
		break;
	}
	return result;
}

void next_module() {
	general_state.module_index = (general_state.module_index + 1)
			% module_count;
	load_module(general_state.module_index);
}

void previous_module() {
	if (general_state.module_index > 0) {
		general_state.module_index--;
	} else {
		general_state.module_index = module_count - 1;
	}
	load_module(general_state.module_index);
}


void load_module(int module_index) {
	Serial.print("Switching to module #");
	Serial.println(module_index);

	if (module != NULL) {
		module->unload();
		free(module);
	}

	module = get_module_instance(module_index);
	if (module != NULL) {
		if (module->persistence_data_size() > 0) {
			EEPROM.readBytes(module_persistence_data_offsets[module_index],
					module_persistence_data, module->persistence_data_size());
			module->load(&display, module_persistence_data);
		} else {
			module->load(&display, NULL);
		}
	}
}

void printEncEvent(const char* caption, EncoderButtonEvent evt) {
	Serial.print("----- #");
	Serial.print(caption);
	Serial.println(" event -----");
	if (evt.evtButtonDown)
		Serial.println("button was pushed down");
	if (evt.evtButtonDownLong)
		Serial.println("button was pushed down for a long time now");
	if (evt.evtButtonUp)
		Serial.println("button released");
	if (evt.evtButtonClick)
		Serial.println("button was pressed w/o turning between down and up");
	if (evt.evtButtonLongClick)
		Serial.println(
				"button was long pressed w/o turning between down and up");
	if (evt.evtButtonDownRight)
		Serial.println("button was turned right while pushed");
	if (evt.evtButtonDownLeft)
		Serial.println("button was turned left while pushed");
	if (evt.evtButtonRight)
		Serial.println("button was turned right w/o being pushed");
	if (evt.evtButtonLeft)
		Serial.println("button was turned left w/o being pushed");
}
