#include "modelLoader.h"
#include "tiny_obj_loader.h"

using namespace vax::objects;

std::optional<DrawableModel*> ModelLoader::loadModel(const std::string& path) {
    // tinyobj::attrib_t attrib;
    // std::vector<tinyobj::shape_t> shapes;
    // std::vector<tinyobj::material_t> materials;
    // std::string warn, err;

    // if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
    //     _logger.error("Failed to load model: " + warn + err);
    //     return std::nullopt;
    // }
    // std::unique_ptr<vax::objects::Mesh> mesh = std::make_unique<vax::objects::Mesh>(*vkEngine->device);
    // std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // for (const auto& shape : shapes) {
    //     for (const auto& index : shape.mesh.indices) {
    //         Vertex vertex{};

    //         vertex.position = {
    //             attrib.vertices[3 * index.vertex_index + 0],
    //             attrib.vertices[3 * index.vertex_index + 1],
    //             attrib.vertices[3 * index.vertex_index + 2]
    //         };

    //         vertex.uv = {
    //             attrib.texcoords[2 * index.texcoord_index + 0],
    //             1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
    //         };

    //         vertex.color = { 1.0f, 1.0f, 1.0f };

    //         mesh->addVertex(vertex);
    //         if (uniqueVertices.count(vertex) == 0) {
    //             uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices().size());
    //             mesh->addVertex(vertex);
    //         }   
    //         mesh->addIndex(uniqueVertices[vertex]);
    //     }
    // }
    // auto drawableModel = vax::objects::DrawableModel(vkEngine->resourceManager->meshManager(), mesh->first);
    return std::nullopt;
}