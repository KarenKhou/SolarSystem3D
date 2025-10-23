// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020-2025 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include "Mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_colVbo = 0;
GLuint g_ibo = 0;
GLuint g_texSun=0, g_texEarth=0, g_texMoon=0, g_texMercure=0,
    g_texVenus=0,g_texMars=0,g_texJupiter=0,g_texSaturne=0,g_texUranuss=0,g_texNeptune=0;


// All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
std::vector<float> g_vertexPositions;
// All triangle indices packed in one array [v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle
std::vector<unsigned int> g_triangleIndices;
std::vector<float> g_vertexColors;


// Basic camera model
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) { m_pos = p; }
  inline glm::vec3 getPosition() { return m_pos; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process
};
Camera g_camera;


GLuint loadTextureFromFileToGPU(const std::string &filename) {
    int width, height, numComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numComponents, 0);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}


// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.0f, 0.0f, 0.4f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprogram() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "../../vertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "../../fragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons

  glUseProgram(g_program);
  g_texSun   = loadTextureFromFileToGPU("../../media/sun2.jpg");
  g_texEarth = loadTextureFromFileToGPU("../../media/earth.jpg");
  g_texMoon  = loadTextureFromFileToGPU("../../media/moon.jpg");
  g_texMercure  = loadTextureFromFileToGPU("../../media/mercure.jpg");
  g_texVenus = loadTextureFromFileToGPU("../../media/venus.jpg");
  g_texMars  = loadTextureFromFileToGPU("../../media/mars.jpg");
  g_texJupiter  = loadTextureFromFileToGPU("../../media/jupiter.jpg");
  // g_texSaturne  = loadTextureFromFileToGPU("../../media/saturne.jpg");
  // g_texUranuss  = loadTextureFromFileToGPU("../../media/uranus.jpg");
  // g_texNeptune  = loadTextureFromFileToGPU("../../media/neptune.jpg");


  glUniform1i(glGetUniformLocation(g_program, "material.albedoTex"), 0);
  // TODO: set shader variables, textures, etc.
}

// Define your mesh(es) in the CPU memory
void initCPUgeometry() {
    g_vertexPositions = {
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f
    };
    g_vertexColors = { // the array of vertex colors [r0, g0, b0, r1, g1, b1, ...]
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f
    };
    g_triangleIndices = { 0, 1, 2 }; // indices just for one triangle
}

void initGPUgeometry() {
  // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)

  glGenVertexArrays(1, &g_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.

  glBindVertexArray(g_vao);

  // Generate a GPU buffer to store the positions of the vertices
  size_t colorBufferSize  = sizeof(float) * g_vertexColors.size();
  size_t vertexBufferSize = sizeof(float)*g_vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector

  glGenBuffers(1, &g_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_posVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, g_vertexPositions.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &g_colVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_colVbo);
  glBufferData(GL_ARRAY_BUFFER, colorBufferSize, g_vertexColors.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);

  // Same for an index buffer object that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*g_triangleIndices.size();
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, g_triangleIndices.data(), GL_DYNAMIC_READ);
#else
  glCreateBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glNamedBufferStorage(g_ibo, indexBufferSize, g_triangleIndices.data(), GL_DYNAMIC_STORAGE_BIT);
#endif

  glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 0.0, 23.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1);
}


// Constants
const static float kSizeSun = 1;
const static float kSizeEarth = 0.5;
const static float kSizeMercure = 0.1;
const static float kSizeVenus = 0.4;
const static float kSizeMars = 0.3;
const static float kSizeJupiter = 0.8;
const static float kSizeSaturne = 0.9;
const static float kSizeUranus = 0.6;
const static float kSizeNeptune = 0.6;
const static float kSizeMoon = 0.25;

const static float kRadOrbitEarth = 8;
const static float kRadOrbitMercure = 4;
const static float kRadOrbitMoon = 2;
const static float kRadOrbitVenus = 6;
const static float kRadOrbitMars = 9;
const static float kRadOrbitJupiter = 11;
const static float kRadOrbitSaturne = 13;
const static float kRadOrbitUranus = 14;
const static float kRadOrbitNeptune = 15;


// Transformations (model matrices)
glm::mat4 g_sun, g_earth, g_moon,g_mercure;
glm::mat4 g_venus, g_mars, g_jupiter;


