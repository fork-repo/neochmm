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

typedef struct 
{
#ifdef _WIN32
	HWND hwnd;
#endif	
} COMPONENTID;

typedef enum {
	KEVENT,
	MEVENT,
	WEVENT
} EVENT_TYPE;
typedef enum {
	MOVE,
	LBUTTONDOWN,
	RBUTTONDOWN,
	LRBOTHBUTTONDOWN,
	LBUTTONUP,
	RBUTTONUP,
	LCLICKED,
	RCLICKED
} MOUSE_STATE;

typedef struct 
		{	
			char key;
			int ctrl;
			int shift;
		}KEYEVENT;

typedef struct 
		{	
			int x;
			int y;
			MOUSE_STATE state;
		}MOUSEEVENT;

typedef struct 
		{	
			int eventnumber;
		}WINDOWEVENT;

typedef struct 
{
	EVENT_TYPE eventtype;
	union{
		KEYEVENT keyevent;
		MOUSEEVENT mouseevent;
		WINDOWEVENT windowevent;
	};
} EVENTS;

const char OS[]=ENV_OS;

extern void Init();
extern int PullEvents(EVENTS* pEvents);
extern int console_main(int argc, char *argv[]);
extern COMPONENTID* CreateWindows(const char* title,int winWidth, int winHeight, void (*exitFunction)(),void (*updateFunction)());
extern int ShowWindows(COMPONENTID* pid);
extern void drawSetPixel(COMPONENTID* pid, int x, int y, int R, int G,int B);
extern void drawLineTo(COMPONENTID* pid, int x, int y);
extern void drawRectangle(COMPONENTID* pid, int x1, int y1,int x2, int y2);
extern void drawMoveTo(COMPONENTID* pid, int x, int y);

class WIN
{
public:
	WIN();
	~WIN();
private:
};
