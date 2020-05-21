// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

double randomm(double max=1.0f) {
    return (max * ((double) std::rand() / RAND_MAX));
}

struct Object {
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float angle;
    float speed;
    float radius;

    glm::mat4 getModelMatrix() const {
        glm::mat4 ModelMatrix = glm::scale(vec3(radius, radius ,radius));
        ModelMatrix = glm::rotate(angle, rotationAxis) * ModelMatrix;
        ModelMatrix = glm::translate(glm::mat4(), position) * ModelMatrix;
        return ModelMatrix;
    }

};

bool areClose(const Object& fst, const Object& snd) {
    return glm::distance(fst.position, snd.position) < ((fst.radius + snd.radius));
}

struct Enemy : public Object {
    float birthTime;
    vec3 direction;
    float speed;
    Enemy() {
        speed = 0.0228f;
        radius = 1.0f;
        birthTime = glfwGetTime();
        position = vec3(randomm(40) - 20 + getPosition()[0], randomm(40), randomm(40) -20 + getPosition()[2]);
        direction = getPosition() - position;
        rotationAxis = vec3(randomm(), randomm(), randomm());
        angle = randomm(360);
    }

    void move(float deltaTime) {
        position += direction *  (speed * deltaTime);
    }

};

struct Fireball : public Object {
    float birthTime;
    vec3 direction;
    Fireball() {
        speed = 15.0f;
        radius = 0.1f;
        birthTime = glfwGetTime();
        direction = getDirection();
        rotationAxis = vec3(0, 1, 0);
        position = getPosition() + direction * (radius + 0.5f);
    }
    void move(float deltaTime) {
        position += direction *  (speed * deltaTime);
    }
};


class Floor{
public:
    Floor() {
        FloorID = LoadShaders("FloorVertexShader.vertexshader",
              "FloorFragmentShader.fragmentshader" );
        FloorMatrixID = glGetUniformLocation(FloorID, "MVP");

        static GLfloat *floorVertexData =
            new GLfloat[18 * 144];
        static GLfloat *colorData =
            new GLfloat[18 * 144];

        int vindex = 0;
        int cindex = 0;
        GLfloat CELL_SIZE = 5.0f;
        for (int i = -6; i < 6; ++i) {
            for (int j = -6; j < 6; ++j) {
                floorVertexData[vindex++] = CELL_SIZE * i;
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * j;

                floorVertexData[vindex++] = CELL_SIZE * (i + 1);
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * j;

                floorVertexData[vindex++] = CELL_SIZE * (i + 1);
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * (j + 1);


                floorVertexData[vindex++] = CELL_SIZE * i;
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * j;

                floorVertexData[vindex++] = CELL_SIZE * i;
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * (j + 1);

                floorVertexData[vindex++] = CELL_SIZE * (i + 1);
                floorVertexData[vindex++] = 0.0f;
                floorVertexData[vindex++] = CELL_SIZE * (j + 1);

                GLfloat cell_color = ((i + j) & 1 ? 0.2f : 0.8f);

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;

                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
                colorData[cindex++] = cell_color;
            }
        }

        glGenBuffers(1, &floorVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 3 * 3 * 8 * 144, floorVertexData, GL_STATIC_DRAW);

        glGenBuffers(1, &floorColorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, floorColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, 3 * 3 * 8 * 144, colorData, GL_STATIC_DRAW);
    }

    ~Floor() {
        glDeleteBuffers(1, &floorVertexBuffer);
        glDeleteProgram(FloorID);
    }

    void draw(glm::mat4 &MVP) {
        glUseProgram(FloorID);
        glUniformMatrix4fv(FloorMatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Bind our texture in Texture Unit 0
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        // glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer : UVs
        // glEnableVertexAttribArray(1);
        // glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, floorColorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);



        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 144*3*2);
    }

    GLuint FloorMatrixID;
    GLuint FloorID;
    GLuint floorVertexBuffer;
    GLuint floorColorBuffer;
};


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>