auto sphere =  Mesh::genSphere(32);
void init() {
  initGLFW();
  initOpenGL();
  initCPUgeometry();
  initGPUprogram();
  initGPUgeometry();
  initCamera();
  sphere->init();

}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
  // std::cout << currentTimeInSec << std::endl;

        double t = glfwGetTime() *0.7;

        // Soleil
        g_sun = glm::scale(glm::mat4(1.0f), glm::vec3(kSizeSun));

        // Terre
        float angleEarthRot   = (float)t;
        float angleEarthOrbit = (float)t / 2.0f;

        g_earth = glm::mat4(1.0f);
        g_earth = glm::rotate(g_earth, angleEarthOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_earth = glm::translate(g_earth, glm::vec3(kRadOrbitEarth, 0.0f, 0.0f));
        g_earth = glm::rotate(g_earth, glm::radians(23.5f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_earth = glm::rotate(g_earth, angleEarthRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_earth = glm::scale(g_earth, glm::vec3(kSizeEarth));

        // Lune
        float angleMoonOrbit = (float)t * 2.0f;
        float angleMoonRot   = angleMoonOrbit;

        g_moon = glm::mat4(1.0f);
        g_moon = glm::rotate(g_moon, angleEarthOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_moon = glm::translate(g_moon, glm::vec3(kRadOrbitEarth, 0.0f, 0.0f));
        g_moon = glm::rotate(g_moon, angleMoonOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_moon = glm::translate(g_moon, glm::vec3(kRadOrbitMoon, 0.0f, 0.0f));
        g_moon = glm::rotate(g_moon, angleMoonRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_moon = glm::scale(g_moon, glm::vec3(kSizeMoon));


        // Mercure
        float angleMercureOrbit = (float)t / 0.3f;
        float angleMercureRot   = (float)t / 3.0f;

        g_mercure = glm::mat4(1.0f);
        g_mercure = glm::rotate(g_mercure, angleMercureOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_mercure = glm::translate(g_mercure, glm::vec3(kRadOrbitMercure, 0.0f, 0.0f));
        g_mercure = glm::rotate(g_mercure, glm::radians(0.03f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_mercure = glm::rotate(g_mercure, angleMercureRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_mercure = glm::scale(g_mercure, glm::vec3(kSizeMercure));

        // Venus
        float angleVenusOrbit = (float)t / 0.62f;
        float angleVenusRot   = -(float)t / 10.0f;

        g_venus = glm::mat4(1.0f);
        g_venus = glm::rotate(g_venus, angleVenusOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_venus = glm::translate(g_venus, glm::vec3(kRadOrbitVenus, 0.0f, 0.0f));
        g_venus = glm::rotate(g_venus, glm::radians(177.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_venus = glm::rotate(g_venus, angleVenusRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_venus = glm::scale(g_venus, glm::vec3(kSizeVenus));

        //mars
        float angleMarsOrbit = (float)t / 1.88f + glm::radians(45.0f);
        float angleMarsRot   = (float)t / 1.03f;

        g_mars = glm::mat4(1.0f);
        g_mars = glm::rotate(g_mars, angleMarsOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_mars = glm::translate(g_mars, glm::vec3(kRadOrbitMars, 0.0f, 0.0f));
        g_mars = glm::rotate(g_mars, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_mars = glm::rotate(g_mars, angleMarsRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_mars = glm::scale(g_mars, glm::vec3(kSizeMars));

        // Jupiter
        float angleJupiterOrbit = (float)t / 11.86f;
        float angleJupiterRot   = (float)t * 2.5f;

        g_jupiter = glm::mat4(1.0f);
        g_jupiter = glm::rotate(g_jupiter, angleJupiterOrbit, glm::vec3(0.0f, 1.0f, 0.0f));
        g_jupiter = glm::translate(g_jupiter, glm::vec3(kRadOrbitJupiter, 0.0f, 0.0f));
        g_jupiter = glm::rotate(g_jupiter, glm::radians(3.1f), glm::vec3(1.0f, 0.0f, 0.0f));
        g_jupiter = glm::rotate(g_jupiter, angleJupiterRot, glm::vec3(0.0f, 1.0f, 0.0f));
        g_jupiter = glm::scale(g_jupiter, glm::vec3(kSizeJupiter));



}



// The main rendering call
void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
    const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
    const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();

    glUseProgram(g_program);
    glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    glm::vec3 camPos = g_camera.getPosition();
    glUniform3fv(glGetUniformLocation(g_program, "camPos"), 1, glm::value_ptr(camPos));

    glUniform1i(glGetUniformLocation(g_program, "material.albedoTex"), 0);

    //Soleil
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_sun));
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 1.0f, 1.0f, 0.2f);
    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 1);

    glm::vec3 lightPos = glm::vec3(g_sun[3]); // position du Soleil dans le monde
    glUniform3fv(glGetUniformLocation(g_program, "lightPos"), 1, glm::value_ptr(lightPos));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texSun);
    sphere->render();

    // Terre
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_earth));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.2f, 1.0f, 0.2f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texEarth);
    sphere->render();

    // Lune
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_moon));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.3f, 0.3f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texMoon);
    sphere->render();


    // mercure
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_mercure));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.5f, 0.5f, 0.5f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texMercure);
    sphere->render();

    // venus
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_venus));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.5f, 0.5f, 0.5f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texVenus);
    sphere->render();

    // mars
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_mars));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.5f, 0.5f, 0.5f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texMars);
    sphere->render();

    // Jupiter
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(g_jupiter));

    glUniform1i(glGetUniformLocation(g_program, "isLightSource"), 0);
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.5f, 0.5f, 0.5f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texJupiter);
    sphere->render();

}


int main(int argc, char ** argv) {
  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)
    while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
