#include "primitivesBuilder.h"
#include "commandManager.h"
#include "queueManager.h"
#include "shaderSharedUtils.h"

using namespace vax::objects;
using namespace vax;

std::optional<DrawableModel> PrimitivesBuilder::createCube(float size, vax::Color color) {
    auto mesh = _meshManager.get().createEmptyMesh();
    if (!mesh)
        return std::nullopt;
    float s = size / 2.0f;
    (*mesh).second->setVertices(
        {// Front face (Z+)
         {{-s, -s, s}, 0, {0, 0, 1, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         {{s, -s, s}, 0, {0, 0, 1, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{s, s, s}, 0, {0, 0, 1, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         {{-s, s, s}, 0, {0, 0, 1, 0}, {0, 0, 0}, {0, 1}, {0, 0}},
         // Back face (Z-)
         {{-s, -s, -s}, 0, {0, 0, -1, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{-s, s, -s}, 0, {0, 0, -1, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         {{s, s, -s}, 0, {0, 0, -1, 0}, {0, 0, 0}, {0, 1}, {0, 0}},
         {{s, -s, -s}, 0, {0, 0, -1, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         // Top face (Y+)
         {{-s, s, -s}, 0, {0, 1, 0, 0}, {0, 0, 0}, {0, 1}, {0, 0}},
         {{-s, s, s}, 0, {0, 1, 0, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         {{s, s, s}, 0, {0, 1, 0, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{s, s, -s}, 0, {0, 1, 0, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         // Bottom face (Y-)
         {{-s, -s, -s}, 0, {0, -1, 0, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         {{s, -s, -s}, 0, {0, -1, 0, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{s, -s, s}, 0, {0, -1, 0, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         {{-s, -s, s}, 0, {0, -1, 0, 0}, {0, 0, 0}, {0, 1}, {0, 0}},
         // Right face (X+)
         {{s, -s, -s}, 0, {1, 0, 0, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{s, s, -s}, 0, {1, 0, 0, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         {{s, s, s}, 0, {1, 0, 0, 0}, {0, 0, 0}, {0, 1}, {0, 0}},
         {{s, -s, s}, 0, {1, 0, 0, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         // Left face (X-)
         {{-s, -s, -s}, 0, {-1, 0, 0, 0}, {0, 0, 0}, {0, 0}, {0, 0}},
         {{-s, -s, s}, 0, {-1, 0, 0, 0}, {0, 0, 0}, {1, 0}, {0, 0}},
         {{-s, s, s}, 0, {-1, 0, 0, 0}, {0, 0, 0}, {1, 1}, {0, 0}},
         {{-s, s, -s}, 0, {-1, 0, 0, 0}, {0, 0, 0}, {0, 1}, {0, 0}}
        }
    );
    for (int i = 0; i < 6; ++i) {
        unsigned int offset = i * 4;
        mesh.value().second->addIndex(offset + 0);
        mesh.value().second->addIndex(offset + 1);
        mesh.value().second->addIndex(offset + 2);

        mesh.value().second->addIndex(offset + 0);
        mesh.value().second->addIndex(offset + 2);
        mesh.value().second->addIndex(offset + 3);
    }

    PBRMaterial material{
        .baseColor = color,
    };
    material.baseColorTextureIndex = NO_TEXTURE_FLAG;
    auto materialIndex = _materialManager.get().insert(material);
    Submesh submesh{
        .indexCount = static_cast<uint32_t>(mesh->second->indices().size()),
        .materialIndex = materialIndex,
    };
    auto drawableModel = vax::objects::DrawableModel(_meshManager.get(), mesh.value().first);
    drawableModel._mesh = mesh.value().second;
    drawableModel._submeshes.push_back(submesh);
    return std::make_optional(drawableModel);
}

std::optional<DrawableModel> PrimitivesBuilder::createBackground() {
    auto mesh = _meshManager.get().createEmptyMesh();
    if (!mesh)
        return std::nullopt;
    mesh.value().second->setVertices({
        {{-1.0f, -1.0f, 1.0f}, 0, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-1.0f, 1.0f, 1.0f}, 0, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, 0, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, 0, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}},
    });

    mesh.value().second->addIndex(0);
    mesh.value().second->addIndex(2);
    mesh.value().second->addIndex(1);
    mesh.value().second->addIndex(1);
    mesh.value().second->addIndex(2);
    mesh.value().second->addIndex(3);

    Submesh submesh{
        .indexCount = static_cast<uint32_t>(mesh->second->indices().size()),
        .materialIndex = NO_MATERIAL_INDEX,
    };
    auto drawableModel = vax::objects::DrawableModel(_meshManager.get(), mesh.value().first);
    drawableModel._mesh = mesh.value().second;
    drawableModel._submeshes.push_back(submesh);
    drawableModel.setSettings(
        DrawableModel::Settings{
            .skipPushConstants = true,
        }
    );
    return std::make_optional(drawableModel);
}