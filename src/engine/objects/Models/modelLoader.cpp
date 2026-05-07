#include "modelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

using namespace vax::objects;
using namespace vax;

constexpr glm::mat4 toGlm(const aiMatrix4x4& m) {
    return {
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    };
}

PBRMaterial processMaterial(aiMaterial* mat) {
    PBRMaterial material{
        .baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        .metallicFactor = 1.0f,
        .roughnessFactor = 1.0f,
        .normalScale = 1.0f,
        .occlusionStrength = 1.0f,
        .emissiveFactorAlphaCutoff = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
    };
    aiColor4D color;
    ai_real factor;

    if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
        material.baseColor = glm::vec4(color.r, color.g, color.b, color.a);
    }
    std::cout << "baseColor: " << material.baseColor.r << " " << material.baseColor.g << " " << material.baseColor.b << " " << material.baseColor.a << std::endl;
    if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color)) {
        material.emissiveFactorAlphaCutoff = glm::vec4(color.r, color.g, color.b, 0.5f);
    }
    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &factor)) {
        material.metallicFactor = factor;
    }
    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &factor)) {
        material.roughnessFactor = factor;
    }
    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0), &factor)) {
        material.normalScale = factor;
    }
    if (
        AI_SUCCESS == aiGetMaterialFloat(
            mat, AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_AMBIENT_OCCLUSION, 0), &factor
        )
        ) {
        material.occlusionStrength = factor;
    }

    aiString diffuseTextureName;

    mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTextureName);


    std::cout << "diffuseTextureName: " << diffuseTextureName.C_Str() << std::endl;

    return material;
}

void processNode(
    const aiScene* scene,
    const aiNode* node,
    const glm::mat4& parentTransform,
    std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    std::vector<Submesh>& submeshes,
    uint32_t& vertexOffset,
    uint32_t& indexOffset,
    uint32_t depth,
    std::vector<MaterialId>& materialIds,
    const vax::utils::Logger& logger
) {
    glm::mat4 transform = parentTransform * toGlm(node->mTransformation);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        Submesh submesh{
            .indexCount = mesh->mNumFaces * 3,
            .firstIndex = indexOffset,
            .vertexOffset = vertexOffset,
            .materialIndex = materialIds[mesh->mMaterialIndex]
        };
        submeshes.push_back(submesh);
        // submesh.debugPrint(logger);

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

   

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(
            scene,
            node->mChildren[i],
            transform,
            vertices,
            indices,
            submeshes,
            vertexOffset,
            indexOffset,
            depth + 1,
            materialIds,
            logger
        );
    }
}

std::optional<DrawableModel> ModelLoader::loadModel(const std::string& path, VkQueue submitQueue) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
        | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        _logger.error("Failed to load model: " + std::string(importer.GetErrorString()));
        return std::nullopt;
    }

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

    std::vector<PBRMaterial> materials;
    materials.reserve(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        materials.push_back(processMaterial(scene->mMaterials[i]));
    }

    auto materialIds = _resourceManager.get().materialManager().insertMaterials(materials);
    if (materialIds.size() != materials.size()) {
        _logger.error("Failed to load materials");
        return std::nullopt;
    }

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
        materialIds,
        _logger
    );

    auto mesh = _resourceManager.get().meshManager().createEmptyMesh();
    if (!mesh) return std::nullopt;

    (*mesh).second->setVertices(modelVertices);
    (*mesh).second->setIndices(modelIndices);

    auto drawableModel = vax::objects::DrawableModel(_resourceManager.get().meshManager(), mesh->first);
    drawableModel._mesh = (*mesh).second;
    drawableModel._submeshes = submeshes;
    return std::make_optional(drawableModel);
}