GLfloat* getSphere(int vSteps, int hSteps, GLfloat R, int &bufSize)
{
    int total_triangles = 2 * (vSteps - 2) * hSteps; // + 1 * 2 * circleSteps;
    int total_buffer_size = 3 * 3 * total_triangles;

    bufSize = total_buffer_size;

    GLfloat* buffer = new GLfloat[total_buffer_size];

    int b_ind = 0;
    for (int theta_frac = 1; theta_frac < vSteps - 1; ++theta_frac) {
        for (int alpha_frac = 0; alpha_frac < hSteps; ++alpha_frac) {
            GLfloat theta_0 = M_PI * theta_frac / vSteps;
            GLfloat theta_1 = M_PI * (theta_frac + 1) / vSteps;

            GLfloat alpha_0 = 2 * M_PI * alpha_frac / hSteps;
            GLfloat alpha_1 = 2 * M_PI * (alpha_frac + 1) / hSteps;

            GLfloat point0[] = {
                R * sin(theta_0) * sin(alpha_0),
                R * cos(theta_0),
                R * sin(theta_0) * cos(alpha_0)
            };
            GLfloat point1[] = {
                R * sin(theta_0) * sin(alpha_1),
                R * cos(theta_0),
                R * sin(theta_0) * cos(alpha_1)
            };
            GLfloat point2[] = {
                R * sin(theta_1) * sin(alpha_0),
                R * cos(theta_1),
                R * sin(theta_1) * cos(alpha_0)
            };
            GLfloat point3[] = {
                R * sin(theta_1) * sin(alpha_1),
                R * cos(theta_1),
                R * sin(theta_1) * cos(alpha_1)
            };
            buffer[b_ind  ] = point0[0];
            buffer[b_ind+1] = point0[1];
            buffer[b_ind+2] = point0[2];

            buffer[b_ind+3] = point1[0];
            buffer[b_ind+4] = point1[1];
            buffer[b_ind+5] = point1[2];

            buffer[b_ind+6] = point2[0];
            buffer[b_ind+7] = point2[1];
            buffer[b_ind+8] = point2[2];


            buffer[b_ind+9] = point2[0];
            buffer[b_ind+10] = point2[1];
            buffer[b_ind+11] = point2[2];

            buffer[b_ind+12] = point3[0];
            buffer[b_ind+13] = point3[1];
            buffer[b_ind+14] = point3[2];

            buffer[b_ind+15] = point1[0];
            buffer[b_ind+16] = point1[1];
            buffer[b_ind+17] = point1[2];

            b_ind += 2 * 3 * 3;
        }
    }

    return buffer;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwPollEvents();
    glfwSetCursorPos(window, width/2, height/2);

    // Dark blue background
    glClearColor(0.2f, 0.0f, 0.2f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    // glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    GLuint EnemyID = LoadShaders( "EnemyVertexShader.vertexshader", "EnemyFragmentShader.fragmentshader" );
    GLuint EnemyMatrixID = glGetUniformLocation(EnemyID, "MVP");

    
    // Get a handle for our buffers
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(EnemyID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(EnemyID, "vertexUV");

    // Load the texture
    GLuint Texture = loadBMP_custom("./snickers-logo.bmp");
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(EnemyID, "myTextureSampler");
    

    GLuint FireballID = LoadShaders( "FireballVertexShader.vertexshader", "FireballFragmentShader.fragmentshader" );
    GLuint FireballMatrixID = glGetUniformLocation(FireballID, "MVP");

    GLuint FireballTexture  = loadBMP_custom("fire.bmp");

    GLuint FireballTextureID  = glGetUniformLocation(FireballID, "myTextureSampler");

    GLuint FireballViewMatrixID = glGetUniformLocation(FireballID, "V");
    GLuint FireballModelMatrixID = glGetUniformLocation(FireballID, "M");

    glUseProgram(FireballID);
    GLuint FireballLightID = glGetUniformLocation(FireballID, "LightPosition_worldspace");
    
    Floor floor;

    static const GLfloat enemyVertexData[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
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
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
        
        
        
        
    };

    static const GLfloat enemyColorData[] = {
        0.583f,  0.771f,  0.9f,
        0.609f,  0.115f,  0.9f,
        0.327f,  0.483f,  0.844f,

        0.822f,  0.569f,  0.9f,
        0.435f,  0.602f,  0.9f,
        0.310f,  0.747f,  0.9f,

        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.9f,

        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,

        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.9f,

        0.676f,  0.977f,  0.9f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,

        0.997f,  0.513f,  0.9f,
        0.945f,  0.719f,  0.9f,
        0.543f,  0.021f,  0.978f,

        0.279f,  0.317f,  0.9f,
        0.167f,  0.620f,  0.9f,
        0.347f,  0.857f,  0.8f,
        
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
    
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        
    };
    
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
    
    GLuint programIDGreen = LoadShaders( "FloorVertexShader.vertexshader", "GreenFragmentShader.fragmentshader" );
    GLuint vertexPosition_modelspaceIDGreen = glGetAttribLocation(programIDGreen, "vertexPosition_modelspaceGreen");
    GLuint MatrixID2 = glGetUniformLocation(programIDGreen, "MVP");
    
    int sphere1_buf_size;
    GLfloat * sphere1_buffer_data = getSphere(4, 4, 3, sphere1_buf_size);
    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphere1_buf_size, sphere1_buffer_data,  GL_STATIC_DRAW);


    GLuint enemyVertexBuffer;
    glGenBuffers(1, &enemyVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(enemyVertexData), enemyVertexData, GL_STATIC_DRAW);

    GLuint enemyColorBuffer;
    glGenBuffers(1, &enemyColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, enemyColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    
    

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("bullet.obj", vertices, uvs, normals);

    GLuint fireballVertexBuffer;
    glGenBuffers(1, &fireballVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, fireballVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint fireballuvBuffer;
    glGenBuffers(1, &fireballuvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, fireballuvBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    GLuint fireballNormalBuffer;
    glGenBuffers(1, &fireballNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, fireballNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    float renderDistance = 50.0f;

    std::vector<Enemy> enemies;
    float enemySpawnTime = 2.0f;
    int maxSize = 10;

    std::vector<Fireball> fireballs;
    float fireballSpeed = 15.0f;
    float cooldown = 0.5f;

    float beginTime = glfwGetTime();
    float lastSpawnTime = beginTime;
    float lastTime = beginTime;

    float lastFireballTime = beginTime;

    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // SPHERE ---------------------------------------------------------------------------------

        glUseProgram(programIDGreen);

        glm::mat4 ModelMatrix3 = glm::mat4(1.0);
        ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(-4.0f, 3.0f, -5.0f));
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
        
        floor.draw(MVP);
        
        
        

        // Enemies
        glUseProgram(EnemyID);
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (currentTime - lastSpawnTime > enemySpawnTime && enemies.size() < maxSize) {
            enemies.push_back(Enemy());
            lastSpawnTime = currentTime;
        }

        for (auto& enemy : enemies) {
            enemy.move(deltaTime);
            auto ModelMatrix = enemy.getModelMatrix();
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(EnemyMatrixID, 1, GL_FALSE, &MVP[0][0]);
            
            ///////
            
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture);
            // Set our "myTextureSampler" sampler to user Texture Unit 0
            glUniform1i(TextureID, 0);
            
            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(vertexPosition_modelspaceID);
            glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
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
            glBindBuffer(GL_ARRAY_BUFFER, enemyColorBuffer);
            glVertexAttribPointer(
                vertexUVID,                   // The attribute we want to configure
                2,                            // size : U+V => 2
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                0,                            // stride
                (void*)0                      // array buffer offset
            );
            
            
            ////////
//
//            glEnableVertexAttribArray(0);
//            glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
//            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//            glEnableVertexAttribArray(1);
//            glBindBuffer(GL_ARRAY_BUFFER, enemyColorBuffer);
//            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
            
            glDisableVertexAttribArray(vertexPosition_modelspaceID);
            glDisableVertexAttribArray(vertexUVID);
        }

        
        
        
        // Fireballs
        glUseProgram(FireballID);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetTime() - lastFireballTime > cooldown) {
            fireballs.push_back(Fireball());
            lastFireballTime = glfwGetTime();
        }
        
        for (auto& fireball: fireballs) {
            fireball.move(deltaTime);
            auto ModelMatrix = fireball.getModelMatrix();
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            glUniformMatrix4fv(FireballMatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(FireballModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(FireballViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

            glm::vec3 lightPos = glm::vec3(1,1,1);
            glUniform3f(FireballLightID, lightPos.x, lightPos.y, lightPos.z);

            glBindBuffer(GL_ARRAY_BUFFER, fireballVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, fireballuvBuffer);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, FireballTexture);
            glUniform1i(FireballTextureID, 0);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, fireballVertexBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, fireballuvBuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, fireballNormalBuffer);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }


        for (size_t i = 0; i < fireballs.size(); ++i) {
            if (glm::length(fireballs[i].position - getPosition()) > renderDistance) {
                fireballs.erase(fireballs.begin() + i);
            }
        }

        for (size_t i = 0; i < enemies.size(); ++i) {
            if (glm::length(enemies[i].position - getPosition()) > renderDistance) {
                enemies.erase(enemies.begin() + i);
            }
        }

        for (size_t i = 0; i < fireballs.size(); ++i) {
            for (size_t j = 0; j < enemies.size(); ++j) {
                if (areClose(fireballs[i], enemies[j])) {
                    std::cout << i << ' ' << j << std::endl;
                    enemies.erase(enemies.begin() + j);
                    fireballs.erase(fireballs.begin() + i);
                }
            }
        }

        glDisableVertexAttribArray(0);
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &enemyVertexBuffer);
    glDeleteBuffers(1, &fireballVertexBuffer);

    // glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(EnemyID);
    glDeleteProgram(FireballID);

    // glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
