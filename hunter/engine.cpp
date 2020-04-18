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
