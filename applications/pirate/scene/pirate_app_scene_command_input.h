#pragma once
#include "../pirate_app.h"

class PirateAppSceneCommandInput : public GenericScene<PirateApp> {
public:
    void on_enter(PirateApp* app, bool need_restore) final;
    bool on_event(PirateApp* app, PirateApp::Event* event) final;
    void on_exit(PirateApp* app) final;

private:
    void result_callback(void* context);

    char command[128] = "[0x";
};
