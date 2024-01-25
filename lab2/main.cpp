#include <iostream>
#include <vector>
#include "utils.h"
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <IL/il.h>

#include "particle_system.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_glut.h"
#include "imgui/backends/imgui_impl_opengl2.h"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************
glm::mat4 projectionMatrix = glm::identity<glm::mat4>();
// Camera parameters
float cameraPosX = 0.0f;
float cameraPosY = 0.0f;
float cameraPosZ = 0.0f;


float cameraSpeed = 0.01f;


GLuint window;
GLuint width = 1000, height = 1000;


ParticleSystem::ParticleSystem particleSystem({0, 0, 0}, 0.5f, 5, 1, 9.81f);

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay();
void myReshape(int width, int height);
void myMouse(int button, int state, int x, int y);
void keyboardCallback(unsigned char key, int x, int y);
void MainLoopStep();

//void create_particles(int i);

//void recreateDeadParticles();

//struct Particle {
//    float x;
//    float y;
//    float zoom;
//    float life; // 0 means just created, 1 means dead
//    // Add more attributes if needed
//};

//std::vector<Particle> particles;  // Collection to store multiple particles



void initIL() {
    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
}

void enableTransparency() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool hasAlphaChannel(ILuint imageID) {
    ilBindImage(imageID);
    return ilGetInteger(IL_IMAGE_CHANNELS) == 4;  // Check if there are 4 channels (RGBA)
}

void applyColorFilter(ILubyte* imageData, ILint width, ILint height, ILint channels, float redMultiplier, float greenMultiplier, float blueMultiplier) {
    for (ILint y = 0; y < height; ++y) {
        for (ILint x = 0; x < width; ++x) {
            ILubyte* pixel = imageData + (y * width + x) * channels;

            // Apply the color filter to each channel
            pixel[0] = static_cast<ILubyte>(pixel[0] * redMultiplier);     // Red channel
            pixel[1] = static_cast<ILubyte>(pixel[1] * greenMultiplier);   // Green channel
            pixel[2] = static_cast<ILubyte>(pixel[2] * blueMultiplier);    // Blue channel
        }
    }
}

glm::vec3 lerp(const glm::vec3& startColor, const glm::vec3& endColor, float t) {
    // Clamp t to the range [0, 1]
    t = std::fmax(0.0f, std::fmin(1.0f, t));

    // Linear interpolation formula
    return glm::vec3(
            (1 - t) * endColor.r + t * startColor.r,
            (1 - t) * endColor.g + t * startColor.g,
            (1 - t) * endColor.b + t * startColor.b
    );
}

void display() {
    //glClear(GL_COLOR_BUFFER_BIT);
    enableTransparency();

        // Load BMP imagem
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);
    ILboolean success = ilLoadImage("resources/iskrica.tga");

    if (success == IL_TRUE) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    } else {
        fprintf(stderr, "Failed to load the image\n");
    }

    for (const auto& particle : particleSystem.particles) {
        if (particle.life > 1.0f) {
            std::cout << "Skipping dead particle" << std::endl;
            continue;  // Skip dead particles
        }


            int width = ilGetInteger(IL_IMAGE_WIDTH);
            int height = ilGetInteger(IL_IMAGE_HEIGHT);
             ILint channels = ilGetInteger(IL_IMAGE_CHANNELS);

//            glm::vec3 currentColor = lerp(particle.startColor, particle.endColor, particle.life);
//            std::cout << currentColor.x << " " << currentColor.y << " " << currentColor.z << std::endl;
        // Get image information
            unsigned char* imageData = ilGetData();
//            applyColorFilter(imageData, width, height, channels, currentColor.r, currentColor.g, currentColor.b);


        // Set up OpenGL for rendering each particle
            glRasterPos3f(particle.position.x, particle.position.y, particle.position.z);
            glPixelZoom(particle.zoom, particle.zoom);
            // Set up OpenGL for rendering
            glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);


    }
    ilDeleteImages(1, &imageID);



}


void timer(int value) {
    int delta_time_ms = 16;
    float fps = 1000.f / delta_time_ms;
    particleSystem.update_particles(delta_time_ms / 1000.f);
    particleSystem.recreateDeadParticles();
    glutPostRedisplay();
    glutTimerFunc(delta_time_ms, timer, 0);  // Set the timer to redraw every 16 milliseconds (60 FPS)
}

