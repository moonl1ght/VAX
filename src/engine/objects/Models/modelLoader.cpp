#include "modelLoader.h"
#include "tiny_obj_loader.h"

std::optional<DrawableModel*> ModelLoader::loadModel(const std::string& path) {
    // Logger::getInstance().log("Loading model: " + path);
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        Logger::getInstance().error(warn + err);
        return std::nullopt;
    }
    std::unique_ptr<vax::objects::Mesh> mesh = std::make_unique<vax::objects::Mesh>(*vkEngine->device);
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.uv = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            mesh->vertices.push_back(vertex);
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices.size());
                mesh->vertices.push_back(vertex);
            }   
            mesh->indices.push_back(uniqueVertices[vertex]);
        }
    }
    return std::make_optional(new DrawableModel(std::move(mesh)));
}