#include "gridWorld.h"
#include "tensorOp.h"
#include "transform.h"
#include "rlMathUtils.h"

using namespace vax::rl::gw::env;
using namespace vax;

void GridWorld::load() {
    _grid = rl::math::Tensor::createZeros({5, 5});
}

rl::gw::env::GridWorldDrawableDescriptor GridWorld::getDrawableDescriptor() const {
    rl::math::TensorOp::print(_grid);
    rl::gw::env::GridWorldDrawableDescriptor descriptor;
    descriptor.drawableDescriptors.reserve(_grid.totalSize());
    int flatIndex = 0;
    for (const auto& block : _grid) {
        BlockType blockType = static_cast<BlockType>(block);
        vax::math::Transform transform = vax::math::Transform();
        std::vector<int> indices = _grid.indices(flatIndex);

        if (vax::rl::math::utils::isBorderIndex(indices, _grid.shape())) {
            blockType = BlockType::WALL;
        }
        auto mul = 1.0f;
        auto offset = 5.0f / 2.0f - 0.5f;
        transform.position = glm::vec3(indices[0] * mul - offset * mul, 0.0f, indices[1] * mul - offset * mul);

        if (blockType == BlockType::WALL) {
            transform.position.y = 0.5f;
        }

        // descriptor.drawableDescriptors.push_back(
        //     objects::DrawableModel::LoadDescriptor{
        //         std::string(blockTypeToPath(blockType)),
        //         transform,
        //     }
        // );
        ++flatIndex;
    }
    descriptor.agentDrawableDescriptor = _agent.getDrawableDescriptor();
    return descriptor;
}

std::string GridWorld::blockTypeToPath(BlockType blockType) const {
    switch (blockType) {
    case BlockType::FLOOR:
        return RES_PATH("assets/models/floor4.glb");
    case BlockType::WALL:
        return RES_PATH("assets/models/wall.glb");
    default:
        return RES_PATH("assets/models/floor4.glb");
    }
}