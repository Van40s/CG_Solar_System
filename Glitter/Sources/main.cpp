// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

// Shader header
#include <shader.hpp>

//Camera header
#include <Camera.h>

//Model
#include <Model.h>

#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

//Camera setting
const std::string program_name = ("Camera");
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

static Camera camera(glm::vec3(400.0f, 0.0f, -100.0f));
static float lastX = 1200 / 2.0f;
static float lastY = 800 / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f; // time between current frame and last frame
static float lastFrame = 0.0f;

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

//    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    Mirage::Shader shaderProgram;
    // Build and compile shader program
    // Vertex shader
    shaderProgram.attach("shader.vert");

    // Fragment shader
    shaderProgram.attach("shader.frag");

    // Link and activate the shader program
    shaderProgram.link().activate();

    Model Sun (PROJECT_SOURCE_DIR "/Glitter/Models/sun/sun.obj");
    Model Mercury (PROJECT_SOURCE_DIR "/Glitter/Models/Mercury/mercury.obj");
    Model venus(PROJECT_SOURCE_DIR "/Glitter/Models/Venus/venus.obj");
    Model Earth(PROJECT_SOURCE_DIR "/Glitter/Models/Earth/earth.obj");
    Model Mars (PROJECT_SOURCE_DIR "/Glitter/Models/Mars/mars.obj");
    Model Jupiter (PROJECT_SOURCE_DIR "/Glitter/Models/Jupiter/jupiter.obj");
    Model Saturn (PROJECT_SOURCE_DIR "/Glitter/Models/Saturn/scene.gltf");
    Model Uranus (PROJECT_SOURCE_DIR "/Glitter/Models/Uranus/uranus.obj");
    Model Neptune (PROJECT_SOURCE_DIR "/Glitter/Models/Neptune/neptune.obj");


    float distanceFromSun = 120.0f;

    float rotationSpeedScale = 0.1f;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(mWindow);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shaderProgram.activate();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)1200 / (float)800, 0.1f, 4000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "projection"), 1, GL_FALSE,
                           &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "view"), 1, GL_FALSE,
                           &view[0][0]);

        // Calculate rotation angles for each planet based on time (currentFrame)
        float mercuryRotationAngle = glm::radians(currentFrame * 3.0083f * rotationSpeedScale);
        float venusRotationAngle = glm::radians(currentFrame * 1.8111f * rotationSpeedScale);
        float earthRotationAngle = glm::radians(currentFrame * 447.04f * rotationSpeedScale);
        float marsRotationAngle = glm::radians(currentFrame * 240.56f * rotationSpeedScale);
        float jupiterRotationAngle = glm::radians(currentFrame * 241.67f * rotationSpeedScale);
        float saturnRotationAngle = glm::radians(currentFrame * 284.72f * rotationSpeedScale);
        float uranusRotationAngle = glm::radians(currentFrame * 196.39f * rotationSpeedScale);
        float neptuneRotationAngle = glm::radians(currentFrame * 242.78f * rotationSpeedScale);


        // Position the Sun
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Sun.Draw(shaderProgram);

        //Mercury
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 40.0f, 0.0f, 0.0f));
        model = glm::rotate(model, mercuryRotationAngle, glm::vec3(0.0f, 0.1f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Mercury.Draw(shaderProgram);

        // Venus
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 80.0f, 0.0f, 0.0f));
        model = glm::rotate(model, venusRotationAngle, glm::vec3(0.0f, -0.1f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        venus.Draw(shaderProgram);

        // Earth
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(distanceFromSun + 120.0f, 0.0f, 0.0f));
        model = glm::rotate(model, earthRotationAngle, glm::vec3(0.0f, 1.0f, 0.1f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE,&model[0][0]);
        Earth.Draw(shaderProgram);

        //Mars
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 160.0f, 0.0f, 0.0f));
        model = glm::rotate(model, marsRotationAngle, glm::vec3(0.0f, 1.0f, 0.05f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Mars.Draw(shaderProgram);

        //Jupiter
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 400.0f, 0.0f, 0.0f));
        model = glm::rotate(model, jupiterRotationAngle, glm::vec3(0.0f, 1.0f, 0.08f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Jupiter.Draw(shaderProgram);

        //Saturn
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 700.0f, 0.0f, 0.0f));
        model = glm::rotate(model, saturnRotationAngle, glm::vec3(0.0f, 1.0f, 0.15f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Saturn.Draw(shaderProgram);

        //Uranus
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 900.0f, 0.0f, 0.0f));
        model = glm::rotate(model, uranusRotationAngle, glm::vec3(0.0f, 0.45f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Uranus.Draw(shaderProgram);

        //Neptune
        model = glm::mat4(1.0f); // Reset the model matrix for Venus
        model = glm::translate(model, glm::vec3(distanceFromSun + 1050.0f, 0.0f, 0.0f));
        model = glm::rotate(model, neptuneRotationAngle, glm::vec3(0.0f, 0.15f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));       // scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get(), "model"), 1, GL_FALSE, &model[0][0]);
        Neptune.Draw(shaderProgram);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glfwTerminate();
    return EXIT_SUCCESS;
}


// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}