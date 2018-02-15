#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Redirect output from std::cout and std::cerr to the log file.
	std::ofstream out("log.txt");
	std::cout.rdbuf(out.rdbuf());
	std::cerr.rdbuf(out.rdbuf());

	int res = 1;

	try
	{
		const int ClientWidth  = 500;
		const int ClientHeight = 500;

		MainWindow mainWindow(hInstance, nCmdShow, ClientWidth, ClientHeight);

		res = mainWindow.runMessageLoop();
	}
	catch (const Exception& ex)
	{
		std::cerr << std::wstring_convert< std::codecvt_utf8<wchar_t> >().to_bytes(ex.message()) << '\n';
		assert(false);
	}
	catch (const std::bad_alloc&)
	{
		std::cerr << "Memalloc failure\n";
	}

	return res;
}
