#pragma once

namespace vax {
    class DrawableScene {
    public:
        DrawableScene();
        virtual ~DrawableScene();

        virtual void draw() = 0;
    };
}