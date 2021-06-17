#ifndef ModuleMarioWalk_h
#define ModuleMarioWalk_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

namespace module_mario_walk {

static const unsigned long walk_delay = 60;
static const unsigned long no_input_auto_move_delay_min = 5000;
static const unsigned long no_input_auto_move_delay_max = 15000;
static const int16_t no_input_auto_move_min_distance = 5;

class ModuleMarioWalk : public ModuleTemplate {
    public:
        void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
        void unload(); //free all memory!!!
        
        boolean render_frame(const unsigned long dt);
        void process_input_event(EncoderButtonEvent evt);
        static module_icon_t* getMenuIcon();
        
        void test();
        
    private:
        cAnimation *animation_walk;
        cAnimation *animation_stand;

        bool left;

        int16_t x_pos;
        int16_t x_target;
        unsigned long next_move_counter;
        uint16_t curr_loop_counter;
        int16_t y_pos=0;

        unsigned long no_input_counter;
        bool input;

        int16_t x_pos_prev;
        int16_t x_target_prev;
        unsigned long next_move_counter_prev;

};

}

#endif
