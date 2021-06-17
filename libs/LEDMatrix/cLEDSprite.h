/*
 * cLEDSprite.h
 *
 *  Created on: 29.05.2020
 *      Author: Robert
 */

#ifndef LIBRARIES_LEDMATRIX_CLEDSPRITE_H_
#define LIBRARIES_LEDMATRIX_CLEDSPRITE_H_

#include <LEDMatrix.h>

enum BackgroundType {bt_any, bt_dark, bt_bright};
enum CycleType {ct_none, ct_cycle, ct_alternate};

class cLEDSprite: public cLEDMatrixBase {
private:
	//int16_t m_x, m_y;
public:
	int16_t x;
	int16_t y;
	bool transparency;
	CRGB colTransparent;

	cLEDSprite(int16_t w, int16_t h, int16_t x, int16_t y, bool transparency, CRGB colTransparent) {
		m_Width = w;
		m_Height = h;
		this->x = x;
		this->y = y;
		this->transparency = transparency;
		this->colTransparent = colTransparent;
		m_LED = (CRGB*)calloc(w*h, sizeof(CRGB));
	}
	cLEDSprite(int16_t w, int16_t h) : cLEDSprite(w, h, 0, 0, false, CRGB::Black) {};
	cLEDSprite(int16_t w, int16_t h, int16_t x, int16_t y) : cLEDSprite(w, h, x, y, false, CRGB::Black) {};
	cLEDSprite(int16_t w, int16_t h, CRGB colTransparent) : cLEDSprite(w, h, 0, 0, true, colTransparent) {};
	cLEDSprite(int16_t w, int16_t h, int16_t x, int16_t y, CRGB colTransparent) : cLEDSprite(w, h, x, y, true, colTransparent) {};

	~cLEDSprite() {
		free(m_LED);
	}
	virtual uint32_t mXY(uint16_t x, uint16_t y) {return y * m_Width + x;}

	void drawOn(cLEDMatrixBase *target) {target->DrawPattern(this, x, y, transparency, colTransparent);};
};


class cAnimation : public iDrawable {
private:
	int16_t m_width;
	int16_t m_height;
	int16_t m_num_frames;
	bool m_transparency;
	CRGB m_transparent_color;
	BackgroundType m_preferred_background;
	unsigned long *m_frame_durations;
	CycleType m_cycle_type;
	CRGB *m_pixels;

	int m_frame_index = 0;
	int m_frame_direction = 1;
	unsigned long m_frame_since = 0;
public:
	cAnimation(const char *data);
	int16_t Width() { return m_width; };
	int16_t Height() { return m_height; };
	bool Transparency() { return m_transparency; };
	CRGB TransparentColor() { return m_transparent_color; };
	CRGB Pixel(int16_t x, int16_t y);
	boolean render(const unsigned long dt); //returns true if animation content was changed
	int16_t loopCounter;
	BackgroundType preferred_background() { return m_preferred_background; };
};


#endif /* LIBRARIES_LEDMATRIX_CLEDSPRITE_H_ */
