#include "engine.hpp"

void Engine::computeMatricesFromInputs(GLFWwindow* window) {
    // glfwGetTime is called only once, the first time this function is called
//    player_.lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - player_.lastTime);
    printf("deltaTime = %f\n", deltaTime*1e+6);
    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Compute new orientation
    player_.horizontalAngle += player_.mouseSpeed * float(1024/2 - xpos );
    player_.verticalAngle   += player_.mouseSpeed * float( 768/2 - ypos );

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    player_.direction = glm::vec3(
        cos(player_.verticalAngle) * sin(player_.horizontalAngle),
        sin(player_.verticalAngle),
        cos(player_.verticalAngle) * cos(player_.horizontalAngle)
    );

    player_.moveDirection = glm::vec3(
        sin(player_.horizontalAngle),
        0.0,
        cos(player_.horizontalAngle)
    );

    // 2D moving only
//    glm::vec3 direction(
//        sin(player_.horizontalAngle),
//        0.0,
//        cos(player_.horizontalAngle)
//    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(player_.horizontalAngle - 3.141592f/2.0f),
        0,
        cos(player_.horizontalAngle - 3.141592f/2.0f)
    );

    printf("%.2f, %.2f\n", player_.direction[0], player_.direction[1]);

    // Up vector
    glm::vec3 up = glm::cross( right, player_.direction );

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        player_.position += player_.moveDirection * deltaTime * player_.speed;
        printf("added %f, %f, %f", (deltaTime ),
               (player_.direction * deltaTime * player_.speed)[1], (player_.direction * deltaTime * player_.speed)[2] );
//        printf("lol kek \n");
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        player_.position -= player_.moveDirection * deltaTime * player_.speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
        player_.position += right * deltaTime * player_.speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
        player_.position -= right * deltaTime * player_.speed;
    }

    printf("playerpos %f %f %f \n", player_.position[0], player_.position[1], player_.position[2]);

    float FoV = player_.initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    player_.ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    player_.ViewMatrix       = glm::lookAt(
        player_.position,           // Camera is here
        player_.position + player_.direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    player_.lastTime = currentTime;
}


////////////////////////

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
        speed = 0.1f;
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

    // Create and compile our GLSL program from the shaders
    GLuint FloorID = LoadShaders( "FloorVertexShader.vertexshader", "FloorFragmentShader.fragmentshader" );
    GLuint FloorMatrixID = glGetUniformLocation(FloorID, "MVP");

    GLuint EnemyID = LoadShaders( "EnemyVertexShader.vertexshader", "EnemyFragmentShader.fragmentshader" );
    GLuint EnemyMatrixID = glGetUniformLocation(EnemyID, "MVP");

    GLuint FireballID = LoadShaders( "FireballVertexShader.vertexshader", "FireballFragmentShader.fragmentshader" );
    GLuint FireballMatrixID = glGetUniformLocation(FireballID, "MVP");

    GLuint FireballTexture  = loadBMP_custom("fire.bmp");

    GLuint FireballTextureID  = glGetUniformLocation(FireballID, "myTextureSampler");

    GLuint FireballViewMatrixID = glGetUniformLocation(FireballID, "V");
    GLuint FireballModelMatrixID = glGetUniformLocation(FireballID, "M");

    glUseProgram(FireballID);
    GLuint FireballLightID = glGetUniformLocation(FireballID, "LightPosition_worldspace");


    static const GLfloat floorVertexData[] = {
            -100.0f, 0.0f,-100.0f,
            -100.0f, 0.0f, 100.0f,
            100.0f, 0.0f, 100.0f,

            100.0f, 0.0f, 100.0f,
            100.0f, 0.0f,-100.0f,
            -100.0f, 0.0f,-100.0f,
    };

    GLuint floorVertexBuffer;
    glGenBuffers(1, &floorVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertexData), floorVertexData, GL_STATIC_DRAW);

//    GLuint uvbuffer;
//    glGenBuffers(1, &uvbuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    static const GLfloat enemyVertexData[] = {
            0.0f, 2.0f, 0.0f,
            -1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,

            0.0f, 2.0f, 0.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f,-1.0f,

            0.0f, 2.0f, 0.0f,
            1.0f, 0.0f,-1.0f,
            -1.0f, 0.0f,-1.0f,

            0.0f, 2.0f, 0.0f,
            -1.0f, 0.0f,-1.0f,
            -1.0f, 0.0f, 1.0f,

            0.0f,-2.0f, 0.0f,
            1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, 1.0f,

            0.0f,-2.0f, 0.0f,
            1.0f, 0.0f,-1.0f,
            1.0f, 0.0f, 1.0f,

            0.0f,-2.0f, 0.0f,
            -1.0f, 0.0f,-1.0f,
            1.0f, 0.0f,-1.0f,

            0.0f,-2.0f, 0.0f,
            -1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f,-1.0f,
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
    };


    GLuint enemyVertexBuffer;
    glGenBuffers(1, &enemyVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(enemyVertexData), enemyVertexData, GL_STATIC_DRAW);

    GLuint enemyColorBuffer;
    glGenBuffers(1, &enemyColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, enemyColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(enemyColorData), enemyColorData, GL_STATIC_DRAW);

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

        // Floor
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

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 2*3);

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
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, enemyColorBuffer);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 8 * 3);
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
    glDeleteBuffers(1, &floorVertexBuffer);
    glDeleteBuffers(1, &enemyVertexBuffer);
    glDeleteBuffers(1, &fireballVertexBuffer);

    // glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(FloorID);
    glDeleteProgram(EnemyID);
    glDeleteProgram(FireballID);

    // glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
