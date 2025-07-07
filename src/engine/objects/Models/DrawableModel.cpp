#include "DrawableModel.hpp"

void DrawableModel::draw(VKStack* vkStack) {
    mesh->draw(vkStack);
}