// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <algorithm>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

#include "controls.hpp"
#include "objects.hpp"
#include "engine.hpp"
#include "sphere.hpp"

bool is_too_far(const Object& object) {
    return glm::distance(Controls::position, object.center) > 10.0f;
}

template <typename U, typename V>
bool are_close(const U& lhs, const V& rhs) {
    return glm::distance(lhs.center, rhs.center) < lhs.radius + rhs.radius;
}

std::default_random_engine generator;
std::uniform_real_distribution<float> uniform(0.0, 1.0);
 

void create_target(std::vector<Target>& targets, std::vector<glm::vec3>& speeds, int cur_ts) {
    float x = uniform(generator) * 2 * 3.14;
    float h = uniform(generator);
    glm::vec3 center(5 * sin(x), 0.1 + 3 * h, 5 * cos(x));
    GLfloat radius = 0.1f + 0.05 * uniform(generator);
    glm::vec3 angle(
            uniform(generator) * 3.14,
            uniform(generator) * 3.14,
            uniform(generator) * 3.14
    );
    std::vector<GLfloat> color({
           uniform(generator),
           uniform(generator),
           uniform(generator)
    });
    float brightness = std::accumulate(color.begin(), color.end(), 0.f);
    targets.emplace_back(center + Controls::position * 0.5f, radius, angle, color,
            cur_ts + brightness * 1000);
    speeds.emplace_back(
            uniform(generator) / 100,
            uniform(generator) / 100,
            uniform(generator) / 100
            );
}

template <typename T>
void remove_object(std::vector<T>& objects, std::vector<glm::vec3>& speeds, int id=0) {
    if (objects.size() > id) {
        objects.erase(objects.begin() + id);
        speeds.erase(speeds.begin() + id);
    }
}


void create_fireball(std::vector<Fireball>& fireballs, std::vector<glm::vec3>& speeds,
    const glm::vec3& direction) {
    auto fireball = Fireball(0.5, 20);
    fireball.move(Controls::position - glm::vec3(0, 1, 0));
    fireballs.emplace_back(fireball);
    speeds.emplace_back(direction * 0.5f);
}


bool fireball_is_available(size_t iteration, size_t last_shoot_time) {
    return (iteration - last_shoot_time > 20);
}



int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "SnickersHunter", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    
    ///test
    
    GLuint PrID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    GLuint MxID = glGetUniformLocation(PrID, "MVP");
    
    //     Get a handle for our buffers
    GLuint vertexPos_modelspaceID = glGetAttribLocation(PrID, "vertexPosition_modelspace");
    GLuint vertexClrID = glGetAttribLocation(PrID, "vertexColor");
    GLuint vertexUVnewID = glGetAttribLocation(PrID, "vertexUV");
    
    
    std::vector<Target> targets;
    std::vector<glm::vec3> target_speeds;
    std::vector<Fireball> fireballs;
    std::vector<glm::vec3> fireball_speeds;
    
    Buffer buffer;
       Floor floor;

       GLuint vertexbuffernew;
       glGenBuffers(1, &vertexbuffernew);

       GLuint colorbuffernew;
       glGenBuffers(1, &colorbuffernew);

       GLuint uvbuffernew;
       glGenBuffers(1, &uvbuffernew);

       // Load the texture using any two methods
       //GLuint Texture = loadBMP_custom("uvtemplate.bmp");
       GLuint Texturenew = loadBMP_custom("fireearth.bmp");

       // Get a handle for our "myTextureSampler" uniform
       GLuint TextureIDnew  = glGetUniformLocation(PrID, "myTextureSampler");

       size_t iteration = 0;
       size_t last_shoot_time = 0;
    ///test
    
    
    
    
    

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    GLuint programIDGreen = LoadShaders( "TransformVertexShader.vertexshader", "GreenFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint MatrixID2 = glGetUniformLocation(programIDGreen, "MVP");

    // Get a handle for our buffers
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");

    GLuint vertexPosition_modelspaceIDGreen = glGetAttribLocation(programIDGreen, "vertexPosition_modelspaceGreen");
    
    
    // Load the texture
    // GLuint Texture = loadDDS("uvtemplate.DDS");
    GLuint Texture = loadBMP_custom("./snickers-logo.bmp");
    

    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f, -1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,


        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,

        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,

        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,

        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    // Two UV coordinatesfor each vertex. They were created withe Blender.
    static const GLfloat g_uv_buffer_data[] = {
        0.000059f, 0.000004f,
        1.000001f, 0.000000f,
        1.000000f, 1.000000f,

        0.000059f, 1.000004f,
        1.000000f, 0.000000f,
        1.000001f, 1.000000f,

        0.000059f, 1.000004f,
        0.000001f, 0.000000f,
        1.000000f, 0.000000f,

        0.000000f, 0.000000f,
        1.000059f, 1.000004f,
        0.000001f, 1.000000f,

        // bad  v v v v

        0.000059f, 0.000004f,
        1.000001f, 0.000000f,
        1.000000f, 1.000000f,

        0.000059f, 1.000004f,
        1.000000f, 0.000000f,
        1.000001f, 1.000000f,

        0.000059f, 1.000004f,
        0.000001f, 0.000000f,
        1.000000f, 0.000000f,

        0.000000f, 0.000000f,
        1.000059f, 1.000004f,
        0.000001f, 1.000000f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);


    int sphere1_buf_size;
    GLfloat * sphere1_buffer_data = getSphere(4, 4, 3, sphere1_buf_size);
    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphere1_buf_size, sphere1_buffer_data,  GL_STATIC_DRAW);

