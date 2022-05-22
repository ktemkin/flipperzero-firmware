#include "pirate_app.h"
#include "scene/pirate_app_scene_start.h"
#include "scene/pirate_app_scene_command_input.h"

PirateApp::PirateApp()
    : scene_controller{this}
    , text_store{128}
    , notification{"notification"} {
}

PirateApp::~PirateApp() {
}

void PirateApp::run() {
    scene_controller.add_scene(SceneType::Start, new PirateAppSceneStart());
    scene_controller.add_scene(SceneType::CommandInputScene, new PirateAppSceneCommandInput());

    notification_message(notification, &sequence_blink_green_10);
    scene_controller.process(100);
}
