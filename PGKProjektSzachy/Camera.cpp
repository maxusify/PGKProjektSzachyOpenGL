#include "Camera.hpp"

const float Camera::TO_RADS = 3.141592654 / 180.0;

Camera::Camera() 
{
}

Camera::Camera(float _windowWidth, float _windowHeight) 
{
    initCamera();
    windowWidth = _windowWidth;
    windowHeight = _windowHeight;
    windowMidX = windowWidth / 2.0f;
    windowMidY = windowHeight / 2.0f;
}

Camera::~Camera() 
{
}

void Camera::initCamera() 
{
    position = glm::vec3(700.f, 300.f, 200.f);
    direction = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    speed = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    movementSpeedFactor = 50.0;

    glm::vec3 initialPosition = cartesianToPolar(position);
    angleH = initialPosition[0];
    angleV = initialPosition[1];
    distance = initialPosition[2];

    pitchSensitivity = 0.2;
    yawSensitivity = 0.2;

    holdingForward = false;
    holdingBackward = false;
    holdingLeftStrafe = false;
    holdingRightStrafe = false;
    holdingZoomNegative = false;
    holdingZoomPositive = false;
}

const float Camera::toRads(const float & _angleInDegrees) const 
{
    return _angleInDegrees * TO_RADS;
}

void Camera::handleMouseMove(int mouseX, int mouseY) 
{
    float horizMovement = (mouseX - windowMidX+1) * yawSensitivity;
    float vertMovement = (mouseY - windowMidY) * pitchSensitivity;

    rotation[0] += vertMovement;
    rotation[1] += horizMovement;

    if (rotation[0] < -90)
        rotation[0] = -90;
    if (rotation[0] > 90) 
        rotation[0] = 90;
    if (rotation[1] < 0)
        rotation[1] += 360;
    if (rotation[1] > 360)
        rotation[1] -= 360;
}

/**
* Konwersja
*/
glm::vec3 Camera::polarToCartesian (float phi, float theta, float d) {
    glm::vec3 tmp;

    tmp[0] = d * sin(theta) * cos(phi);
    tmp[1] = d * cos(theta);
    tmp[2] = d * sin(theta) * sin(phi);

    return tmp;
}

/**
* Konwersja
*/
glm::vec3 Camera::cartesianToPolar(glm::vec3 initPos) 
{
    float x = initPos[0];
    float y = initPos[1];
    float z = initPos[2];

    float phi = atan(y/x);
    float r = sqrt(x*x + y*y + z*z);
    float theta = acos(z/r);

    glm::vec3 tmp = glm::vec3(phi, theta, r);
    
    return tmp;
}

/**
* Przesuwa kamer? bior?c pod uwag? klatki na sekund?
*/
void Camera::move(int fps) 
{
    if (holdingForward) 
    {
        angleV -= toRads(45) / fps;

        if (angleV < toRads(0.001))
            angleV = toRads(0.001);  
    }

    if (holdingBackward) 
    {
        angleV += toRads(45) / fps;

        if (angleV > toRads(90.0))
            angleV = toRads(90.0);
    }

    if (holdingLeftStrafe) 
    {
        angleH += toRads(180) / fps;

        if (angleH > toRads(360))
            angleH = toRads(0);
    }

    if (holdingRightStrafe) 
    {
        angleH -= toRads(180) / fps;

        if (angleH < toRads(0))
            angleH = toRads(360);
    }

    if (holdingZoomNegative) 
    {
        distance += (float)500 / fps;

        if (distance >3000)
            distance -= (float)500 / fps;
    }

    if (holdingZoomPositive) 
    {
        distance -= (float)500 / fps;
        if (distance <0.001)
            distance += (float)500 / fps;
    }

    position = polarToCartesian(angleH, angleV, distance);
}
