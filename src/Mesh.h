#ifndef MESH_H
#define MESH_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <cmath>


class Mesh {
public:
    void init();
    void render();
    static std::shared_ptr<Mesh> genSphere(size_t resolution = 16);

private:
    std::vector<float> m_vertexPositions;
    std::vector<float> m_vertexNormals;
    std::vector<unsigned int> m_triangleIndices;
    std::vector<float> g_vertexColors;
    std::vector<float> m_vertexTexCoords;

    GLuint m_texCoordVbo = 0;
    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_ibo = 0;
    GLuint g_colVbo=0;
};

#endif // MESH_H
