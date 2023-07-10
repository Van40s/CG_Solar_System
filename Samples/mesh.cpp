// Preprocessor Directives
#define STB_IMAGE_IMPLEMENTATION

// Local Headers
#include "mesh.hpp"

// System Headers
#include <stb_image.h>
#include <memory>

// Define Namespace
namespace Mirage
{
    Mesh::Mesh(std::string const & filename) : Mesh()
    {
        // Load a Model from File
        Assimp::Importer loader;
        aiScene const * scene = loader.ReadFile(
            PROJECT_SOURCE_DIR "/Mirage/Models/" + filename,
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_OptimizeGraph                   |
            aiProcess_FlipUVs);

        // Walk the Tree of Scene Nodes
        auto index = filename.find_last_of("/");
        if (!scene) fprintf(stderr, "%s\n", loader.GetErrorString());
        else parse(filename.substr(0, index), scene->mRootNode, scene);
    }

    Mesh::Mesh(std::vector<Vertex> const & vertices,
               std::vector<GLuint> const & indices,
               std::map<GLuint, std::string> const & textures)
                    : mIndices(indices)
                    , mVertices(vertices)
                    , mTextures(textures)
    {
        // Bind a Vertex Array Object
        glGenVertexArrays(1, & mVertexArray);
        glBindVertexArray(mVertexArray);

        // Copy Vertex Buffer Data
        glGenBuffers(1, & mVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     mVertices.size() * sizeof(Vertex),
                   & mVertices.front(), GL_STATIC_DRAW);

        // Copy Index Buffer Data
        glGenBuffers(1, & mElementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mIndices.size() * sizeof(GLuint),
                   & mIndices.front(), GL_STATIC_DRAW);

        // Set Shader Attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, Normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, uv));
        glEnableVertexAttribArray(0); // Vertex Positions
        glEnableVertexAttribArray(1); // Vertex Normals
        glEnableVertexAttribArray(2); // Vertex UVs

        // Cleanup Buffers
        glBindVertexArray(0);
        glDeleteBuffers(1, & mVertexBuffer);
        glDeleteBuffers(1, & mElementBuffer);
    }

    void Mesh::draw(GLuint shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::parse(std::string const & path, aiNode const * node, aiScene const * scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            parse(path, scene->mMeshes[node->mMeshes[i]], scene);
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            parse(path, node->mChildren[i], scene);
    }

    void Mesh::setupMesh() {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }

    void Mesh::parse(std::string const & path, aiMesh const * mesh, aiScene const * scene)
    {
        // Create Vertex Data from Mesh Node
        std::vector<Vertex> vertices; Vertex vertex;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {   if (mesh->mTextureCoords[0])
            vertex.uv       = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal   = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
            vertices.push_back(vertex);
        }

        // Create Mesh Indices for Indexed Drawing
        std::vector<GLuint> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

        // Load Mesh Textures into VRAM
        std::map<GLuint, std::string> textures;
        auto diffuse  = process(path, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);
        auto specular = process(path, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR);
        textures.insert(diffuse.begin(), diffuse.end());
        textures.insert(specular.begin(), specular.end());

        // Create New Mesh Node
        mSubMeshes.push_back(std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures)));
    }

    std::map<GLuint, std::string> Mesh::process(std::string const & path,
                                                aiMaterial * material,
                                                aiTextureType type)
    {
        std::map<GLuint, std::string> textures;
        for(unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            // Define Some Local Variables
            GLenum format;
            GLuint texture;
            std::string mode;

            // Load the Texture Image from File
            aiString str; material->GetTexture(type, i, & str);
            std::string filename = str.C_Str(); int width, height, channels;
            filename = PROJECT_SOURCE_DIR "/Mirage/Models/" + path + "/" + filename;
            unsigned char * image = stbi_load(filename.c_str(), & width, & height, & channels, 0);
            if (!image) fprintf(stderr, "%s %s\n", "Failed to Load Texture", filename.c_str());

            // Set the Correct Channel Format
            switch (channels)
            {
                case 1 : format = GL_ALPHA;     break;
                case 2 : format = GL_LUMINANCE; break;
                case 3 : format = GL_RGB;       break;
                case 4 : format = GL_RGBA;      break;
            }

            // Bind Texture and Set Filtering Levels
            glGenTextures(1, & texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, format,
                         width, height, 0, format, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Release Image Pointer and Store the Texture
            stbi_image_free(image);
                 if (type == aiTextureType_DIFFUSE)  mode = "diffuse";
            else if (type == aiTextureType_SPECULAR) mode = "specular";
            textures.insert(std::make_pair(texture, mode));
        }   return textures;
    }
};
