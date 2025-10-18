#version 330 core            // Minimal GL version support expected from the GPU
// layout(location=0) in vec3 vPosition;
// layout(location=1) in vec3 vColor;
// uniform mat4 viewMat, projMat;
// out vec3 fColor;
// // ...
// void main() {
//         fColor = vColor; // will be passed to the next stage
//         gl_Position = projMat * viewMat * vec4(vPosition, 1.0); // mandatory to rasterize properly
// }



// layout(location = 0) in vec3 vPosition;
// layout(location = 1) in vec3 vNormal;

// uniform mat4 viewMat;
// uniform mat4 projMat;

// out vec3 fPosition;   // position du fragment dans l’espace monde
// out vec3 fNormal;     // normale associée

// void main()
// {
//     // ⚙️ On passe la position du vertex dans l’espace monde
//     fPosition = vPosition;

//     // ⚙️ On passe la normale au fragment shader
//     fNormal = vNormal;

//     // ⚙️ Transformation classique de la position pour la projection
//     gl_Position = projMat * viewMat * vec4(vPosition, 1.0);
// }

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;


uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;

void main() {
    fPosition = vec3(modelMat * vec4(vPosition, 1.0));
    fNormal   = mat3(modelMat) * vNormal;

    fTexCoords = vTexCoords;

    gl_Position = projMat * viewMat * vec4(fPosition, 1.0);
}
