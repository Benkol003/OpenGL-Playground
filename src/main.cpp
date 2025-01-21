#include <iostream>
#include <exception>
#include <string>
#include <tuple>
#include <functional>

#ifdef __EMSCRIPTEN__
#define IMGUI_IMPL_OPENGL_ES3
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/gl.h>
#endif

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define GLM_FORCE_INTRINSICS // still need to check if intrinsics are being used
// TODO add SSE/AVX to cmake
#include "glm/vec3.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "procedural_meshes.hpp"
#include "shader.hpp"
#include "raytracer.hpp"
#include "control.hpp"

const int WINDOW_SIZE[2] = {900, 900};

// main function for emscripten hack
#ifdef __EMSCRIPTEN__
std::function<void()> main_loop_ptr;
void main_loop()
{
    main_loop_ptr();
}
#endif

int main(int argc, char **argv)
{
    try
    {
        // initialise GLFW and window
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialise GLFW");
        }

        // window setup
        // glfwWindowHint(GLFW_RESIZABLE,false);
        //using static variables for emscripten so variables dont go off the stack when main exits but the lambda loop is still running
        // in the future move this stuff into a class / function
        static GLFWwindow *root = glfwCreateWindow(WINDOW_SIZE[0], WINDOW_SIZE[1], "OpenGL", NULL, NULL);
        if (!root)
            throw std::runtime_error("Failed to initialise GLFW window");

        glfwMakeContextCurrent(root);
        glfwSwapInterval(0); // disable vsync
        glfwSetKeyCallback(root, keys_callback);
        glfwSetMouseButtonCallback(root, mouse::button_callback);
        glfwSetCursorPosCallback(root, mouse::pos_callback);
        glfwSetScrollCallback(root, mouse::scroll_callback);

#ifndef __EMSCRIPTEN__
        // initialise GLAD/GL
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            throw std::runtime_error("Failed to initialise GLAD");
        }
        printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
#endif

        glViewport(0, 0, WINDOW_SIZE[0], WINDOW_SIZE[1]);
        // glfwSetFramebufferSizeCallback(root, resize_callback);
        // glfwSetInputMode(root,GLFW_STICKY_KEYS,true);

        // imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(root, true);

#ifdef __EMSCRIPTEN__
        ImGui::GetIO().IniFilename = nullptr; // no ile access in emscripten atm
        ImGui_ImplGlfw_InstallEmscriptenCallbacks(root, "#canvas");

        emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_TRUE, key_callback_emscripten);
        emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_TRUE, key_callback_emscripten);

#endif
        ImGui_ImplOpenGL3_Init();

        // imgui out params
        static int divisionFactor = 10;
        static float heightRatio = 1;
        static const char *items[3] = {"Sphere", "Cylinder", "Circle"};
        static int selected = 0;
        static bool anySelected = false;

        static auto data = sphere(divisionFactor);
        static auto data_wireframe = toWireframe(data);

        static auto renderer = Renderer(data_wireframe.vertexes, data_wireframe.indexes);
        shader::init();

        // main loop
        static double timeCurrent = glfwGetTime();
        static double timeOld;

        static bool wireframe = true;

        static float dt_acc = 0;
        static int fps = 0;

        static bool firstFrame = true; //used for init UI placement

#ifdef __EMSCRIPTEN__
        main_loop_ptr = [&]()
        {
#else
        while (!glfwWindowShouldClose(root))
        {
#endif
            // dt and fps counter
            timeOld = timeCurrent;
            timeCurrent = glfwGetTime();
            float dt = timeCurrent - timeOld;

            // make the fps counter readable
            dt_acc += dt;
            if (dt_acc > 0.125)
            {
                fps = 1 / dt;
                dt_acc = 0;
            }

            glfwPollEvents();
            // build immediate mode ui
            // TODO window doesnt automatically resize to fit widgets
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("help",NULL,ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::TextColored({1,1,1,1},"Controls:\n"
            "AWSD keys or mouse right click hold + move mouse: object rotation\n"
            "arrow keys or mouse left click hold + move mouse: object x/y translation\n"
            "+- keys or mouse scroll wheel: object distance / z translation\n"
            "Enter key: reset object position and rotation\n"
            );

            if(firstFrame){
            ImGui::SetWindowPos({0,0},true);
            //next window aligned underneath
            ImVec2 currentWindowSize = ImGui::GetWindowSize();
            ImGui::SetNextWindowPos({0,currentWindowSize.y+70},true); //TODO why isnt it perfectly aligned under with +0
            //TODO the imgui ini file would be better but we cant load it atm in emscripten
            firstFrame = false;
            }
            ImGui::End();


            ImGui::Begin("object controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextColored({1, 1, 0, 1}, "%d FPS", fps);
            ImGui::TextColored({0, 1, 0, 1}, "Triangle Count: %d", data.indexes.size() / 3);
            anySelected = false;
            if (ImGui::BeginCombo("Select Mesh", items[selected], 0))
            {
                for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
                {
                    bool onSelect = false;
                    if (ImGui::Selectable(items[i], &onSelect))
                        selected = i;
                    anySelected |= onSelect;
                    if (selected == 2 && divisionFactor > 16)
                        divisionFactor = 16; // prevent overflow in circle generator
                }
                ImGui::EndCombo();
            }

            if (anySelected || ImGui::SliderInt("triangle\ndivision\nfactor", &divisionFactor, selected == 1 ? 3 : 1, selected == 2 ? 16 : 256) || // regular prism can have triangle as smallest cap - division factor determines n-polyhedron of cap. Also circle depth increases triangles exponentially
                (selected == 1 && ImGui::SliderFloat("Height Ratio", &heightRatio, 0.1, 100)))
            {
                switch (selected)
                {
                case 0:
                    data = sphere(divisionFactor);
                    break;

                case 1:
                    if(divisionFactor<3) divisionFactor = 3;
                    data = regularPrism(divisionFactor, heightRatio);
                    break;

                case 2:
                    data = circle(divisionFactor);
                }
                data_wireframe = toWireframe(data);
                auto useData = wireframe ? data_wireframe : data;
                renderer.updateData(useData.vertexes, useData.indexes);
            }
            if (ImGui::Selectable("wireframe mode", &wireframe))
            {
                auto useData = wireframe ? data_wireframe : data;
                renderer.updateData(useData.vertexes, useData.indexes);
            }
            ImGui::End();

            // camera transform update
            glm::mat4 cameraMat;
            if (cameraRotationAxis != glm::vec3(0))
                cameraRotation = glm::rotate(glm::mat4(1), glm::radians(90 * dt), cameraRotationAxis) * cameraRotation;
            cameraTranslation = glm::translate(cameraTranslation, cameraTranslateDirection * 1.f * dt);

            glUniformMatrix4fv(shader::u_transform, 1, false, glm::value_ptr(cameraProjection * cameraTranslation * cameraRotation));

            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES,0,3);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(root);
        };

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop, 0, false);
        // should switch to use request animation loop instead
#endif

#ifndef __EMSCRIPTEN__
        // TODO figure out how to clean up in emscripten
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
#endif
        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception in main: " << typeid(e).name() << ": " << e.what() << '\n';
        glfwTerminate();
    }
}