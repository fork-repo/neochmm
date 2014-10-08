#include "win.h"
#include <stdio.h>
#include <string.h>

using namespace std;

HWND rootHWND;
HINSTANCE rootHInstance;
void (*EXITFunction)();
void (*UPDATEFunction)();

static void debug(const char * format,...)
{
    char buf[1024 ];
    va_list ap;
    va_start(ap , format);
    _vsnprintf(buf , sizeof (buf), format, ap );
    OutputDebugString (buf);
    va_end(ap );
}

static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	int argc;
	argc = 0;
	bufp = cmdline;
	for ( bufp = cmdline; *bufp; ) {
		int tmpcount = 0;
		char * tmppoint = bufp;
		while ( (*bufp != ' ') && (*bufp != '\0') ) {
			++bufp;
			tmpcount++;
		}
		if ( *bufp != '\0') {
			if ( argv ) {
				argv[argc] = tmppoint;
				argv[argc][tmpcount] = '\0';
			}
			++bufp;
		//跨過n個空白
			while ( (*bufp == ' ') ) {
				++bufp;
			}
		}else{
			if ( argv ) {
				argv[argc] = tmppoint;
				argv[argc][tmpcount] = '\0';
			}
			argc++;	
			break;
		}
		argc++;		
	}
	return argc;
}

size_t StringLength(const char *string)
{
    size_t len = 0;
    while ( *string++ ) {
        ++len;
    }
    return len;
}

