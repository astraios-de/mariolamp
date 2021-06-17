#ifndef ModulePacman_h
#define ModulePacman_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_pacman {

static const int num_ghosts = 3;
static const int num_items = num_ghosts+1;
static const unsigned long move_delay = 60;
static const int waypoint_count = 2;

static const int16_t waypoint_margin = 1; // corner will be this amount of pixels away from border
static const int16_t item_distance = 2; // distance of chasing items

static const int BORDER_TOP=0;
static const int BORDER_RIGHT=1;
static const int BORDER_BOTTOM=2;
static const int BORDER_LEFT=3;

struct PacmanItem {
	cAnimation *animation;
	int16_t x;
	int16_t y;
	bool rotate;
	int waypoint_index; //which waypoint is it currently approaching?
	bool flip;
	RotationType_t rotation;
};

struct Waypoint {
	int16_t x;
	int16_t y;
};


class ModulePacman : public ModuleTemplate {
    private:
        PacmanItem pacman;
        PacmanItem ghosts[num_ghosts];
        PacmanItem* all_items[num_ghosts+1];
        Waypoint waypoints[waypoint_count];

        unsigned long next_move_counter;

        int curr_ghost_count;

        void mix_ghosts();
	void initialize_items();

    public:
        void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
        void unload(); //free all memory!!!
        
        boolean render_frame(const unsigned long dt);
        void process_input_event(EncoderButtonEvent evt);
        static module_icon_t* getMenuIcon();
        void test();
};

}

#endif
