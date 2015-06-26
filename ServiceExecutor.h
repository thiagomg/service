#pragma once

#include "ServiceBase.h"

#include <string>
#include <thread>
#include <condition_variable>
#include <map>

#include "Configuration.h"

class ServiceExecutor : public ServiceBase
{
private:
	_tstring _serviceName;

	volatile bool _running = true;
	volatile bool _mustStop = false;

	std::thread *_svcThread = nullptr;

	std::condition_variable _cond;
	std::mutex _mutex;

	std::map<_tstring, _tstring> _extraConfigs;
	_tstring _serviceBin;
	_tstring _serviceDir;
	_tstring _configFileName;

	Configuration _config;

protected:
	bool _loadConfiguration(DWORD dwArgc, PWSTR *pszArgv);
	_tstring _findConfigFile();

public:
	ServiceExecutor(TCHAR serviceName[]);
	~ServiceExecutor();

	void addExtraConfigs(const std::map<_tstring, _tstring> &extraConfigs);

	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);
	virtual void OnStop();
	virtual void OnPause();
	virtual void OnContinue();
	virtual void OnShutdown();

	virtual void runService();

};

