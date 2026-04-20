#include <luna.h>

#include "app.h"
#include "window.h"

int main() {
    printf("SDL version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    std::srand(std::time(0));
    vax::App app = vax::App();
    app.run();

    return EXIT_SUCCESS;
}