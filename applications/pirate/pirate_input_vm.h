#pragma once
#include "lib/app-scened-template/view_modules/generic_view_module.h"
#include "pirate_input.h"

class PirateInputVM : public GenericViewModule {
public:
    PirateInputVM();
    ~PirateInputVM() final;
    View* get_view() final;
    void clean() final;

    /** 
     * @brief Set byte input result callback
     * 
     * @param input_callback input callback fn
     * @param changed_callback changed callback fn
     * @param callback_context callback context
     * @param bytes buffer to use
     * @param bytes_count buffer length
     */
    void set_result_callback(
        PirateInputCallback input_callback,
        CharChangedCallback changed_callback,
        void* callback_context,
        char* chars,
        uint8_t char_count);

private:
    PirateInput* pirate_input;
};
