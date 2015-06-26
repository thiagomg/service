// winsvc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "ServiceInstaller.h"
#include "ServiceExecutor.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

//TODO: Configuravel!
//#define SERVICE_NAME             _T("ThiagoService")
//#define SERVICE_DISPLAY_NAME     _T("ThiagoService Teste")
#define SERVICE_START_TYPE       SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES     _T("")
//#define SERVICE_ACCOUNT          _T("NT AUTHORITY\\LocalService")
//#define SERVICE_PASSWORD         NULL

template<typename T>
bool parseParameters(int argc, _TCHAR* argv[], 
	bool &install, bool &remove, bool &debug, map<T, T> &extra)
{

	using KV = std::pair<T, T>;

	for (int i = 1; i < argc; i++) {

		if ((*argv[i] == L'-' || (*argv[i] == L'/'))) {
			if (_tcsicmp(L"install", argv[i] + 1) == 0) {
				install = true;
			} else if (_tcsicmp(L"remove", argv[i] + 1) == 0) {
				remove = true;
			}
			else if (_tcsicmp(L"debug", argv[i] + 1) == 0) {
				debug = true;
				continue;
			}
			else if (_tcsicmp(L"d", argv[i] + 1) == 0) {
				debug = true;
				continue;
			}
			else if (_tcsicmp(L"-help", argv[i] + 1) == 0) {
				return false;
			} else if (_tcsicmp(L"h", argv[i] + 1) == 0) {
				return false;
			}
			else {
				if ((i + 1) < argc) {
					extra.insert(KV(argv[i] + 1, argv[i + 1]));
					i++; //jump value
				}
				else
					extra.insert(KV(argv[i] + 1, L""));
			}
		}
		else
		{
			extra.insert(KV(argv[i] + 1, L""));
		}

	}

	extra.insert(make_pair(L"binName", argv[0]));

	return true;

}

int _tmain(int argc, _TCHAR* argv[])
{
	bool install = false;
	bool remove = false;
	bool debug = false;
	map<_tstring, _tstring> extra;

	//TODO: Adicionar -config
	if (!parseParameters(argc, argv, install, remove, debug, extra)) {
		wprintf(L"Parameters:\n");
		wprintf(L"-install - install the service.\n");
		wprintf(L"-remove - remove the service.\n");
		wprintf(L"-debug - run service as console app.\n");
		wprintf(L"\n----- If installing the service\n");
		wprintf(L"-svcName - Service Name\n");
		wprintf(L"-displayName - Display Name [optional]\n");
		wprintf(L"-account - User account to start [optional]\n");
		wprintf(L"           Default: NT AUTHORITY\\LocalService\n");
		wprintf(L"-password - Account password [optional]\n");
		wprintf(L"\n");
		return 1;
	}

	_tstring svcName = extra[L"svcName"];
	if ( install ) {

		_tstring displayName = extra[L"displayName"];
		_tstring account = extra[L"account"];
		_tstring password = extra[L"password"];

		if (svcName.empty()) {
			wprintf(L"Service Name must be set\n");
			return 1;
		}
		if( account.empty() ) {
			account = _T("NT AUTHORITY\\LocalService");
		}

		if (displayName.empty()) {
			wprintf(L"DisplayName not present. Set to %s\n", svcName.c_str());
			displayName = svcName;
		}

		// Install the service when the command is 
		// "-install" or "/install".
		InstallService(
			(PWSTR)svcName.c_str(),            // Name of service
			(PWSTR)displayName.c_str(),        // Name to display
			SERVICE_START_TYPE,         // Service start type
			SERVICE_DEPENDENCIES,       // Dependencies
			(PWSTR)account.c_str(),            // Service running account
			password.empty() ? nullptr : (PWSTR)password.c_str() // Password of the account
			);
	} else if (remove) {
		// Uninstall the service when the command is 
		// "-remove" or "/remove".
		UninstallService((PWSTR)svcName.c_str());
	} else if (debug) {
		wprintf(_T("Running in debug mode\n"));
		ServiceExecutor service((PWSTR)svcName.c_str());
		service.addExtraConfigs(extra);
		service.OnStart(argc, argv);
		service.runService();
	
	} else {

		wprintf(_T("Running in normal mode\n"));
		ServiceExecutor service((PWSTR)svcName.c_str());
		service.addExtraConfigs(extra);
		if (!ServiceBase::Run(service))
		{
			wprintf(_T("Service failed to run w/err 0x%08lx\n"), GetLastError());
		}
	}
	return 0;
}

