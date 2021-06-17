#ifndef ModuleCandle_h
#define ModuleCandle_h

#include "ModuleTemplate.h"
#include <cLEDSprite.h>

class ModuleCandle : public ModuleTemplate {
    public:
        void load(cLEDMatrixBase* display, const void* persistence_data_buffer);
        void unload(); //free all memory!!!
        
        boolean render_frame(const unsigned long dt);
        void process_input_event(EncoderButtonEvent evt);
        static module_icon_t* getMenuIcon();
        
        void test();
        
    private:
        cAnimation *animation;
};

#endif
