/*
 ################################################################################
 #                                                                              #
 #    Ambient Lights Controller Firmware                                        #
 #                                                                              #
 #    Copyright 2012 Robert Mai                                                 #
 #                                                                              #
 #    Contact: robert@hsapps.com                                                #
 #    URL:     http://www.hsapps.com                                            #
 #                                                                              #
 #                                                                              #
 #    Ambient Lights Controller Firmware is free software: you can              #
 #    redistribute it and/or modify it under the terms of the GNU General       #
 #    Public License as published by the Free Software Foundation, either       #
 #    version 3 of the License, or (at your option) any later version.          #
 #                                                                              #
 #    Ambient Lights Controller Firmware is distributed in the hope that        #
 #    it will be useful, but WITHOUT ANY WARRANTY; without even the implied     #
 #    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.          #
 #    See the GNU General Public License for more details.                      #
 #                                                                              #
 #    You should have received a copy of the GNU General Public License         #
 #    along with this program.  If not, see <http://www.gnu.org/licenses/>.     #
 #                                                                              #
 ################################################################################
 */

#include "EncoderButton.h"
#include "Arduino.h"

EncoderButton::EncoderButton() {
}

void EncoderButton::setup(int pinA, int pinB, int pinBtn,
		unsigned long long_press_time, bool pullup, void (*ISR_callback)(void)) {

	btn_a.setup(pinA, debounce_time_ab, pullup);
	btn_b.setup(pinB, debounce_time_ab, pullup);
	btn_btn.setup(pinBtn, debounce_time_btn, pullup);

	this->long_press_time = long_press_time;

	val_a = btn_a.read();
	val_b = btn_b.read();
	val_btn = btn_btn.read();
	btn_down_time = millis();
	btn_direction = BUTTON_RIGHT;

	previous_millis_a = 0;
	previous_millis_b = 0;
	previous_millis_btn = 0;

	attachInterrupt(digitalPinToInterrupt(pinA), ISR_callback, CHANGE);
	attachInterrupt(digitalPinToInterrupt(pinB), ISR_callback, CHANGE);
	attachInterrupt(digitalPinToInterrupt(pinBtn), ISR_callback,
			CHANGE);
}

void IRAM_ATTR EncoderButton::read_encoder_ISR() {
	portENTER_CRITICAL_ISR(&(this->mux));
	this->evaluate_pins();

	portEXIT_CRITICAL_ISR(&(this->mux));
}

void EncoderButton::evaluate_pins() {
	EncoderButtonEvent evt;

	new_btn = btn_btn.read();
	new_a = btn_a.read();
	new_b = btn_b.read();

	if (new_btn != val_btn) {
		val_btn = new_btn;

		evt.evtButtonDown = val_btn == BUTTON_DOWN;
		evt.evtButtonUp = val_btn == BUTTON_UP;

		if (val_btn == BUTTON_UP && btn_click_interfered == false) {
			if ((millis() - btn_down_time) > long_press_time) {
				evt.evtButtonLongClick = true;
			} else {
				evt.evtButtonClick = true;
			}
		} else if (val_btn == BUTTON_DOWN) {
			btn_down_time = millis();
			btn_click_interfered = false;
		}
	} else {
		if (val_btn == BUTTON_DOWN && (millis() - btn_down_time) > long_press_time) {
			if (!btn_down_long_reported) {
				evt.evtButtonDownLong = true;
				btn_down_long_reported = true;
			}
		} else {
			btn_down_long_reported = false;
		}
	}

	if (new_a != val_a || new_b != val_b) {
		if (((new_a < val_a) && (new_b == 1)) || ((new_a > val_a) && (new_b == 0))) {
			btn_direction = BUTTON_RIGHT;
		}
		if (((new_b < val_b) && (new_a == 1)) || ((new_b > val_b) && (new_a == 0))) {
			btn_direction = BUTTON_LEFT;
		}
		if ((new_a == new_b) && (1 == new_a)) // only report on button's lock position
				{
			btn_click_interfered = true;
			evt.evtButtonDownRight = (val_btn == BUTTON_DOWN)
					&& (btn_direction == BUTTON_RIGHT);
			evt.evtButtonDownLeft = (val_btn == BUTTON_DOWN)
					&& (btn_direction == BUTTON_LEFT);
			evt.evtButtonRight = (val_btn == BUTTON_UP)
					&& (btn_direction == BUTTON_RIGHT);
			evt.evtButtonLeft = (val_btn == BUTTON_UP)
					&& (btn_direction == BUTTON_LEFT);
		}
		val_a = new_a;
		val_b = new_b;
	}
	bool evtButton = evt.evtButtonDown || evt.evtButtonDownLong || evt.evtButtonUp || evt.evtButtonClick
			|| evt.evtButtonLongClick || evt.evtButtonDownRight
			|| evt.evtButtonDownLeft || evt.evtButtonRight || evt.evtButtonLeft;
	if (evtButton) {
		queue_write(evt);
	}
	//if (evtButton) Serial.println("some event happened");
//  Serial.print("A:");
//  Serial.print(valA);
//  Serial.print(" B:");
//  Serial.println(valB);
}

int EncoderButton::debounce(int newVal, int oldVal, unsigned long currentMillis,
		unsigned long *previousMillis) {
	if ((newVal != oldVal)
			&& (currentMillis - *previousMillis >= debounce_time_btn)) {
		*previousMillis = currentMillis;
		return newVal;
	} else {
		return oldVal;
	}
}

void EncoderButton::queue_write(EncoderButtonEvent evt) {
	event_queue[queue_write_pos] = evt;
	if (queue_read_pos == -1) {
		queue_read_pos = queue_write_pos;
	} else if (queue_read_pos == queue_write_pos) {
		// we're about to overwrite the oldest unread value --> move read pointer to the next, newer one
		queue_read_pos = (queue_read_pos + 1) % ENCODER_BUTTON_QUEUE_SIZE;
	}
	queue_write_pos = (queue_write_pos + 1) % ENCODER_BUTTON_QUEUE_SIZE;
}

bool EncoderButton::has_event() {
	return queue_read_pos >= 0;
}

EncoderButtonEvent EncoderButton::get_next_event() {
	if (queue_read_pos >= 0) {
		int resultPos = queue_read_pos;
		queue_read_pos = (queue_read_pos + 1) % ENCODER_BUTTON_QUEUE_SIZE;
		if (queue_read_pos == queue_write_pos) {
			//we overtook the writer --> invalidate read pointer
			queue_read_pos = -1;
		}
		return event_queue[resultPos];
	} else {
		return {}; // user must check EncoderButton::hasEvent() first!
	}
}

int DebouncedPin::read() {
	new_val = read_raw();

	unsigned long now = millis();

	if (new_val != val) {
		if (new_val == val_pre_debounce) {
			if (now - val_pre_debounce_since >= debounce_time) {
				// stable change
				val = new_val;
			} else {
				// not stable yet! do not propagate new button status
			}
		} else {
			val_pre_debounce = new_val;
			val_pre_debounce_since = now;
			//it's new == unstable --> keep old value
		}
	}
	return val;
}

DebouncedPin::DebouncedPin() {
}

void DebouncedPin::setup(int pin_number, const unsigned long debounce_time,
		bool pullup) {
	this->pin_number = pin_number;
	this->debounce_time = debounce_time;

	pinMode(pin_number, pullup ? INPUT_PULLUP : INPUT);

	val = read_raw();
	val_pre_debounce = val;
	val_pre_debounce_since = 0;
}

int DebouncedPin::read_raw() {
	return digitalRead(pin_number);
}
