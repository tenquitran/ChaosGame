#pragma once


namespace ChaosGameApp
{
	class MainWindow
	{
	public:
		// Parameters: hInstance - application instance;
		//             nCmdShow - controls how the window is to be shown;
		//             width - client area width (in pixels);
		//             height - client area height (in pixels).
		// Throws: Exception, std::bad_alloc
		MainWindow(HINSTANCE hInstance, int nCmdShow, int width, int height);

		virtual ~MainWindow();

		int runMessageLoop();

	protected:
		// The About box dialog procedure.
		static INT_PTR CALLBACK aboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

		// The main window procedure.
		static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		MainWindow(const MainWindow&) = delete;
		MainWindow& operator=(const MainWindow&) = delete;

		ATOM registerClass();

		BOOL initInstance(int nCmdShow);

		void createScene();

	protected:
		// Application instance.
		HINSTANCE m_hInstance;

		static const size_t MAX_LOADSTRING = 100;

		// Title bar text.
		TCHAR m_szTitle[MAX_LOADSTRING];

		// The window class name.
		TCHAR m_szWindowClass[MAX_LOADSTRING];

		// Width and height of the window's client area (in pixels).
		int m_clientWidth;
		int m_clientHeight;

		// Window handle.
		HWND m_hWnd;

		// OpenGL rendering context.
		HGLRC m_hRC;

		// GLSL program.
		std::unique_ptr<ProgramGLSL> m_spProgram;

		// Shape for the chaos game.
		EShape m_shape;

		// Vertex restrictions for the chaos game.
		EVertexRestrictions m_restrictions;

		// OpenGL scene for the chaos game.
		std::unique_ptr<Scene> m_spScene;
	};
}
