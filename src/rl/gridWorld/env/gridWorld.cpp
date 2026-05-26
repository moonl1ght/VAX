#include "gridWorld.h"
#include "tensorOp.h"
#include "transform.h"

using namespace vax::rl::gw::env;
using namespace vax;

void GridWorld::load() {
    _grid = vax::rl::math::Tensor::createZeros({3, 3});
    _grid.set({0, 0}, 1.0f);
}

vax::rl::gw::env::GridWorldDrawableDescriptor GridWorld::getDrawableDescriptor() const {
    vax::rl::math::TensorOp::print(_grid);
    vax::rl::gw::env::GridWorldDrawableDescriptor descriptor;
    descriptor.drawableDescriptors.reserve(_grid.totalSize());
    int flatIndex = 0;
    for (const auto& block : _grid) {
        vax::math::Transform transform = vax::math::Transform();
        std::vector<int> indices = _grid.indices(flatIndex);
        transform.position = glm::vec3(indices[0] * 3.0f, 0.0f, indices[1] * 3.0f);

        descriptor.drawableDescriptors.push_back(
            vax::objects::DrawableModel::LoadDescriptor{
                std::string(blockTypeToPath(static_cast<BlockType>(block))),
                transform,
            }
        );
        ++flatIndex;
    }
    return descriptor;
}

std::string GridWorld::blockTypeToPath(BlockType blockType) const {
    switch (blockType) {
    case BlockType::FLOOR:
        return RES_PATH("assets/models/wood_floor.glb");
    case BlockType::WALL:
        return RES_PATH("assets/models/wall_cube.glb");
    default:
        return RES_PATH("assets/models/wood_floor.glb");
    }
}