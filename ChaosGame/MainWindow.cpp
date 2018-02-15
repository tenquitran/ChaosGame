#include "stdafx.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow, int width, int height)
	: m_hInstance(hInstance), m_clientWidth(width), m_clientHeight(height)
{
	if (!m_hInstance)
	{
		assert(false); throw EXCEPTION(L"Application instance is NULL");
	}
	else if (   m_clientWidth  < 1 
		     || m_clientHeight < 1)
	{
		assert(false); throw EXCEPTION_FMT(L"Invalid window size: (%d, %d)", m_clientWidth, m_clientHeight);
	}

	// Initialize global strings.
	LoadString(hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHAOSGAME, m_szWindowClass, MAX_LOADSTRING);

	if (!registerClass())
	{
		assert(false); throw EXCEPTION(L"Failed to register windows class");
	}

	// Perform application initialization:
	if (!initInstance(nCmdShow))
	{
		throw EXCEPTION(L"Failed to initialize application instance");
	}
}

MainWindow::~MainWindow()
{
}

ATOM MainWindow::registerClass()
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = windowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CHAOSGAME));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_CHAOSGAME);
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL MainWindow::initInstance(int nCmdShow)
{
	const DWORD WINDOW_STYLE = WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

	RECT wndRect = { 0, 0, m_clientWidth, m_clientHeight };
	
	if (!AdjustWindowRect(&wndRect, WINDOW_STYLE, FALSE))
	{
		std::cerr << "AdjustWindowRect() failed: " << GetLastError() << '\n';
		assert(false); return FALSE;
	}

	m_hWnd = CreateWindow(m_szWindowClass, m_szTitle, WINDOW_STYLE, CW_USEDEFAULT, CW_USEDEFAULT, 
		wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, nullptr, nullptr, m_hInstance, this);
	if (!m_hWnd)
	{
		std::cerr << "CreateWindow() failed: " << GetLastError() << '\n';
		assert(false); return FALSE;
	}

	ShowWindow(m_hWnd, nCmdShow);

	if (!UpdateWindow(m_hWnd))
	{
		std::cerr << "UpdateWindow() failed\n";
		assert(false); return FALSE;
	}

	return TRUE;
}

int MainWindow::runMessageLoop()
{
	MSG msg = {};

	// Initialize the scene.

	if (0 == m_clientHeight)    // prevent dividing by zero
	{
		m_clientHeight = 1;
	}

	GLfloat aspectRatio = m_clientWidth / (GLfloat)m_clientHeight;

	m_spScene = std::make_unique<Scene>(m_hWnd, GetDC(m_hWnd), aspectRatio);

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_spScene->render();
	}

	return (int)msg.wParam;
}

INT_PTR MainWindow::aboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK MainWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static MainWindow *pMainWnd = nullptr;
#if 0
	PAINTSTRUCT ps;
	HDC hdc;
#endif

	switch (message)
	{
	case WM_CREATE:
		pMainWnd = (MainWindow *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		assert(pMainWnd);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(pMainWnd->m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, aboutProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
#if 0
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
#endif
	case WM_SIZE:
		if (pMainWnd->m_spScene)
		{
			WORD newClientWidth  = LOWORD(lParam);
			WORD newClientHeight = HIWORD(lParam);

			pMainWnd->m_spScene->resize(newClientWidth, newClientHeight);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		{
			int key = (int)wParam;
			//LPARAM keyData = lParam;

			switch (key)
			{
#if 0
			case VK_ESCAPE:
				DestroyWindow(hWnd);
				break;
			// Translate the model.
			case VK_NUMPAD2:
				pMainWnd->m_spDemo->translateModelX(0.1f);
				break;
			case VK_NUMPAD3:
				pMainWnd->m_spDemo->translateModelX(-0.1f);
				break;
				// TODO: translate the second model.
			case VK_NUMPAD4:
				pMainWnd->m_spDemo->translateModelX_2(0.7f);
				break;
			case VK_NUMPAD5:
				pMainWnd->m_spDemo->translateModelX_2(-0.7f);
				break;
			// Translate the camera.
			case 0x57:    // W key
				pMainWnd->m_spDemo->translateCameraY(0.1f);
				break;
			case 0x53:    // S key
				pMainWnd->m_spDemo->translateCameraY(-0.1f);
				break;
			case 0x41:    // A key
				pMainWnd->m_spDemo->translateCameraX(-0.1f);
				break;
			case 0x44:    // D key
				pMainWnd->m_spDemo->translateCameraX(0.1f);
				break;
				// TODO: more appropriate keys to translate on the Z axis
			case 0x5A:    // Z key: move farther
				pMainWnd->m_spDemo->translateCameraZ(-0.1f);
				break;
			case 0x43:    // C key: move closer
				pMainWnd->m_spDemo->translateCameraZ(0.1f);
				break;
#endif
			// Rotate the camera.
			case VK_UP:
				pMainWnd->m_spScene->rotateCameraX(2.0f);
				break;
			case VK_DOWN:
				pMainWnd->m_spScene->rotateCameraX(-2.0f);
				break;
			case VK_LEFT:
				pMainWnd->m_spScene->rotateCameraY(-2.0f);
				break;
			case VK_RIGHT:
				pMainWnd->m_spScene->rotateCameraY(2.0f);
				break;
			// Zoom in/out for camera.
			case VK_NUMPAD0:
				pMainWnd->m_spScene->scaleCamera(-0.05f);
				break;
			case VK_NUMPAD1:
				pMainWnd->m_spScene->scaleCamera(0.05f);
				break;
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
