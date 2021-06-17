#include "ModuleTemplate.h"

void ModuleTemplate::load(cLEDMatrixBase* display, const void* persistence_data_buffer)
{
  this->display = display;
}

void ModuleTemplate::test() {
  //display->setPixel(27,5,0xFFFFFF);

}

/**
 * How many bytes does this module need to store its persistence data.
 * Returns 0 if no persistence data provided.
 */
int ModuleTemplate::persistence_data_size() {
	return 0;
}

/**
 * Writes module's persistence data to buffer.
 * Memory allocated for buffer must be >= persistence_data_size().
 * Returns amount of bytes written (should be equal to persistence_data_size()).
 */
int ModuleTemplate::get_persistence_data(void* buffer) {
	return 0;
}
