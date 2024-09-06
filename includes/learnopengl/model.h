#pragma once
#include <glad/glad.h>                 //所有头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#define STB_IMAGE_IMPLEMENTATION        //原作者没写
#include <stb_image.h>
#include <assimp/Importer.hpp>        //assimp库头文件
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
//从文件中读取纹理
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

//Model类
// 相当于
class Model
{
public:
    vector<Texture> textures_loaded;// 保存一个model的所有的texture
    vector<Mesh> meshes;// 每个mesh都有自己的Texture集合
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    /// model的绘制是以mesh为单位进行的
    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // 解析过的path也不应该再解析，加载过的模型不应该再加载
    void loadModel(string const &path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // 唯一根节点 scene->mRootNode
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "错误::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // 模型路径，不包含模型名
        directory = path.substr(0, path.find_last_of('/'));

        // 从Model的根节点开始解析
        processNode(scene->mRootNode, scene);
    }

    // 递归解析节点
    /// 解析model中所有的mesh
    void processNode(aiNode *node, const aiScene *scene)
    {
        // 叶子节点：如果本节点有网格mesh，解析每一个mesh
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//            auto name = mesh->mName;
            meshes.push_back(processMesh(mesh, scene));
        }

        // 如果本节点有子节点
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // 解析一个mesh最后得到VBO EBO texture
    // 解析mesh就是解析obj，副切线与切线是assimp计算得到的
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // 要填写的数据
        vector<Vertex>       vertices;// vertex
        vector<unsigned int> indices; // EBO
        vector<Texture>      textures;// texture

        // 遍历mesh的每一个顶点
        // VBO
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            // v 顶点位置 pos
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // vt 顶点纹理
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);


            // vn 顶点法线
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // 切线副切线都是assimp根据模型数据计算出来的
            // T
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // B
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;

            // 一个条目的顶点数据 pos/tex/norm/tan/btan
            vertices.push_back(vertex);
        }

        // EBO
        // obj文件中的一行f数据就是一个面
        /// 遍历一个mesh中的所有的面
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            // 基本就是三个
            /// 遍历一个面中的所有顶点
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // 每个网格都对应一个材质
        /// 有没有可能一个网格对应多个材质？
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 我们假设着色器中的采样器名称约定。 每个漫反射纹理应命名为'texture_diffuseN'，其中N是从1到MAX_SAMPLER_NUMBER的序列号。
        //同样适用于其他纹理，如下列总结：
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 一个材质可能会包含多张纹理
        /// 将所有纹理按类型整理并添加
        // 1. duffuse
        vector<Texture> diffuseMaps;
        loadMaterialTextures(diffuseMaps,material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. spec
        vector<Texture> specularMaps;
        loadMaterialTextures(specularMaps,material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3.normal
        std::vector<Texture> normalMaps;
        loadMaterialTextures(normalMaps,material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. height
        std::vector<Texture> heightMaps;
        loadMaterialTextures(heightMaps,material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    void loadMaterialTextures(vector<Texture>& textures , aiMaterial *mat, aiTextureType type, string typeName)
    {
//		vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);// 获取到纹理的name_str
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                // 如果纹理加载到显存，并已经添加到textures_loaded中，textures_loaded就不再重复添加了
                // 避免纹理重复加载到显存
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            // 避免重复加载
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
//		return textures;
    }
};

//从磁盘加载纹理到显存，并返回一个texture_ID
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (     nrComponents == 1)format = GL_RED;
        else if (nrComponents == 3)format = GL_RGB;
        else if (nrComponents == 4)format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "纹理无法从此路径加载: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}


