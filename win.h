#pragma once
#ifdef _WIN32
#include <windows.h>
#define ENV_OS "WIN"
//把原來的main改成console_main()
#define main    console_main
#elif __unix__
#define ENV_OS "UNIX"
#elif __linux__
#define ENV_OS "LINUX"
#elif __APPLE__
#define ENV_OS "MAC"
#endif

const char OS[]=ENV_OS;

extern int console_main(int argc, char *argv[]);

extern int CreateWindows(const char* title,int winWidth, int winHeight);

extern int ShowWindows();


class WIN
{
public:
	WIN();
	~WIN();
private:
};
