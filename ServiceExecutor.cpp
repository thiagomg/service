#include "stdafx.h"

#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <exception>

#include <cstdio>

#include "ServiceExecutor.h"
#include "Utils.h"

//TODO: Criar eventos de pause/cont/shutdown/etc

using namespace std;

struct loader_exception : public std::exception {
	std::string _text;
	loader_exception(const std::string &text) {
		_text = text;
	}

	const char *what() const {
		return _text.c_str();
	}

};

ServiceExecutor::ServiceExecutor(TCHAR serviceName[]) : ServiceBase(serviceName)
{
	this->_serviceName = serviceName;
}


ServiceExecutor::~ServiceExecutor()
{
}

void ServiceExecutor::OnStart(DWORD dwArgc, PWSTR *pszArgv)
{
	//LOG: _serviceName starting
	_running = true;

	_svcThread = new thread([this]() {
		this->runService();
		_running = false;
		_cond.notify_all();
	});


	//Carregar a configuracao 
	bool ret = _loadConfiguration(dwArgc, pszArgv);
	if (!ret) {
		std::string fName;
		throw loader_exception(std::string("Error when loading config:") + fName);
	}

}

void ServiceExecutor::OnStop()
{
	_running = false;
	_mustStop = true;

	if (_svcThread != nullptr) {
		//LOG: Stopping...
		//_svcThread->join();

		auto timeout = std::chrono::seconds(10);
		unique_lock<mutex> lock(_mutex);
		_cond.wait_for(lock, timeout, [&]() {
			return (_running == false);
		});
	}
}

void ServiceExecutor::OnPause()
{
	_running = false;
}

void ServiceExecutor::OnContinue()
{
	_running = true;
}

void ServiceExecutor::OnShutdown()
{
	_running = false;
}

void ServiceExecutor::runService()
{
	//Aqui a carga dos plugins será feita.
	int count = 0;
	while (!_mustStop) {
		FILE *f = fopen(R"(c:\CCASE\Thiago\src\winsvc\winsvc\Debug\out.log)", "a");
		if (f != nullptr) {
			fprintf(f, "running %d ...\r\n", ++count);
			fclose(f);
		}
		Sleep(100);
	}
}

void ServiceExecutor::addExtraConfigs(const std::map<_tstring, _tstring> &extraConfigs)
{
	_extraConfigs = extraConfigs;
}

_tstring ServiceExecutor::_findConfigFile()
{
	const auto it = _extraConfigs.find(_T("config"));
	if (it != _extraConfigs.end()) {
		return it->second;
	}

	if (Utils::fileExists(_serviceBin + _T(".cfg"))) {
		return _serviceBin + _T(".cfg");
	}
	if (Utils::fileExists(_serviceDir + _T("service.cfg"))) {
		return _serviceDir + _T("service.cfg");
	}

	return _T("service.cfg");
}

bool ServiceExecutor::_loadConfiguration(DWORD dwArgc, PWSTR *pszArgv)
{
	if (dwArgc == 0)
		return false;

	_serviceBin = _extraConfigs[_T("binName")];
	_serviceDir = Utils::getDirName(_serviceBin);
	if (Utils::endsWith(_serviceDir, { _T('\\'), _T('/') }))
		_serviceDir += Utils::getSeparator(_serviceDir, std::unordered_set<_TCHAR>{ _T('\\'), _T('/') });

	_configFileName = _findConfigFile();

	return _config.loadFile(_configFileName);

	return true;
}