int main(int argc, char ** argv) {

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInit(&argc, argv);
    initIL();

    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Enable alpha blending for transparency
    enableTransparency();

    window = glutCreateWindow("Glut OpenGL Polygon");
    glutReshapeFunc(myReshape);
    // Display the image
    glutDisplayFunc(MainLoopStep);
    //glutMouseFunc(myMouse);
//    glutKeyboardFunc(keyboardCallback);

    // Initialize particle system
//    particleSystem.create_particles(100);
    particleSystem.startColor = {1.f, 1.f, 1.f};
    particleSystem.endColor = {0.f, 0.f, 0.f};


    glutTimerFunc(0, timer, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();

    // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    ImGui_ImplGLUT_InstallFuncs();


    glutMainLoop();
    return 0;
}



void myDisplay()
{


    display();
    //glFlush();

}




//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int w, int h) {

    width = w;
    height = h;
    //promjena sirine i visine prozora
    float aspectRatio = (float)w / (float)h;
//	indeks tocke 0-prva 1-druga tocka
//    glViewport(0, 0, w, h);    //  otvor u prozoru
    glMatrixMode(GL_PROJECTION);        //	matrica projekcije
//
//    glLoadMatrixf(&projectionMatrix[0][0]);

    //glLoadIdentity();  //	jedinicna matrica
    glOrtho(-1, 1, -1, 1, -1 , 1);    //	okomita projekcija
    glMatrixMode(GL_MODELVIEW);            //	matrica pogleda


   // glLoadMatrixf(&projectionMatrix[0][0]);
//    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(cameraPosX, cameraPosY, cameraPosZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // boja pozadine
    glClear(GL_COLOR_BUFFER_BIT);        //	brisanje pozadine
    myDisplay();
}



static ImVec4 startColor = ImVec4(1.0f, 1.f, 1.f, 1.00f);
static ImVec4 endColor = ImVec4(0.f, 0.f, 0.f, 1.00f);
static float particleSize = 1.0f;


void MainLoopStep()

{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();




    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static int particleCount = 10;
        static int counter = 0;

        ImGui::Begin("Particle system!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Number of particles to add from current source.");               // Display some text (you can use a format strings too)

        ImGui::SliderInt("particle to add", &particleCount, 0, 100);            // Edit 1 float using a slider from 0.0f to 1.0f
        if(ImGui::ColorEdit3("Start color", (float*)&startColor)){
            particleSystem.startColor = {startColor.x, startColor.y, startColor.z};
        }// Edit 3 floats representing a color
        if(ImGui::ColorEdit3("End color", (float*)&endColor)){
            particleSystem.endColor = {endColor.x, endColor.y, endColor.z};

        }// Edit 3 floats representing a color

        if(ImGui::SliderFloat("Particles size, ", &particleSize, 0.05f, 2.5f)){
            particleSystem.particleSize = particleSize;
        }

        if (ImGui::Button("Add particles"))   {
            counter += particleCount;
            particleSystem.create_particles(particleCount);

        }                         // Buttons return true when clicked (most widgets return true when edited/activated)
        if(ImGui::IsMouseClicked(1)){
            particleSystem.center = {io.MousePos.x/ (float)width * 2.0f - 1.0f, -io.MousePos.y/ (float)height * 2.0f + 1.0f, 0};
        }
        if(ImGui::IsKeyPressed(ImGuiKey_W, false) ){
            cameraPosZ -= cameraSpeed;

        }
        if(ImGui::IsKeyPressed(ImGuiKey_A, false) ){

            cameraPosX -= cameraSpeed;


        }
        if(ImGui::IsKeyPressed(ImGuiKey_S, false) ){
            cameraPosZ += cameraSpeed;
        }
        if(ImGui::IsKeyPressed(ImGuiKey_D, false) ){

            cameraPosX += cameraSpeed;

        }


        glOrtho(-1, 1, -1, 1, -5 , 5);    //	okomita projekcija


        glMatrixMode(GL_MODELVIEW);        //	matrica projekcije
        glm::mat4 viewMatrix = glm::lookAt(glm::vec3(cameraPosX, cameraPosY, cameraPosZ), glm::vec3(cameraPosX, cameraPosY, cameraPosZ-5.f), glm::vec3(0, 1, 0));

//        glLoadIdentity();  //	jedinicna matrica
        glLoadMatrixf(&viewMatrix[0][0]);
        ImGui::SameLine();
        ImGui::Text("Particle Count = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }


    // Rendering
    ImGui::Render();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    myDisplay();


    glutSwapBuffers();
    glutPostRedisplay();

}