//    GLuint uvbuffer2;
//    glGenBuffers(1, &uvbuffer2);
//    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    Engine eng;

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        buffer.clear();
        
        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
//        computeMatricesFromInputs();
        eng.computeMatricesFromInputs(window);

        glm::mat4 ProjectionMatrix = eng.getProjectionMatrix();
        glm::mat4 ViewMatrix = eng.getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            vertexPosition_modelspaceID,  // The attribute we want to configure
            3,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(vertexUVID);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            vertexUVID,                   // The attribute we want to configure
            2,                            // size : U+V => 2
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
        
        // Start draw 2nd object ------------------------------------------------------------
        
        // Use our shader
        glUseProgram(programID);
        
        glm::mat4 ModelMatrix2 = glm::mat4(1.0);
        ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(4.0f, 0.0f, -5.0f));
        glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;
    
        // Compute the MVP matrix from keyboard and mouse input
//        computeMatricesFromInputs();
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            vertexPosition_modelspaceID,  // The attribute we want to configure
            3,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(vertexUVID);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            vertexUVID,                   // The attribute we want to configure
            2,                            // size : U+V => 2
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);

        
        
        /// test fireball
        
        // create targets
        if (uniform(generator) < 0.3) {  // 0.03
            create_target(targets, target_speeds, iteration);
        }

        // remove objects that are too far
        for (size_t i = 0; i < targets.size(); ++i) {
            if (targets[i].expired(iteration)) {
                remove_object(targets, target_speeds, i);
            }
        }

        bool has_collision = false;
        // remove collided objects
        for (size_t i = 0; i < targets.size(); ++i) {
            for (size_t j = 0; j < fireballs.size(); ++j) {
                if (are_close(targets[i], fireballs[j])) {
                    std::cout << "COLLIDE" << std::endl;
                    remove_object(targets, target_speeds, i);
                    remove_object(fireballs, fireball_speeds, j);
                    has_collision = true;
                    break;
                }
            }
        }
        if (has_collision) {
            glClearColor(1.0f, 1.0f, 0.2f, 0.0f);
        } else {
            glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        }
        
        if (Controls::isSpacePressed(window) && fireball_is_available(iteration, last_shoot_time)) {
            last_shoot_time = iteration;
            std::cout << "Fire!\n";
            create_fireball(fireballs, fireball_speeds, Controls::direction);
        }

        floor.draw(buffer);
        for (size_t i = 0; i < targets.size(); ++i) {
            targets[i].move(target_speeds[i]);
            targets[i].draw(buffer);
        }

        for (size_t i = 0; i < fireballs.size(); ++i) {
            fireballs[i].move(fireball_speeds[i]);
            fireballs[i].draw(buffer);
        }
        
        // Get position from controls
        Controls::computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrixnew = Controls::getProjectionMatrix();
        glm::mat4 ViewMatrixnew = Controls::getViewMatrix();
        glm::mat4 ModelMatrixnew = glm::mat4(1.0);
        glm::mat4 MVPnew = ProjectionMatrixnew * ViewMatrixnew * ModelMatrixnew;
        
        glUseProgram(PrID);
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MxID, 1, GL_FALSE, &MVP[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texturenew);
        glUniform1i(TextureIDnew, 0);
        
        
        // 1rst attribute buffer : vertices
           glEnableVertexAttribArray(vertexPos_modelspaceID);
           glBindBuffer(GL_ARRAY_BUFFER, vertexbuffernew);
           glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffer.size(), buffer.vertex_data(), GL_STATIC_DRAW);
           glVertexAttribPointer(vertexPos_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

           // 2nd attribute buffer : colors
           glEnableVertexAttribArray(vertexClrID);
           glBindBuffer(GL_ARRAY_BUFFER, colorbuffernew);
           glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffer.size(), buffer.color_data(), GL_STATIC_DRAW);
           glVertexAttribPointer(vertexClrID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

           // 3rd attribute buffer : textures
           glEnableVertexAttribArray(vertexUVnewID);
           glBindBuffer(GL_ARRAY_BUFFER, uvbuffernew);
           glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffer.texture_size(), buffer.texture_data(), GL_STATIC_DRAW);
           glVertexAttribPointer(vertexUVnewID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

           glDrawArrays(GL_TRIANGLES, 0, buffer.size() / 3);

           glDisableVertexAttribArray(vertexPos_modelspaceID);
           glDisableVertexAttribArray(vertexClrID);
           glDisableVertexAttribArray(vertexUVnewID);
               // Swap buffers
               glfwSwapBuffers(window);
               glfwPollEvents();
               ++iteration;
        ///test fireball
        
        
        
        
        
        
        
        
        // SPHERE ---------------------------------------------------------------------------------

        glUseProgram(programIDGreen);

        glm::mat4 ModelMatrix3 = glm::mat4(1.0);
        ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(-4.0f, 0.0f, -5.0f));
        glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP3[0][0]);

        glEnableVertexAttribArray(vertexPosition_modelspaceIDGreen);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        glVertexAttribPointer(
            vertexPosition_modelspaceIDGreen,  // The attribute we want to configure
            3,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
        );

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, sphere1_buf_size); // 12*3 indices starting at 0 -> 12 triangles

        // FINISH ---------------------------------------------------------------------------------

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &TextureID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

