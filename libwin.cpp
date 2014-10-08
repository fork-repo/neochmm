#include "win.h"
#include <stdio.h>
#include <string.h>

using namespace std;

HWND rootHWND;
HINSTANCE rootHInstance;

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
                printf("window create\n");
                return TRUE;
                break;
        case WM_SETFOCUS:                       
                 break;  
        case WM_COMMAND:
                 break;
        case WM_PAINT:
                break;
        case WM_TIMER:
                 // a timer msg was received so call our main function!
                PostQuitMessage(WM_QUIT);
                return TRUE;
                break;
         case WM_DESTROY:
                PostQuitMessage(WM_QUIT);
                 break;
         default:
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
     // rootHWND = hWnd;

}

int CreateWindows(const char* title,int winWidth, int winHeight)
{
	  // COLORREF backgroundColor = RGB (238, 238 , 224 );
      // int winWidth = 300 ;
      // int winHeight = 300 ;     
   	  //     MSG        Msg ;         //GetMessage傳回值
        HWND       hWnd ;        //CreateWindow時傳回值
        //註冊視窗
        const char ClsName[]  = "BasicApp" ;
        int WndNameLen = StringLength(title);
        char *WndName = (char*)malloc(sizeof(char)*WndNameLen);
        strcpy(WndName, title);
        
        //debug ("create window[%s]\n", WndName);

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
        return 0 ;
    }
    //rootHWND = hWnd;
    return 0;
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

int ShowWindows()
{
	 	MSG        msg ;  
	 	ShowWindow(rootHWND, SW_SHOWNORMAL);
        UpdateWindow(rootHWND);
       /* while(GetMessage(&Msg, NULL, 0 , 0 ) > 0 )
        {
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
        }*/
       while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			if ( msg.message == WM_QUIT ) break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			//printf("aaaa\n");
		}
        return msg.wParam;
}

int Pullevents()
{
    MSG        msg ;  
   // ShowWindow(rootHWND, SW_SHOWNORMAL);
   //     UpdateWindow(rootHWND);
    
      TranslateMessage( &msg );
      DispatchMessage( &msg );
      //printf("aaaa\n");
        return msg.wParam;
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