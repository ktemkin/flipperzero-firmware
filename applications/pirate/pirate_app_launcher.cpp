#include "pirate_app.h"

extern "C" int32_t pirate_app(void* p) {
    UNUSED(p);
    PirateApp* app = new PirateApp();
    app->run();
    delete app;

    return 0;
}
