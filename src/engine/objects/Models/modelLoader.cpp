#include "modelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace vax::objects;
using namespace vax;

static glm::mat4 toGlm(const aiMatrix4x4& m) {
    // Assimp is row-major, GLM is column-major
    return {
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    };
}

static void processNode(
    const aiScene* scene,
    const aiNode* node,
    const glm::mat4& parentTransform,
    std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    std::vector<Submesh>& submeshes,
    uint32_t& vertexOffset,
    uint32_t& indexOffset,
    uint32_t depth,
    const vax::utils::Logger& logger
) {
    aiVector3D pos, scale;
    aiQuaternion rot;
    node->mTransformation.Decompose(scale, rot, pos);
    // std::cout << "Node: " << node->mName.C_Str()
    //           << "  pos=(" << pos.x << "," << pos.y << "," << pos.z << ")"
    //           << "  scale=(" << scale.x << "," << scale.y << "," << scale.z << ")"
    //           << "  meshes=" << node->mNumMeshes << std::endl;
    glm::mat4 transform = parentTransform * toGlm(node->mTransformation);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));

    for (uint32_t i = 0; i < depth; ++i) {
        std::cout << "-";
    }
    std::cout << "Node: " << node->mName.C_Str() << " pos: (" << pos.x << "," << pos.y << "," << pos.z << ")" << " scale: (" << scale.x << "," << scale.y << "," << scale.z << ")" << "parent: " << parentTransform[0][0] << "," << parentTransform[0][1] << "," << parentTransform[0][2] << "," << parentTransform[0][3] << "," << parentTransform[1][0] << "," << parentTransform[1][1] << "," << parentTransform[1][2] << "," << parentTransform[1][3] << "," << parentTransform[2][0] << "," << parentTransform[2][1] << "," << parentTransform[2][2] << "," << parentTransform[2][3] << "," << parentTransform[3][0] << "," << parentTransform[3][1] << "," << parentTransform[3][2] << "," << parentTransform[3][3] << std::endl;

    // if (node->mNumMeshes > 0) {
    //     std::cout << "Node: " << node->mName.C_Str()
    //               << "  pos=(" << pos.x << "," << pos.y << "," << pos.z << ")"
    //               << "  scale=(" << scale.x << "," << scale.y << "," << scale.z << ")"
    //               << "  meshes=" << node->mNumMeshes << std::endl;
    //     }

    // if (node->mName.C_Str() == "sweepers_low_sweeper_0") {
        

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        Submesh submesh{
            .indexCount = mesh->mNumFaces * 3,
            .firstIndex = indexOffset,
            .vertexOffset = vertexOffset,
            .materialIndex = mesh->mMaterialIndex
        };
        submeshes.push_back(submesh);
        submesh.debugPrint(logger);

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            Vertex vertex;
            glm::vec4 pos = glm::vec4(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z, 1.0f);
            pos = transform * pos;
            vertex.position = glm::vec3(pos);
            vertex.normal = normalMatrix * glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            if (mesh->mTextureCoords[0]) {
                vertex.uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            }
            vertices.push_back(vertex);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        vertexOffset += mesh->mNumVertices;
        indexOffset += submesh.indexCount;
    }
// }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(scene, node->mChildren[i], transform, vertices, indices, submeshes, vertexOffset, indexOffset, depth + 1, logger);
    }
}

std::optional<DrawableModel> ModelLoader::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        _logger.error("Failed to load model: " + std::string(importer.GetErrorString()));
        return std::nullopt;
    }

    _logger.debug("Loaded model: {}", path);

    size_t totalVertexCount = 0;
    size_t totalIndexCount = 0;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        totalVertexCount += scene->mMeshes[i]->mNumVertices;
        totalIndexCount += scene->mMeshes[i]->mNumFaces * 3;
    }

    std::vector<Vertex> modelVertices;
    modelVertices.reserve(totalVertexCount);

    std::vector<uint32_t> modelIndices;
    modelIndices.reserve(totalIndexCount);

    std::vector<Submesh> submeshes;
    uint32_t currentVertexOffset = 0;
    uint32_t currentIndexOffset = 0;

    submeshes.reserve(scene->mNumMeshes);

    processNode(
        scene,
        scene->mRootNode,
        glm::mat4(1.0f),
        modelVertices,
        modelIndices,
        submeshes,
        currentVertexOffset,
        currentIndexOffset,
        0,
        _logger
    );

    // for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
    //     const aiMesh* mesh = scene->mMeshes[i];
    //     Submesh submesh{
    //         .indexCount = mesh->mNumFaces * 3,
    //         .firstIndex = currentIndexOffset,
    //         .vertexOffset = currentVertexOffset,
    //         .materialIndex = mesh->mMaterialIndex
    //     };
    //     submeshes.push_back(submesh);
    //     submesh.debugPrint(_logger);

    //     for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
    //         Vertex vertex;
    //         glm::vec4 pos = glm::vec4(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z, 1.0f);
    //         pos = toGlm(scene->mRootNode->mTransformation) * pos;
    //         vertex.position = {pos.x, pos.y, pos.z};
    //         vertex.normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
    //         vertex.uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
    //         modelVertices.push_back(vertex);
    //     }
    
    //     for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
    //         aiFace face = mesh->mFaces[f];
    //         for (unsigned int j = 0; j < face.mNumIndices; j++) {
    //             modelIndices.push_back(face.mIndices[j] + currentVertexOffset);
    //         }
    //     }
    
    //     currentVertexOffset += mesh->mNumVertices;
    //     currentIndexOffset += submesh.indexCount;
    // }

    // for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
    //     aiMaterial* mat = scene->mMaterials[i];

    //     aiTextureType types[] = { aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR };

    //     for (auto type : types) {
    //         for (unsigned int j = 0; j < mat->GetTextureCount(type); j++) {
    //             aiString texPath;
    //             mat->GetTexture(type, j, &texPath);
    //             std::cout << "  Type " << type << " Path: " << texPath.C_Str() << "\n";
    //         }
    //     }
    // }

    auto mesh = _resourceManager.get().meshManager().createEmptyMesh();
    if (!mesh) return std::nullopt;

    (*mesh).second->setVertices(modelVertices);
    (*mesh).second->setIndices(modelIndices);
    // (*mesh).second->setSubmeshes(submeshes);

    auto drawableModel = vax::objects::DrawableModel(_resourceManager.get().meshManager(), mesh->first);
    drawableModel._mesh = (*mesh).second;
    return std::make_optional(drawableModel);
}
