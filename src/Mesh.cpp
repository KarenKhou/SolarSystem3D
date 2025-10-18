#include "Mesh.h"

std::shared_ptr<Mesh> Mesh::genSphere(const size_t resolution) {
    auto mesh = std::make_shared<Mesh>();

    const float PI = 3.14159265359f;
    const size_t nbrdedivverticale = resolution;
    const size_t nbrdedivhorizontale = resolution;

    // --- Génération des sommets ---
    for (size_t i = 0; i <= nbrdedivverticale; ++i) {
        float phi = PI * float(i) / float(nbrdedivverticale); // de 0 à π
        for (size_t j = 0; j <= nbrdedivhorizontale; ++j) {
            float theta = 2.0f * PI * float(j) / float(nbrdedivhorizontale); // de 0 à 2π

            // Coordonnées sphériques
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            mesh->m_vertexPositions.push_back(x);
            mesh->m_vertexPositions.push_back(y);
            mesh->m_vertexPositions.push_back(z);

            // Normales (identiques à la position car sphère unitaire)
            mesh->m_vertexNormals.push_back(x);
            mesh->m_vertexNormals.push_back(y);
            mesh->m_vertexNormals.push_back(z);

            float u = float(j) / float(nbrdedivhorizontale);
            float v = float(i) / float(nbrdedivverticale);

            mesh->m_vertexTexCoords.push_back(u);
            mesh->m_vertexTexCoords.push_back(v);


        }
    }

    // --- Génération des triangles ---
    /*
     * Par rectangle on fait
    1er triangle : (first, second, first+1)
    2e triangle : (second, second+1, first+1)
    */
    for (size_t i = 0; i < nbrdedivverticale; ++i) {
        for (size_t j = 0; j < nbrdedivhorizontale; ++j) {
            unsigned int first  = i * (nbrdedivhorizontale + 1) + j;
            unsigned int second = first + nbrdedivhorizontale + 1;

            mesh->m_triangleIndices.push_back(first);
            mesh->m_triangleIndices.push_back(first + 1);
            mesh->m_triangleIndices.push_back(second);

            mesh->m_triangleIndices.push_back(second);
            mesh->m_triangleIndices.push_back(first + 1);
            mesh->m_triangleIndices.push_back(second + 1);

        }
    }
    std::vector<float> g_vertexColors;
    g_vertexColors = { // the array of vertex colors [r0, g0, b0, r1, g1, b1, ...]
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f
    };

    return mesh;
}
void Mesh::init() {
    // Crée et active le VAO (Vertex Array Object)
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // --- Positions des sommets ---
    glGenBuffers(1, &m_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * m_vertexPositions.size(),
                 m_vertexPositions.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,                // location = 0 dans le shader
        3,                // 3 composantes (x, y, z)
        GL_FLOAT,         // type
        GL_FALSE,         // ne pas normaliser
        3 * sizeof(float),// taille d’un vertex
        (void*)0);        // offset
    glEnableVertexAttribArray(0);

    // --- Normales des sommets ---
    glGenBuffers(1, &m_normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * m_vertexNormals.size(),
                 m_vertexNormals.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                // location = 1 dans le shader
        3,                // 3 composantes (nx, ny, nz)
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(1);

    // --- Indices des triangles ---
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * m_triangleIndices.size(),
                 m_triangleIndices.data(),
                 GL_STATIC_DRAW);

    // --- Nettoyage ---
    glBindVertexArray(0); // désactive le VAO


    glGenBuffers(1, &m_texCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * m_vertexTexCoords.size(),
                 m_vertexTexCoords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

}


void Mesh::render() {
    glBindVertexArray(m_vao); //je dois rebind car g desactive le vao dans init
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); //je desactive pr eviter les erreures.
}
