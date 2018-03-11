#include "stdafx.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow, int width, int height)
	: m_hInstance(hInstance), m_clientWidth(width), m_clientHeight(height), 
	  m_hWnd(nullptr), m_hRC(nullptr), m_shape(EShape::Undefined), m_restrictions(EVertexRestrictions::None)
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
#if 0
	// Initialize the scene.

	if (0 == m_clientHeight)    // prevent dividing by zero
	{
		m_clientHeight = 1;
	}

	GLfloat aspectRatio = m_clientWidth / (GLfloat)m_clientHeight;

	m_spScene = std::make_unique<Scene>(m_hWnd, GetDC(m_hWnd), aspectRatio);
#endif

	HDC hDC = GetDC(m_hWnd);
	if (!hDC)
	{
		std::cerr << __FUNCTION__ << ": device context is NULL\n";
		assert(false); return 1;
	}

	// Set up OpenGL context for our window.

	const int OpenGlMajor = 4;
	const int OpenGlMinor = 4;

	if (!OpenGLHelpers::setupOpenGlContext(OpenGlMajor, OpenGlMinor, hDC, m_hRC))
	{
		std::cerr << "Failed to set up OpenGL context (version " << OpenGlMajor << "." << OpenGlMinor << ")\n";
		assert(false); return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER, "shaders\\chaos.vert" },
		{ GL_GEOMETRY_SHADER, "shaders\\chaos.geom" },
		{ GL_FRAGMENT_SHADER, "shaders\\chaos.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (m_spScene)
		{
			m_spScene->render();
		}
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

void MainWindow::createScene()
{
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);

	GLfloat aspectRatio = (rect.right - rect.left) / (GLfloat)(rect.bottom - rect.top);

	m_spScene = std::make_unique<Scene>(
		GetDC(m_hWnd), aspectRatio, m_spProgram->getProgram(), m_shape, m_restrictions);
}

LRESULT CALLBACK MainWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static MainWindow *pMainWnd = nullptr;

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
		// Select shape.
		case ID_SHAPE_TRIANGLE:
			if (EShape::Triangle != pMainWnd->m_shape)
			{
				pMainWnd->m_shape = EShape::Triangle;

				pMainWnd->createScene();
			}
			break;
		case ID_SHAPE_SQUARE:
			if (EShape::Square != pMainWnd->m_shape)
			{
				pMainWnd->m_shape = EShape::Square;

				pMainWnd->createScene();
			}
			break;
		case ID_SHAPE_PENTAGON:
			if (EShape::Pentagon != pMainWnd->m_shape)
			{
				pMainWnd->m_shape = EShape::Pentagon;

				pMainWnd->createScene();
			}
			break;
		case ID_SHAPE_HEXAGON:
			if (EShape::Hexagon != pMainWnd->m_shape)
			{
				pMainWnd->m_shape = EShape::Hexagon;

				pMainWnd->createScene();
			}
			break;
		case ID_SHAPE_TETRAHEDRON:
			if (EShape::Tetrahedron != pMainWnd->m_shape)
			{
				pMainWnd->m_shape = EShape::Tetrahedron;

				pMainWnd->createScene();
			}
			break;
		// Select vertex restrictions.
		case ID_VR_NONE:
			if (EVertexRestrictions::None != pMainWnd->m_restrictions)
			{
				pMainWnd->m_restrictions = EVertexRestrictions::None;

				pMainWnd->createScene();
			}
			break;
		case ID_VR_NOT_THE_SAME:
			if (EVertexRestrictions::NotTheSame != pMainWnd->m_restrictions)
			{
				pMainWnd->m_restrictions = EVertexRestrictions::NotTheSame;

				pMainWnd->createScene();
			}
			break;
		case ID_VR_NOT_OFFSET_ONE:
			if (EVertexRestrictions::NotOffset_1 != pMainWnd->m_restrictions)
			{
				pMainWnd->m_restrictions = EVertexRestrictions::NotOffset_1;

				pMainWnd->createScene();
			}
			break;
		case ID_VR_NOT_OFFSET_ONE_ANTICLOCKWISE:
			if (EVertexRestrictions::NotOffset_1_Anticlockwise != pMainWnd->m_restrictions)
			{
				pMainWnd->m_restrictions = EVertexRestrictions::NotOffset_1_Anticlockwise;

				pMainWnd->createScene();
			}
			break;
		case ID_VR_NOT_OFFSET_TWO:
			if (EVertexRestrictions::NotOffset_2 != pMainWnd->m_restrictions)
			{
				pMainWnd->m_restrictions = EVertexRestrictions::NotOffset_2;

				pMainWnd->createScene();
			}
			break;
		// Other menu items.
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
			// Zoom in/out for the camera.
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
