#pragma once
#include "../pirate_app.h"

class PirateAppSceneStart : public GenericScene<PirateApp> {
public:
    void on_enter(PirateApp* app, bool need_restore) final;
    bool on_event(PirateApp* app, PirateApp::Event* event) final;
    void on_exit(PirateApp* app) final;

private:
    void submenu_callback(void* context, uint32_t index);
    uint32_t submenu_item_selected = 0;
};
