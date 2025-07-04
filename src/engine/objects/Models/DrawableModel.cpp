#include "DrawableModel.hpp"

void DrawableModel::draw(VKStack* vkStack) {
    _mesh.draw(vkStack);
}