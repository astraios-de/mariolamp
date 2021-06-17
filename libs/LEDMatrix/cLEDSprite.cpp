/*
 * cLEDSprite.cpp
 *
 *  Created on: 29.05.2020
 *      Author: Robert
 */

#include <stdint.h>
//#include <pixeltypes.h>

#include "cLEDSprite.h"

cAnimation::cAnimation(const char *data) {
	m_width = ((uint16_t*)data)[0];
	m_height = ((uint16_t*)data)[1];
	m_num_frames = ((uint16_t*)data)[2];
	data += 3*sizeof(uint16_t);

	m_transparency = ((bool*)data)[0];
	data += sizeof(bool);

	m_transparent_color = ((CRGB*)data)[0];
	data += sizeof(CRGB);

	m_preferred_background = ((BackgroundType*)data)[0];
	data += sizeof(BackgroundType);

	m_frame_durations = (unsigned long*)data;
	data += m_num_frames*sizeof(unsigned long);

	m_cycle_type = ((CycleType*)data)[0];
	data += sizeof(CycleType);

	m_pixels = (CRGB*)data;

	if (m_num_frames < 2) {
		m_frame_direction = 0; // indicate that render() does not need to bother about frame switch
	}

	loopCounter = 0;
/*
	Serial.print("m_width=");
	Serial.println(m_width);

	Serial.print("m_height=");
	Serial.println(m_height);

	Serial.print("m_num_frames=");
	Serial.println(m_num_frames);

	Serial.print("m_transparency=");
	Serial.println(m_transparency);

	Serial.print("m_transparentColor=");
	Serial.println(m_transparent_color[0], HEX);
	Serial.println(m_transparent_color[1], HEX);
	Serial.println(m_transparent_color[2], HEX);

	Serial.print("m_preferred_background=");
	Serial.println(m_preferred_background);

	Serial.print("m_frame_durations=");
	for (int i=0; i<m_num_frames; i++) {
		Serial.println(m_frame_durations[i]);
	}

	Serial.print("m_cycle_type=");
	Serial.println(m_cycle_type);

	Serial.println(sizeof(CRGB));//*/

}

CRGB cAnimation::Pixel(int16_t x, int16_t y) {
	return m_pixels[m_frame_index*m_width*m_height + y*m_width + x];
}

boolean cAnimation::render(const unsigned long dt) {
	int old_frame_index = m_frame_index;
	if (m_frame_direction != 0 && m_frame_since + dt > m_frame_durations[m_frame_index]) {
		// switch to next frame
		m_frame_since = m_frame_since + dt - m_frame_durations[m_frame_index]; // correct quantisation error in timing

		m_frame_index += m_frame_direction;
		if (m_frame_index >= m_num_frames) {
			// exceeded while going forward
			switch (m_cycle_type) {
				case ct_none:
					//stay in last frame forever
					m_frame_index = m_num_frames-1;
					m_frame_direction = 0;
					break;
				case ct_cycle:
					//start over from first frame
					m_frame_index = 0;
					loopCounter++;
					break;
				case ct_alternate:
					// change direction
					m_frame_index = m_num_frames - 2; // check that we have at least 2 frames is done in constructor
					m_frame_direction = -1;
					break;
				default:
					//should not be possible
					break;
			}
		} else if (m_frame_index < 0) {
			// exceeded while going backward
			// --> cycle type can only be ct_alternate
			// --> change direction
			m_frame_index = 1; // check that we have at least 2 frames is done in constructor
			m_frame_direction = 1;
			loopCounter++;
		}
	} else {
		m_frame_since += dt;
	}
	return m_frame_index != old_frame_index;
}
