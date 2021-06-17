#ifndef ModuleTester_h
#define ModuleTester_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

class ModuleTester : public ModuleTemplate {
    public:
        void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
        void unload(); //free all memory!!!
        
        boolean render_frame(const unsigned long dt);
        void process_input_event(EncoderButtonEvent evt);
        static module_icon_t* getMenuIcon();
        
        void test();
        
    private:
        int current_animation;
        bool new_animation;
        unsigned long current_animation_duration;
        unsigned long target_animation_duration;
        int16_t sprite_vx;
        int16_t sprite_vy;
        unsigned long next_sprite_move;
        CHSV color;
        cLEDSprite *sprite;
        cAnimation *animation;
};

#endif