LRESULT CALLBACK WndProcedure(HWND hWnd , UINT Msg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    PAINTSTRUCT Ps;
    HFONT font;
    switch(Msg)
    {
        case WM_CREATE:
                //printf("window create\n");
                break;
        case WM_SETFOCUS: 
                //printf("WM_SETFOCUS\n");                      
                 break;  
        case WM_COMMAND:
                 //printf("WM_COMMAND\n"); 
                 break;
        case WM_PAINT:
                 //printf("WM_PAINT\n");
                 if (UPDATEFunction){
                    UPDATEFunction();
                  }
                break;
        case WM_TIMER:
                 // a timer msg was received so call our main function!
                //PostQuitMessage(WM_QUIT);
                break;
         case WM_DESTROY:
                 PostQuitMessage(WM_QUIT);
                 if (EXITFunction){
                    EXITFunction();
                  }
                 break;
         default:
                 //printf("default\n");
                 return DefWindowProc(hWnd, Msg , wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance , PSTR szCmdLine , int iCmdShow)
{
	rootHInstance = hInstance;
	HMODULE handle;
	char **argv;
	int argc;
	char *cmdline;
	char *env_str;
	//讀DDRAW.DLL
	handle = LoadLibrary(TEXT("DDRAW.DLL"));
	if ( handle != NULL ) {
		//printf("load DDRAW.DLL success!!\n");
		FreeLibrary(handle);
	}
	char *pbuffer = GetCommandLine();
	int bufferlen = StringLength(pbuffer);
	cmdline = (char*)malloc(sizeof(char) * (bufferlen + 1));
	strncpy(cmdline, pbuffer, bufferlen);
	cmdline[bufferlen] = '\0';
	//cmdline = (char*)malloc(sizeof(char) * bufferlen);
	argc = ParseCommandLine(cmdline, NULL);
	argv = (char**)malloc((sizeof(char*)*argc));
	ParseCommandLine(cmdline, argv);
	/*
	int i=0;
	for (i=0;i<argc;i++){
		printf("[%s]\n",argv[i]);
	}
	*/
	console_main(argc,argv);
    return 0;
}
void Init()
{
      HWND       hWnd ;
      WNDCLASS wce;
      int winWidth = 300;
      int winHeight = 300;
      memset(&wce, 0,sizeof(WNDCLASS));
      //wce.lpfnWndProc = DefWindowProc;
      wce.lpfnWndProc = WndProcedure;
      wce.lpszClassName = "BasicApp";
      wce.hInstance = rootHInstance;
      wce.hbrBackground = (HBRUSH)(COLOR_WINDOW);
     // RegisterClass(&wce);
      if ( RegisterClass(&wce) == 0 ) {
        MessageBox(NULL, "Register window fail!", "ERROR!" , MB_ICONEXCLAMATION | MB_OK);
         return;
      }

        int screenX = GetSystemMetrics(SM_CXSCREEN );
        int screenY = GetSystemMetrics(SM_CYSCREEN );

        int xCtr = (screenX / 2) - ( winWidth / 2);
        int yCtr = (screenY/ 2) - ( winHeight / 2);
 

      hWnd = CreateWindowEx(0, "BasicApp",
                                           "BasicApp",
                                           WS_OVERLAPPED, CW_USEDEFAULT,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           CW_USEDEFAULT, NULL, NULL,
                                           rootHInstance, NULL);
       if(hWnd == NULL ){
          MessageBox (NULL, "Create window fail!", "ERROR!" , MB_ICONEXCLAMATION | MB_OK);
        return;
      }
      rootHWND = hWnd;

}

COMPONENTID* CreateWindows(const char* title,int winWidth, int winHeight, void (*exitFunction)(), void (*updateFunction)())
{
	  //COLORREF backgroundColor = RGB(238, 238, 224);
    COMPONENTID* pid = (COMPONENTID*)malloc(sizeof(COMPONENTID));
        HWND       hWnd ;        //CreateWindow時傳回值
        //註冊視窗
        const char ClsName[]  = "BasicApp" ;
        int WndNameLen = StringLength(title);
        char *WndName = (char*)malloc(sizeof(char)*WndNameLen);
        strcpy(WndName, title);
        
        //debug ("create window[%s]\n", WndName);
/*
        WNDCLASSEX WndClsEx ;
        WndClsEx .cbSize      = sizeof(WNDCLASSEX );
        WndClsEx .style       = CS_HREDRAW | CS_VREDRAW;
        WndClsEx .lpfnWndProc = WndProcedure;
        WndClsEx .cbClsExtra  = 0;
        WndClsEx .cbWndExtra  = 0;
        WndClsEx .hInstance   = rootHInstance;
        WndClsEx .hIcon       = LoadIcon (NULL, IDI_APPLICATION);
        WndClsEx .hIconSm     = LoadIcon(NULL, IDI_APPLICATION);
        WndClsEx .hCursor     = LoadCursor(NULL, IDC_ARROW);
        //WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        //WndClsEx .hbrBackground = (HBRUSH) CreateSolidBrush(backgroundColor );
        //WndClsEx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
        WndClsEx .lpszMenuName  = NULL;
        WndClsEx .lpszClassName = ClsName;
        if(!RegisterClassEx (&WndClsEx))
    {
        MessageBox (NULL, "register windows fail !", "ERROR!" , MB_ICONEXCLAMATION | MB_OK);
        return 0 ;
    }
    */
        int screenX = GetSystemMetrics(SM_CXSCREEN );
        int screenY = GetSystemMetrics(SM_CYSCREEN );

        int xCtr = (screenX / 2) - ( winWidth / 2);
        int yCtr = (screenY/ 2) - ( winHeight / 2);
        hWnd = CreateWindow(ClsName,
                          WndName ,
                          (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX),
                          xCtr ,
                          yCtr ,
                          winWidth ,
                          winHeight ,
                          NULL,
                          NULL,
                          rootHInstance ,
                          NULL);
         if(hWnd == NULL )
        {
        MessageBox (NULL, "Create window fail!", "ERROR!" ,
            MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    pid->hwnd = hWnd;
    EXITFunction = exitFunction;
    UPDATEFunction = updateFunction;
    //rootHWND = hWnd;
    return pid;
    /*
        ShowWindow(hWnd, SW_SHOWNORMAL);
        UpdateWindow(hWnd);
        while( GetMessage (&Msg, NULL, 0 , 0 ) > 0 )
        {
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
        }

        return Msg.wParam;
        */

}

int ShowWindows(COMPONENTID* pid)
{
	 	ShowWindow(pid->hwnd, SW_SHOWNORMAL);
    UpdateWindow(pid->hwnd);
}

void drawSetPixel(COMPONENTID* pid, int x, int y, int R, int G,int B)
{
   HDC hdc;
   COLORREF color = RGB(R,G,B);
   hdc = GetDC(pid->hwnd);
   SetPixel(hdc, x, y, color);
   ReleaseDC(pid->hwnd, hdc);

}
void drawMoveTo(COMPONENTID* pid, int x, int y)
{
   HDC hdc;
   hdc = GetDC(pid->hwnd);
   MoveToEx(hdc, x, y,(LPPOINT) NULL);
   ReleaseDC(pid->hwnd, hdc);
}
void drawLineTo(COMPONENTID* pid, int x, int y)
{
   HDC hdc;
   hdc = GetDC(pid->hwnd);
   LineTo(hdc, x, y);
   ReleaseDC(pid->hwnd, hdc);
}

void drawRectangle(COMPONENTID* pid, int x1, int y1,int x2, int y2)
{
   HDC hdc;
   hdc = GetDC(pid->hwnd);
   Rectangle(hdc, x1, y1, x2, y2);
   ReleaseDC(pid->hwnd, hdc);
}
int PullEvents(EVENTS* pEvents)
{
  MSG msg; 
  int ret = 0;
  memset(pEvents,0,sizeof(EVENTS)); 
  PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);  
  if (msg.message == WM_KEYDOWN)
  {
    pEvents->eventtype = KEVENT;
    switch (msg.wParam)
    {
    case VK_ESCAPE:
      pEvents->keyevent.key = 27;
      pEvents->keyevent.ctrl = 0;
      pEvents->keyevent.shift = 0;
      break;
    case VK_RETURN:
      pEvents->keyevent.key = 13;
      pEvents->keyevent.ctrl = 0;
      pEvents->keyevent.shift = 0;
      break;
    default:
      if(::GetKeyState(VK_CONTROL) < 0){
         pEvents->keyevent.ctrl = 1;
      }
      if(::GetKeyState(VK_SHIFT) < 0){
         pEvents->keyevent.shift = 1;
      }   
      pEvents->keyevent.key = msg.wParam;
      break;
    }
    ret = 1; 
  }else if (msg.message == WM_MOUSEMOVE) {
    pEvents->eventtype = MEVENT;
    pEvents->mouseevent.state = (MOUSE_STATE) msg.wParam;
    pEvents->mouseevent.x = msg.lParam &0xffff;
    pEvents->mouseevent.y = (msg.lParam & 0xffff0000) >> 16;
    ret = 1; 
  }else if(msg.message == WM_SETFOCUS){
    printf("MOvE\n");

  }
  TranslateMessage( &msg );
  DispatchMessage( &msg );
  return ret;
}

WIN::WIN()
{
	if (strcmp(OS,"WIN") ==0){
		printf("%s\n",OS);
		printf("end\n");

	}
}
WIN::~WIN()
{

}