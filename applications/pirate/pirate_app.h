#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <generic_scene.hpp>
#include <scene_controller.hpp>
#include <view_controller.hpp>
#include <record_controller.hpp>
#include <text_store.h>

#include <view_modules/submenu_vm.h>
#include "pirate_input_vm.h"

#include <notification/notification_messages.h>

class PirateApp {
public:
    enum class EventType : uint8_t {
        GENERIC_EVENT_ENUM_VALUES,
        MenuSelected,
        ByteEditResult,
    };

    enum class SceneType : uint8_t {
        GENERIC_SCENE_ENUM_VALUES,
        CommandInputScene,
    };

    class Event {
    public:
        union {
            int32_t menu_index;
        } payload;

        EventType type;
    };

    SceneController<GenericScene<PirateApp>, PirateApp> scene_controller;
    TextStore text_store;
    ViewController<PirateApp, SubmenuVM, PirateInputVM> view_controller;
    RecordController<NotificationApp> notification;

    ~PirateApp();
    PirateApp();

    void run();
};
