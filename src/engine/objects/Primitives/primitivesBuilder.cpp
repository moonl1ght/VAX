#include "primitivesBuilder.h"
#include "commandManager.h"
#include "queueManager.h"

using namespace vax::objects;
using namespace vax;

std::optional<DrawableModel> PrimitivesBuilder::createCube(float size, vax::Color color) {
    auto mesh = _meshManager.get().createEmptyMesh();
    if (!mesh) return std::nullopt;
    float s = size / 2.0f;
    (*mesh).second->setVertices({
        // Front face (Z+)
        {{-s, -s,  s}, {0, 0, 1}, {0, 0, 0}, {0, 0}}, {{ s, -s,  s}, {0, 0, 1}, {0, 0, 0}, {1, 0}},
        {{ s,  s,  s}, {0, 0, 1}, {0, 0, 0}, {1, 1}}, {{-s,  s,  s}, {0, 0, 1}, {0, 0, 0}, {0, 1}},
        // Back face (Z-)
        {{-s, -s, -s}, {0, 0,-1}, {0, 0, 0}, {1, 0}}, {{-s,  s, -s}, {0, 0,-1}, {0, 0, 0}, {1, 1}},
        {{ s,  s, -s}, {0, 0,-1}, {0, 0, 0}, {0, 1}}, {{ s, -s, -s}, {0, 0,-1}, {0, 0, 0}, {0, 0}},
        // Top face (Y+)
        {{-s,  s, -s}, {0, 1, 0}, {0, 0, 0}, {0, 1}}, {{-s,  s,  s}, {0, 1, 0}, {0, 0, 0}, {0, 0}},
        {{ s,  s,  s}, {0, 1, 0}, {0, 0, 0}, {1, 0}}, {{ s,  s, -s}, {0, 1, 0}, {0, 0, 0}, {1, 1}},
        // Bottom face (Y-)
        {{-s, -s, -s}, {0,-1, 0}, {0, 0, 0}, {0, 0}}, {{ s, -s, -s}, {0,-1, 0}, {0, 0, 0}, {1, 0}},
        {{ s, -s,  s}, {0,-1, 0}, {0, 0, 0}, {1, 1}}, {{-s, -s,  s}, {0,-1, 0}, {0, 0, 0}, {0, 1}},
        // Right face (X+)
        {{ s, -s, -s}, {1, 0, 0}, {0, 0, 0}, {1, 0}}, {{ s,  s, -s}, {1, 0, 0}, {0, 0, 0}, {1, 1}},
        {{ s,  s,  s}, {1, 0, 0}, {0, 0, 0}, {0, 1}}, {{ s, -s,  s}, {1, 0, 0}, {0, 0, 0}, {0, 0}},
        // Left face (X-)
        {{-s, -s, -s}, {-1,0, 0}, {0, 0, 0}, {0, 0}}, {{-s, -s,  s}, {-1,0, 0}, {0, 0, 0}, {1, 0}},
        {{-s,  s,  s}, {-1,0, 0}, {0, 0, 0}, {1, 1}}, {{-s,  s, -s}, {-1,0, 0}, {0, 0, 0}, {0, 1}}
    });
    for (int i = 0; i < 6; ++i) {
        unsigned int offset = i * 4;
        mesh.value().second->addIndex(offset + 0);
        mesh.value().second->addIndex(offset + 1);
        mesh.value().second->addIndex(offset + 2);

        mesh.value().second->addIndex(offset + 0);
        mesh.value().second->addIndex(offset + 2);
        mesh.value().second->addIndex(offset + 3);
    }
    auto commandBuffer = _commandManager.get().createSingleTimeCommandBuffer();
    mesh->second->loadBuffers(_queueManager.get().graphicsQueue, commandBuffer);

    PBRMaterial material{
        .baseColor = color,
    };
    material.baseColorTextureIndex = NO_TEXTURE_FLAG;
    auto materialIndex = _materialManager.get().insert(material);
    Submesh submesh{
        .materialIndex = materialIndex,
        .indexCount = static_cast<uint32_t>(mesh->second->indices().size())
    };
    auto drawableModel = vax::objects::DrawableModel(_meshManager.get(), mesh.value().first);
    drawableModel._mesh = mesh.value().second;
    drawableModel._submeshes.push_back(submesh);
    return std::make_optional(drawableModel);
}