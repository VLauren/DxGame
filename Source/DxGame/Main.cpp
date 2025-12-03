
#include "DxGameApp.h"

// ===========
// Entry Point
// ===========
int main(int argc, char* argv[])
{
	// Read current debug flag word
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	// Add the flag to dump unfreed blocks on exit
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);

	// TODO init logging system

	DxGameApp application{ "DxGame" };
	application.Run();

	// Reports leaks to stderr
	_CrtDumpMemoryLeaks();

	// TODO Destroy logging system

	return 0;
}

