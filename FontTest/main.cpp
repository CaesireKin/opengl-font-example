#include <gl/glew.h>
#include <gl/wglew.h>
#include <Windows.h>

#pragma comment (lib,"glew32.lib")
#pragma comment (lib,"glu32.lib")
#pragma comment (lib,"opengl32.lib")

int nFontList;

void Render(HDC);
void Resize(int width,int height);
void Configure(HDC);

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int iCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASSEX wndex;

	wndex.cbSize =sizeof(WNDCLASSEX);
	wndex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndex.cbClsExtra = 0;
	wndex.cbWndExtra = 0;
	wndex.lpfnWndProc = WndProc;
	wndex.hInstance = hInstance;
	wndex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndex.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndex.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndex.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	wndex.lpszClassName = TEXT("Main");
	wndex.lpszMenuName = NULL;

	if(!RegisterClassEx(&wndex))
	{
		MessageBox(NULL,TEXT("Initialize Failed"),TEXT("Error"),MB_OK);
		return 0;
	}

	hWnd = CreateWindow(TEXT("Main"),TEXT("FontTest"),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,
		CW_USEDEFAULT,1280,720,NULL,NULL,hInstance,NULL);

	ShowWindow(hWnd,iCmdShow);
	UpdateWindow(hWnd);

	HDC hDC = GetDC(hWnd);

	int iPixelFormat;

	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA,
		32,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,
		16,
		0,0,0,0,0,0,0
	};

	iPixelFormat = ChoosePixelFormat(hDC,&pfd);

	SetPixelFormat(hDC,iPixelFormat,&pfd);

	HGLRC hRenderContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC,hRenderContext);

	GLenum err = glewInit();

	if(GLEW_OK != err)
	{
		MessageBox(NULL,TEXT("Glew Initialize Failed"),TEXT("Error"),MB_OK);
		return false;
	}

	int attribs[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB,3,
		WGL_CONTEXT_MINOR_VERSION_ARB,1,
		WGL_CONTEXT_FLAGS_ARB,0,0
	};

	//创建更高版本的OpenGL RenderContext句柄
	if(wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hRenderContext = wglCreateContextAttribsARB(hDC,0,attribs);
		wglMakeCurrent(NULL,NULL);
		wglMakeCurrent(hDC,hRenderContext);
	}

	//获取当前驱动支持的OpenGL版本
	GLubyte* strGLVersion = const_cast<GLubyte*>(glGetString(GL_VERSION));
	
	int Major_Version , Minor_Version;
	//获取当前能够使用的OpenGL最高版本和最低版本
	glGetIntegerv(GL_MAJOR_VERSION,&Major_Version);
	glGetIntegerv(GL_MINOR_VERSION,&Minor_Version);

	RECT rect;
	GetClientRect(hWnd,&rect);
	Resize(rect.right,rect.bottom);

	Configure(hDC);

	while(1)
	{
		if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			if(WM_QUIT == msg.message)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Render(hDC);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_SIZE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}

void Render(HDC hdc)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glColor3f(0.7f,0.7f,0.7f);
	glListBase(nFontList);
	glCallLists(6,GL_UNSIGNED_BYTE,"OpenGL");

	glPopMatrix();

	SwapBuffers(hdc);
}

void Configure(HDC hDC)
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	HFONT hFont;
	GLYPHMETRICSFLOAT gmf[128];
	LOGFONT logFont;
	
	logFont.lfHeight = -10;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_BOLD;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = FALSE;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	wcscpy_s(logFont.lfFaceName,TEXT("Arial"));

	hFont = CreateFontIndirect(&logFont);
	SelectObject(hDC,hFont);

	nFontList = glGenLists(128);
	wglUseFontOutlines(hDC,0,128,nFontList,0.0f,0.5f,WGL_FONT_POLYGONS,gmf);

	DeleteObject(hFont);
}

void Resize(int width,int height)
{
	GLdouble aspect = (GLdouble)width/(GLdouble)height;
	GLdouble fovy = 90.0;
	GLdouble zNear = 1.0;
	GLdouble zFar = 32000.0;

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy,aspect,zNear,zFar);
}