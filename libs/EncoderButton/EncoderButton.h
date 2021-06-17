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

#ifndef EncoderButton_h
#define EncoderButton_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#if !defined(ENCODER_BUTTON_QUEUE_SIZE)
	#define ENCODER_BUTTON_QUEUE_SIZE 5
#endif

#define BUTTON_RIGHT 1
#define BUTTON_LEFT  -1

#define BUTTON_DOWN 0
#define BUTTON_UP 1

static const unsigned long debounce_time_btn = 30;
static const unsigned long debounce_time_ab = 10;

struct EncoderButtonEvent {
    boolean evtButtonDown = false; //button was pushed down
    boolean evtButtonDownLong = false; //button is now pushed down for a longer time than long press duration
    boolean evtButtonUp = false; //button released
    boolean evtButtonClick = false; //button was pressed w/o turning between down and up
    boolean evtButtonLongClick = false; //button was long pressed w/o turning between down and up
    boolean evtButtonDownRight = false; //button was turned right while pushed
    boolean evtButtonDownLeft = false; //button was turned left while pushed
    boolean evtButtonRight = false; //button was turned right w/o being pushed
    boolean evtButtonLeft = false; //button was turned left w/o being pushed
};

class DebouncedPin
{
private:
	int pin_number=0;
	unsigned long debounce_time=0;

    int val=0;
    int new_val=0;
    int val_pre_debounce=0;
    unsigned long val_pre_debounce_since=0;

public:
    DebouncedPin();
    void setup(int pin_number, const unsigned long debounce_time, bool pullup);
	int read(); //returns debounced value
	int read_raw(); //returns value without debouncing
};

class EncoderButton
{
  private:
	DebouncedPin btn_a;
	DebouncedPin btn_b;
	DebouncedPin btn_btn;
    unsigned long long_press_time;
    int val_a;
    int val_b;
    int val_btn;
    int new_a;
    int new_b;
    int new_btn;
    unsigned long previous_millis_a;
    unsigned long previous_millis_b;
    unsigned long previous_millis_btn;
    int btn_direction;
    bool btn_click_interfered;
    unsigned long btn_down_time;
    bool btn_down_long_reported;

    unsigned long last_btn_down_evt = 0;
    unsigned long last_btn_up_evt = 0;

    EncoderButtonEvent event_queue[ENCODER_BUTTON_QUEUE_SIZE];
    int queue_read_pos = -1; // where to read next, -1 = invalid --> queue empty
    int queue_write_pos = 0; //where to write next

    int debounce(int newVal, int oldVal, unsigned long currentMillis, unsigned long *previousMillis);

	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

  public:
    void evaluate_pins();
    EncoderButton();
    void setup(int pinA, int pinB, int pinBtn, unsigned long longClickTime, bool pullup, void (*ISR_callback)(void));
	void IRAM_ATTR read_encoder_ISR();

	void queue_write(EncoderButtonEvent evt);
	bool has_event();
	EncoderButtonEvent get_next_event();
};

#endif

