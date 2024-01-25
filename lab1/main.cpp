#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>


GLint Ix;


//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window;
GLuint width = 1000, height = 1000;
glm::mat4 projectionMatrix = glm::identity<glm::mat4>();
std::vector<glm::vec3> scaledVertices;
std::vector<glm::vec3> faces;
std::vector<glm::vec3> curve_points;
std::vector<Utils::planeCoefficients> planesCoefficients;
std::vector<glm::vec3> bSplineCurvePoints;
std::vector<glm::vec3> bSplineCurvePTagents;
glm::vec3 objectOrientation = glm::vec3(1.f, 0.f, 0.f);

std::string filename = "resources/aircraft747.obj";
std::string curve_points_file_path = "resources/curve_points.txt";
int currentPoint = 0;
int precision = 100;
//std::vector<glm::vec3> points = {};
//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay();
void myReshape(int width, int height);
void myMouse(int button, int state, int x, int y);
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);

void myPoint(float x, float y, float z, bool isGreen);


void myCurve();
void drawTangent(glm::vec3 point, glm::vec3 tangent);

int main(int argc, char ** argv) {

    std::vector<glm::vec3> vertices;

    Utils::readVerticesAndFaces(filename, vertices, faces);
    curve_points = Utils::readCurvePointsFromFile(curve_points_file_path);


    bSplineCurvePoints = Utils::getCubeBSplineCurve(curve_points, precision);
    bSplineCurvePTagents = Utils::getCubeBSplineCurveTangent(curve_points, precision);
    //objectOrientation = glm::normalize(bSplineCurvePoints[1] - bSplineCurvePoints[0]);

    std::cout << "BSpline curve points: " << bSplineCurvePoints.size() << std::endl;
    std::cout << "BSpline tangent size: " << bSplineCurvePTagents.size() << std::endl;

    scaledVertices = Utils::scaleToOne(vertices);

    //rotationMatrix = glm::transpose(rotationMatrix);
    glm::mat4 scaleMatrix = glm::mat4(0.3f);
    objectOrientation = bSplineCurvePTagents[currentPoint];
    std::vector<glm::vec3> scaledVerticies;
    for (glm::vec3 v : scaledVertices){
        glm::vec4 v4 = glm::vec4(v, 1);
        scaledVerticies.push_back(  v4 * scaleMatrix  );
    }
    scaledVertices = scaledVerticies;

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInit(&argc, argv);

    window = glutCreateWindow("Glut OpenGL Polygon");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);
    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);


    glutMainLoop();
    return 0;
}
void myPolygon()
{

    glm::vec3 center = Utils::getCenter(scaledVertices);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for(int i=0; i< faces.size(); i++){
        glBegin(GL_POLYGON);

        glColor4f( (scaledVertices[faces[i].x-1].z + 1)*0.5f, 0.0f, 0.f, 0.0f);
        glVertex3f(scaledVertices[faces[i].x-1].x , scaledVertices[faces[i].x -1 ].y , scaledVertices[faces[i].x - 1].z);
        //glVertex3f(scaledVertices[faces[i].x-1].x , scaledVertices[faces[i].x -1 ].y , 0.f);


        glColor4f((scaledVertices[faces[i].y-1].z + 1)*0.5f,0.f,  0.f, 0.0f);
        glVertex3f(scaledVertices[faces[i].y-1].x, scaledVertices[faces[i].y -1].y , scaledVertices[faces[i].y -1].z);
        //glVertex3f(scaledVertices[faces[i].y-1].x, scaledVertices[faces[i].y -1].y ,0.f);

        glColor4f((scaledVertices[faces[i].z-1].z + 1)*0.5f, 0.f, 0.f, 0.0f);
        glVertex3f(scaledVertices[faces[i].z -1 ].x , scaledVertices[faces[i].z -1 ].y , scaledVertices[faces[i].z -1].z);
        //glVertex3f(scaledVertices[faces[i].z -1 ].x , scaledVertices[faces[i].z -1 ].y ,0.0f);

        glEnd();
    }


}


void myDisplay()
{
   myPolygon();

    myCurve();

    drawTangent(bSplineCurvePoints[currentPoint], bSplineCurvePTagents[currentPoint]);

    glFlush();
}

