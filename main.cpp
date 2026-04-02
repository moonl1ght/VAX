#include <luna.h>

#include "src/app/App.hpp"

int main() {
    printf("SDL version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    // SDL_SetMainReady(); // Required before SDL_Init when skipping SDL's main
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    std::cout << "Hello, World!" << std::endl;
    // return 0;
    std::srand(std::time(0));
    // Logger::getInstance().log("Run application");
    static App app = App();
    try {
        app.run();
    }
    catch (const std::exception& e) {
        Logger::getInstance().error(e.what());
        return EXIT_FAILURE;
    }
    // SDL_SetMainReady(); // Required before SDL_Init when skipping SDL's main
    // if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    //     SDL_Log("SDL_Init failed: %s", SDL_GetError());
    //     return 1;
    // }

    // SDL_Window* window = SDL_CreateWindow(
    //     "Hello, SDL2 on macOS 🍎",
    //     SDL_WINDOWPOS_CENTERED,
    //     SDL_WINDOWPOS_CENTERED,
    //     640,
    //     480,
    //     0
    // );

    // if (!window)
    // {
    //     printf("error creating window: %s\n", SDL_GetError());
    //     SDL_Quit();
    //     return 1;
    // }

    // // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // // Keep the window open, in this case SDL_Delay(5000); statement won't work.
    // static bool running = true;
    // SDL_Event e;
    // while (running)
    // {
    //     // std::cout << "SDL_PollEvent" << SDL_GetError() << std::endl;
    //     // SDL_Event e;
    //     while (SDL_PollEvent(&e) != 0)
    //     {
    //         std::cout << "SDL_PollEvent " << e.type << " " << (running ? "true" : "false") << std::endl;
    //         if (e.type == SDL_QUIT)
    //         {
    //             std::cout << "SDL_QUIT" << std::endl;
    //             running = false;
    //             break;
    //         } else {
    //             // std::cout << "SDL_PollEvent" << e.type << std::endl;
    //         }
    //     }
    //     // std::cout << "SDL_PollEvent" << SDL_GetError() << std::endl;
    //     // SDL_SetRenderDrawColor(renderer, 255, 255, 255 , 255);
    //     // SDL_RenderClear(renderer);
    //     // SDL_RenderPresent(renderer);

    // // SDL_Delay(16);
    // }

    // // clean up resources before exiting.
    // SDL_DestroyWindow(window);
    // SDL_Quit();

    // Game game = Game();
    // game.run();

    return EXIT_SUCCESS;
}