#pragma once

// System Headers
#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

// Standard Headers
#include <map>
#include <memory>
#include <vector>
#include <string>

#define MAX_BONE_INFLUENCE 4

// Define Namespace
namespace Mirage
{
    struct Vertex {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
        glm::vec2 uv;
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };


    class Mesh
    {
    public:

        // mesh Data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;
        unsigned int VAO;

        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }

        // Implement Default Constructor and Destructor
         Mesh() { glGenVertexArrays(1, & mVertexArray); }
        ~Mesh() { glDeleteVertexArrays(1, & mVertexArray); }

        // Implement Custom Constructors
        Mesh(std::string const & filename);
        Mesh(std::vector<Vertex> const & vertices,
             std::vector<GLuint> const & indices,
             std::map<GLuint, std::string> const & textures);



        // Public Member Functions
        void draw(GLuint shader);


        Mesh(Mesh const &) = default;

    private:

        // Disable Copying and Assignment
        Mesh & operator=(Mesh const &) = delete;

        // Private Member Functions
        void parse(std::string const & path, aiNode const * node, aiScene const * scene);
        void parse(std::string const & path, aiMesh const * mesh, aiScene const * scene);
        std::map<GLuint, std::string> process(std::string const & path,
                                              aiMaterial * material,
                                              aiTextureType type);

        void setupMesh();

        // Private Member Containers
        std::vector<std::shared_ptr<Mesh>> mSubMeshes;
        std::vector<GLuint> mIndices;
        std::vector<Vertex> mVertices;
        std::map<GLuint, std::string> mTextures;

        // Private Member Variables
        GLuint mVertexArray;
        GLuint mVertexBuffer;
        GLuint mElementBuffer;

        // render data
        unsigned int VBO, EBO;
    };
};
