#ifndef DrawableModel_hpp
#define DrawableModel_hpp

#include "luna.h"
#include "Model.hpp"

class DrawableModel : public Model {
public:
    DrawableModel();
    ~DrawableModel();

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    void draw(VKStack* vkStack);

private:
    Mesh _mesh;
};

#endif