void myCurve() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_POINTS);


    for (int i=0; i< curve_points.size(); i++){

        glColor4f(0.0f, 0.0f, 0.f, 0.0f);
        glVertex3f(curve_points[i].x , curve_points[i].y , curve_points[i].z);
    }
    float color = 0.3f;
    for (int i=0; i< bSplineCurvePoints.size(); i++){
        if (i % precision == 0){
            glColor4f(color, color, color, color);
            color += 0.1f;
        }


        glVertex3f(bSplineCurvePoints[i].x , bSplineCurvePoints[i].y , bSplineCurvePoints[i].z);
    }
    glEnd();

}

void myMouse(int button, int state, int x, int y){

}


void drawTangent(glm::vec3 point, glm::vec3 tangent) {
    // Define the length of the tangent line
    float tangentLength = 5.0f;

    // Create two points to define the tangent line
    glm::vec3 start = point;
    glm::vec3 end = point + tangent * tangentLength;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_LINES);

    glColor4f(0.0f, 0.0f, 0.f, 0.0f);

    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);

    glEnd();


}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int w, int h) {
    width = w;
    height = h;

    //promjena sirine i visine prozora
    Ix = 0;
    float aspectRatio = (float)w / (float)h;
//	indeks tocke 0-prva 1-druga tocka
    glViewport(0, 0, w, h);    //  otvor u prozoru
    glMatrixMode(GL_PROJECTION);        //	matrica projekcije
    glLoadMatrixf(&projectionMatrix[0][0]);

    //glLoadIdentity();  //	jedinicna matrica
    glOrtho(-1, 1, -1, 1, -1 , 1);    //	okomita projekcija
    glMatrixMode(GL_MODELVIEW);            //	matrica pogleda


   // glLoadMatrixf(&projectionMatrix[0][0]);
    glLoadIdentity();
    glClearColor(0.5f, 1.0f, 1.0f, 0.0f); // boja pozadine
    glClear(GL_COLOR_BUFFER_BIT);        //	brisanje pozadine
    myDisplay();
}

//void myPoint(float x, float y, float z, bool isGreen = false) {
//
//    // Set the modelview matrix to move the sphere to the custom location
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(x, y, z);
//
//    // Draw the sphere
//
//    if (isGreen) {
//        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
//    }
//    else{
//        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
//    }
//
//    glutSolidSphere(0.01, 50, 50);
//
//}

//*********************************************************************************
//	Mis.
//*********************************************************************************





void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    float rotation_speed = 5.f;
    switch (theKey)
    {
        case 'w':
            // rotation up (x axis)
            projectionMatrix = glm::rotate(projectionMatrix, glm::radians(rotation_speed), glm::vec3(1.0f, 0.0f, 0.0f));
            break;

        case 's':

            // rotation down (x axis)
            projectionMatrix = glm::rotate(projectionMatrix, glm::radians(-rotation_speed), glm::vec3(1.0f, 0.0f, 0.0f));
            break;

        case 'a':
            // rotation left (y axis)
            projectionMatrix = glm::rotate(projectionMatrix, glm::radians(rotation_speed), glm::vec3(0.0f, 1.0f, 0.0f));
            break;

        case 'd':
            // rotation right (y axis)
            projectionMatrix = glm::rotate(projectionMatrix, glm::radians(-rotation_speed), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case 't':
            std::cout << "Current point: "<< currentPoint << std::endl;
            glm::vec3 center = Utils::getCenter(scaledVertices);
            glm::vec3 centerToPoint = bSplineCurvePoints[currentPoint] - center;
            currentPoint = (currentPoint + 1) % bSplineCurvePoints.size();
            glm::mat4 translationMatrix = glm::mat4( 1.f, 0.f, 0.f,centerToPoint.x,
                                            0.f, 1.f, 0.f, centerToPoint.y,
                                            0.f, 0.f, 1.f, centerToPoint.z,
                                            0.f, 0.f, 0.f, 1.f);
            std::vector<glm::vec3> translatedVerticies;
            auto tagent = bSplineCurvePTagents[currentPoint];
//            auto tmp = tagent.y;
//            tagent.y = tagent.z;
//            tagent.z = tmp;
            auto rotationMatrix = Utils::createRotationMatrix(objectOrientation,tagent);
            //rotationMatrix = glm::transpose(rotationMatrix);
            objectOrientation = bSplineCurvePTagents[currentPoint];
            for (glm::vec3 v : scaledVertices){
                glm::vec4 v4 = glm::vec4(v, 1);
                translatedVerticies.push_back(v4 * rotationMatrix * translationMatrix );
            }
            scaledVertices = translatedVerticies;

            // rotate object


    }
    myReshape(width, height);
    glFlush();
}
