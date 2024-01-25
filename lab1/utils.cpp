//
// Created by mario on 4/2/23.
//
#include <sstream>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include "utils.h"

namespace  Utils {
    glm::vec3 readVec3FromLine(std::string line) {
        std::stringstream ss(line);
        std::string dummy;
        glm::vec3 vec;
        ss >> dummy >> vec.x >> vec.y >> vec.z;
        return vec;
    }

    minMax getMinMax(std::vector<glm::vec3> vertices){
        minMax minMax;
        minMax.XMin = vertices[0].x;
        minMax.XMax = vertices[0].x;
        minMax.YMin = vertices[0].y;
        minMax.YMax = vertices[0].y;
        minMax.ZMin = vertices[0].z;
        minMax.ZMax = vertices[0].z;
        for (auto &v : vertices) {
            if (v.x < minMax.XMin) {
                minMax.XMin = v.x;
            }
            if (v.x > minMax.XMax) {
                minMax.XMax = v.x;
            }
            if (v.y < minMax.YMin) {
                minMax.YMin = v.y;
            }
            if (v.y > minMax.YMax) {
                minMax.YMax = v.y;
            }
            if (v.z < minMax.ZMin) {
                minMax.ZMin = v.z;
            }
            if (v.z > minMax.ZMax) {
                minMax.ZMax = v.z;
            }
        }
        return minMax;

    }

    glm::vec3 getCenter(std::vector<glm::vec3> vertices){
        glm::vec3 center;
        minMax minMax = getMinMax(vertices);
        center.x = (minMax.XMax + minMax.XMin) / 2;
        center.y = (minMax.YMax + minMax.YMin) / 2;
        center.z = (minMax.ZMax + minMax.ZMin) / 2;
        return center;
    }

    std::vector<glm::vec3> scaleToOne(std::vector<glm::vec3> vertices){
        minMax minMax = getMinMax(vertices);
        float max = minMax.XMax - minMax.XMin;
        if (minMax.YMax - minMax.YMin > max) {
            max = minMax.YMax - minMax.YMin;
        }
        if (minMax.ZMax - minMax.ZMin > max) {
            max = minMax.ZMax - minMax.ZMin;
        }
        for (auto &v : vertices) {
            v.x = v.x / max;
            v.y = v.y / max;
            v.z = v.z / max;
        }

        // move to center
        glm::vec3 center = getCenter(vertices);
        for (auto &v : vertices) {
            v.x = v.x - center.x;
            v.y = v.y - center.y;
            v.z = v.z - center.z;
        }
        return vertices;
    }

    void readVerticesAndFaces(std::string filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &faces){
        std::ifstream file(filename);

        int vertexCount = 0;
        int faceCount = 0;

        if (file.is_open()) {
            std::string line;
            // Count vertices and faces
            while (getline(file, line)) {
                if(line[0] == 'v' && line[1] == ' ') {
                    vertexCount++;
                } else if(line[0] == 'f' && line[1] == ' '){
                    faceCount++;
                }
            }
            std::cout << "Vertices: " << vertexCount << std::endl;
            std::cout << "Faces: " << faceCount << std::endl;
            std::cout << "Allocating memory" << std::endl;

            vertices.reserve(vertexCount);
            faces.reserve(faceCount);

            std::cout << "Reading data" << std::endl;
            file.clear();
            file.seekg(0, std::ios::beg);

            while (getline(file, line)) {
                if(line[0] == 'v' && line[1] == ' ') {
                    vertices.push_back(Utils::readVec3FromLine(line));
                } else if(line[0] == 'f' && line[1] == ' '){
                    faces.push_back(Utils::readVec3FromLine(line));
                }
            }
            file.close();
        } else {
            std::cout << "Unable to open file" << std::endl;
        }
    }

