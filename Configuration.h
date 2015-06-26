#pragma once

#include <vector>

class Configuration
{
public:
	Configuration(void);
	~Configuration(void);

	void setSelectedChannel(int selectedChannel);
	int getSelectedChannel();

	bool loadFile(const _tstring &fileName);

	const _tstring *get(const _tstring &key);
	_tstring getValue(const _tstring &key);
	std::vector< std::pair<_tstring, _tstring> > getPrefix(const _tstring &prefix);

protected:
	void _parseLine(const _tstring &line);
	bool _getTokens(const _tstring &line, _tstring &key, _tstring &val);

private:
	std::map<_tstring, _tstring> _mapConfig;

	int _selectedChannel;

};
