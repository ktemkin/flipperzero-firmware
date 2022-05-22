#include "pirate_app_scene_command_input.h"

void PirateAppSceneCommandInput::on_enter(PirateApp* app, bool /* need_restore */) {
    PirateInputVM* pirate_input = app->view_controller;
    auto callback = cbc::obtain_connector(this, &PirateAppSceneCommandInput::result_callback);

    pirate_input->set_result_callback(callback, NULL, app, command, sizeof(command));
    app->view_controller.switch_to<PirateInputVM>();
}

bool PirateAppSceneCommandInput::on_event(PirateApp* app, PirateApp::Event* event) {
    bool consumed = false;

    if(event->type == PirateApp::EventType::ByteEditResult) {
        app->scene_controller.switch_to_previous_scene();
        consumed = true;
    }

    return consumed;
}

void PirateAppSceneCommandInput::on_exit(PirateApp* app) {
    app->view_controller.get<PirateInputVM>()->clean();
}

void PirateAppSceneCommandInput::result_callback(void* context) {
    PirateApp* app = static_cast<PirateApp*>(context);
    PirateApp::Event event;

    event.type = PirateApp::EventType::ByteEditResult;

    app->view_controller.send_event(&event);
}
