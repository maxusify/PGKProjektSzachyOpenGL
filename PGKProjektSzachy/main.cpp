#include <iostream>
#include <Windows.h>
#include "Application.hpp"

// Wymuszenie u�ycia dedykowanej karty graficznej

//#ifdef __cplusplus
//extern "C" {
//#endif
//
//    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//
//#ifdef __cplusplus
//}
//#endif

using namespace std;

int main(void)
{
    Application::start();
}
