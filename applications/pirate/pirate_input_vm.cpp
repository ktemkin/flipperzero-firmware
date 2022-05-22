#include "pirate_input_vm.h"

PirateInputVM::PirateInputVM() {
    pirate_input = pirate_input_alloc();
}

PirateInputVM::~PirateInputVM() {
    pirate_input_free(pirate_input);
}

View* PirateInputVM::get_view() {
    return pirate_input_get_view(pirate_input);
}

void PirateInputVM::clean() {
    pirate_input_set_result_callback(pirate_input, NULL, NULL, NULL, NULL, 0);
}

void PirateInputVM::set_result_callback(
    PirateInputCallback input_callback,
    CharChangedCallback changed_callback,
    void* callback_context,
    char* chars,
    uint8_t char_count) {
    pirate_input_set_result_callback(
        pirate_input, input_callback, changed_callback, callback_context, chars, char_count);
}
