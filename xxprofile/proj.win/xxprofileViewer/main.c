#include <windows.h>

void mainLoop();

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow) {
    mainLoop();
    return 0;
}
