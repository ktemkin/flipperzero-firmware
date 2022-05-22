#include "pirate_app_scene_start.h"

typedef enum {
    SubmenuCommandInput,
} SubmenuIndex;

void PirateAppSceneStart::on_enter(PirateApp* app, bool need_restore) {
    auto submenu = app->view_controller.get<SubmenuVM>();
    auto callback = cbc::obtain_connector(this, &PirateAppSceneStart::submenu_callback);

    submenu->add_item("I2C Command", SubmenuCommandInput, callback, app);

    if(need_restore) {
        submenu->set_selected_item(submenu_item_selected);
    }
    app->view_controller.switch_to<SubmenuVM>();
}

bool PirateAppSceneStart::on_event(PirateApp* app, PirateApp::Event* event) {
    bool consumed = false;

    if(event->type == PirateApp::EventType::MenuSelected) {
        submenu_item_selected = event->payload.menu_index;
        switch(event->payload.menu_index) {
        case SubmenuCommandInput:
            app->scene_controller.switch_to_next_scene(PirateApp::SceneType::CommandInputScene);
            break;
        }
        consumed = true;
    }

    return consumed;
}

void PirateAppSceneStart::on_exit(PirateApp* app) {
    app->view_controller.get<SubmenuVM>()->clean();
}

void PirateAppSceneStart::submenu_callback(void* context, uint32_t index) {
    PirateApp* app = static_cast<PirateApp*>(context);
    PirateApp::Event event;

    event.type = PirateApp::EventType::MenuSelected;
    event.payload.menu_index = index;

    app->view_controller.send_event(&event);
}
