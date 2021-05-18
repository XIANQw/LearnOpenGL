#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include <vector>
#include "Mesh.h"

class Model {
public:
    /*  模型数据  */
    std::vector<Mesh<VertexNormalTex>> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;


	Model() = default;
	Model(std::string path) {
		loadModel(path);
	}
    void Draw(Shader shader) {
        for (Mesh<VertexNormalTex>& mesh:meshes) {
            mesh.draw(shader);
        }
    }

private:
    /*  函数   */
    void loadModel(std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }


    void processNode(aiNode* node, const aiScene* scene) {
        for (auto i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (auto i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    
    
    Mesh<VertexNormalTex> processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<VertexNormalTex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;
           
        // Generate vertex
        for (auto i = 0; i < mesh->mNumVertices; i++) {
            VertexNormalTex vertex;
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0])
                vertex.texcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.texcoord = glm::vec2(0);
            vertices.push_back(vertex);
        }

        // Set index
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // Set material 
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            loadMaterialTextures(material, aiTextureType_DIFFUSE, texture_type::t_diffusemap, textures);
            loadMaterialTextures(material, aiTextureType_SPECULAR, texture_type::t_specularmap, textures);
        }
        auto res = Mesh<VertexNormalTex>(vertices, indices);
        res.material.textures = textures;
        return res;
    }
    
    
    void loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        texture_type typeName, std::vector<Texture>& outTextureLoaded) {
        for (auto i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check whether texture was loaded
            bool skip = false;
            for (auto& tex:textures_loaded) {
                if (std::strcmp(str.C_Str(), tex.m_path.c_str()) == 0) {
                    outTextureLoaded.push_back(tex);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                std::string filename = directory + "/" + std::string(str.C_Str());
                Texture texture = TextureImporter::importTexture(filename.c_str(), typeName);
                outTextureLoaded.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
    }



};

