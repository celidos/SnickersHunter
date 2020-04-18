#pragma once

#include <cstdio>
#include <cstdlib>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Player {
protected:

public:
    double lastTime = glfwGetTime();
    // Initial position : on +Z
    glm::vec3 position = glm::vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
// Initial vertical angle : none
    float verticalAngle = 0.0f;
// Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.004f;

    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    glm::vec3 direction;
    glm::vec3 moveDirection;

    glm::mat4 getViewMatrix(){
        return ViewMatrix;
    }
    glm::mat4 getProjectionMatrix(){
        return ProjectionMatrix;
    }
};

class Engine {
private:
    Player player_;
public:
    void computeMatricesFromInputs(GLFWwindow* window);
    glm::mat4 getViewMatrix() { return player_.getViewMatrix(); }
    glm::mat4 getProjectionMatrix() { return player_.getProjectionMatrix(); }
};
