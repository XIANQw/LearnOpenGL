#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include <vector>
#include "Mesh.h"


uint32_t TextureFromFile(std::string path, std::string& directory) {
    std::string filename = directory + '/' + path;

    uint32_t textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponent;
    // Binding texture 2D
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set wrap and filter attributs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S and T direction's wrap mode: repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Min and Mag filter mode : linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load data from image
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponent, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrComponent == 1)
            format = GL_RED;
        else if (nrComponent == 3)
            format = GL_RGB;
        else if (nrComponent == 4)
            format = GL_RGBA;
        // Copy image data to texture2D
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // Generate Mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Texture " << path << " : Failed to load texture" << std::endl;
    }
    // Free data
    stbi_image_free(data);

    return textureID;
}

class Model {
private:
    /*  模型数据  */
    std::vector<Mesh<VertexNormalTex>> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

public:
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
        res.textures = textures;
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
                if (std::strcmp(str.C_Str(), tex.path.data()) == 0) {
                    outTextureLoaded.push_back(tex);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                outTextureLoaded.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
    }



};

