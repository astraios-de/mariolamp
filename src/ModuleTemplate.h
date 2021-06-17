#ifndef ModuleTemplate_h
#define ModuleTemplate_h

#include <FastLED.h>
#include <FastLED_GFX.h>
#include <LEDMatrix.h>
#include <EncoderButton.h>

typedef struct {
  uint8_t width;
  uint8_t height;
  uint8_t* data;
} module_icon_t;

class ModuleTemplate {
    public:
        virtual void load(cLEDMatrixBase* display, const void* persistence_data_buffer); //allocate memory etc.
        virtual void unload(); //free all memory!!!
        
        virtual boolean render_frame(const unsigned long dt); //returns if display content was changed and needs to be refreshed
        virtual void process_input_event(EncoderButtonEvent evt);
        // static module_icon_t* getMenuIcon(); //must be implemented by module class!
        
        virtual int persistence_data_size();
        virtual int get_persistence_data(void* buffer);

        virtual void test();
        
    protected:
        cLEDMatrixBase* display;

        
//    private:
        //private members...
};

#endif