    std::vector<glm::vec3> readCurvePointsFromFile(std::string file_path){
        std::ifstream file(file_path);
        std::vector<glm::vec3> curvePoints;
        int high, low = 0;
        if (file.is_open()) {
            std::string line;
            std::cout << "Reading data" << std::endl;
            getline(file, line);
            if (high == 0 && low == 0){
                std::stringstream ss(line);
                std::string dummy;

                ss >> dummy >> high >> low;
            }
            while (getline(file, line)) {
                curvePoints.push_back(Utils::readVec3FromLine(line)  /((float)high - (float)low));
            }
            file.close();
        } else {
            std::cout << "Unable to open file" << std::endl;
        }
        return curvePoints;
    }
    glm::vec3 getCubeBSplineCurvePoint(std::vector<glm::vec3> controlPoints, float t, int segmentNumber){


        glm::mat4 basisMatrix(-1, 3, -3, 1,
                              3, -6, 0, 4,
                              -3, 3, 3, 1,
                              1, 0, 0, 0);

        // Create a column vector with the parameter values (t^3, t^2, t, 1)
        glm::vec4 tVector(t * t * t, t * t, t, 1);

        // Create a matrix with the control point vectors as columns
        glm::mat4 controlPointsMatrix(controlPoints[segmentNumber].x, controlPoints[segmentNumber+1].x, controlPoints[segmentNumber+2].x, controlPoints[segmentNumber+3].x,
                                      controlPoints[segmentNumber].y, controlPoints[segmentNumber+1].y, controlPoints[segmentNumber+2].y, controlPoints[segmentNumber+3].y,
                                      controlPoints[segmentNumber].z, controlPoints[segmentNumber+1].z, controlPoints[segmentNumber+2].z, controlPoints[segmentNumber+3].z,
                                      1, 1, 1, 1);

        // Calculate the resulting vector by multiplying the basis matrix and control points matrix
        glm::vec4 resultVector = tVector *(1/6.f) *  basisMatrix * controlPointsMatrix;

        // The B-spline curve point is the resulting 3D vector
        glm::vec3 splinePoint(resultVector.x, resultVector.y, resultVector.z);
        return splinePoint;

    }

    glm::vec3 getCubeBSplineCurvePointTagent(std::vector<glm::vec3> controlPoints, float t, int segmentNumber){


        glm::mat4x3 basisMatrix(-1, 2, -1,
                              3, -4, 0,
                              -3, 2, 1,
                              1, 0, 0);

        // Create a column vector with the parameter values (t^3, t^2, t, 1)
        glm::vec3 tVector( t * t, t, 1);

        // Create a matrix with the control point vectors as columns
        glm::mat4 controlPointsMatrix(controlPoints[segmentNumber].x, controlPoints[segmentNumber+1].x, controlPoints[segmentNumber+2].x, controlPoints[segmentNumber+3].x,
                                      controlPoints[segmentNumber].y, controlPoints[segmentNumber+1].y, controlPoints[segmentNumber+2].y, controlPoints[segmentNumber+3].y,
                                      controlPoints[segmentNumber].z, controlPoints[segmentNumber+1].z, controlPoints[segmentNumber+2].z, controlPoints[segmentNumber+3].z,
                                      1, 1, 1, 1);

        // Calculate the resulting vector by multiplying the basis matrix and control points matrix
        glm::vec4 resultVector = tVector * (1/6.f) *  basisMatrix * controlPointsMatrix;

        // The B-spline curve point is the resulting 3D vector
        glm::vec3 splinePoint(resultVector.x, resultVector.y, resultVector.z);
        return splinePoint;

    }

    std::vector<glm::vec3> getCubeBSplineCurve(std::vector<glm::vec3> controlPoints, int precision){
        std::vector<glm::vec3> curvePoints;
        float t = 0;
        for (int j = 0; j < controlPoints.size() - 3 ; j++) {
            t = 0;
            for (int i = 0; i < precision; i++) {
                curvePoints.push_back(Utils::getCubeBSplineCurvePoint(controlPoints, t, j));
                t += 1.0f / precision;
            }
        }
        return curvePoints;

    }

    std::vector<glm::vec3> getCubeBSplineCurveTangent(std::vector<glm::vec3> controlPoints, int precision){
        std::vector<glm::vec3> curvePointsTagent;
        float t = 0;
        int segmentNumber = 0;
        for (int j = 0; j < controlPoints.size() - 3 ; j++) {

            for (int i = 0; i < precision; i++) {
                if (t > 0.99999f) {
                    t = 0.f;
                }
                curvePointsTagent.push_back(Utils::getCubeBSplineCurvePointTagent(controlPoints, t, j));
                t += 1.0f / precision;
            }
        }
        return curvePointsTagent;

    }

// Function to create a rotation matrix to align an object with a given direction
    glm::mat4 createRotationMatrix(glm::vec3 objectPosition, glm::vec3 targetPosition) {
        // Calculate the view direction from object position to target position
        glm::vec3 axis = glm::cross(objectPosition, targetPosition);

        // Calculate the rotation angle (in radians) using the dot product
        if (glm::length(objectPosition) * glm::length(targetPosition) < 0.001f){
            return glm::mat4(1.0f);
        }
        float cosAngle = glm::dot(objectPosition, targetPosition) / (glm::length(objectPosition) * glm::length(targetPosition));
        if( cosAngle > 0.99999f){
            return glm::mat4(1.0f);
        }
        float angle = acos(cosAngle);

        // Create a rotation matrix
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
        //rotationMatrix = glm::transpose(rotationMatrix);
        return rotationMatrix;
    }



}