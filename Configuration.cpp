#include "stdafx.h"

#include <map>
#include <sstream>
#include "Configuration.h"

Configuration::Configuration(void) : _selectedChannel(0)
{
}

Configuration::~Configuration(void)
{
}

void Configuration::setSelectedChannel(int selectedChannel)
{
	_selectedChannel = selectedChannel;
}

int Configuration::getSelectedChannel()
{
	return _selectedChannel;
}

bool Configuration::loadFile(const _tstring &fileName)
{
	FILE *f = _tfopen(fileName.c_str(), __T("r"));
	if( f == NULL ) {
		return false;
	}

	_tstring fileContent;
	{
		_tstringstream ss;
		char buf[4096];
		while( !feof(f) ) {		
			size_t count = fread(buf, 1, sizeof(buf), f);
			buf[count] = 0;
			ss << buf;
		}
		fileContent = ss.str();
		fclose(f);
	}

	_tstringstream ss;
	for(_tstring::const_iterator it=fileContent.begin(); it != fileContent.end(); it++) {
		if( *it == 13 || *it == 10 ) {
			_tstring line = ss.str();
			ss.str(_T(""));
			if( line.size() > 0 )
				_parseLine(line);
		} else {
			ss << *it;
		}
	}

	return true;
}

void Configuration::_parseLine(const _tstring &line)
{
	_tstring key, val;
	if( _getTokens(line, key, val) ) {
		_mapConfig[key] = val;
		printf("[%s][%s]\r\n", key.c_str(), val.c_str());
	}

}

bool Configuration::_getTokens(const _tstring &line, _tstring &key, _tstring &val)
{
	//clean leading spaces
	_tstring::const_iterator itb=line.begin();
	for(; itb != line.end(); itb++) {
		if( *itb != ' ' ) break;
	}

	_tstring::const_iterator its=itb;

	if( its != line.end() && *its == '#' ) return false;

	for(; itb != line.end(); itb++) {
		if( *itb == '=' ) break;
	}
	key.assign(its, itb);

	if( key.empty() ) return false;
	
	itb++;
	for(; itb != line.end(); itb++) {
		if( *itb != ' ' ) break;
	}

	its=line.end();
	for(its--; its != itb; its--) {
		if( *its != ' ' ) {
			if( its != line.end() ) 
				its++;
			break;
		}
	}

	val.assign(itb, its);

	if( key.empty() || val.empty() ) {
		return false;
	}

	return true;
}

const _tstring *Configuration::get(const _tstring &key)
{
	std::map<_tstring, _tstring>::const_iterator it = _mapConfig.find(key);
	if( it != _mapConfig.end() ) {
		return &(it->second);
	}
	return NULL;
}

_tstring Configuration::getValue(const _tstring &key)
{
	const _tstring *s = get(key);
	if( s == NULL ) return _T("");
	return *s;
}

std::vector< std::pair<_tstring, _tstring> > Configuration::getPrefix(const _tstring &prefix)
{
	std::vector< std::pair<_tstring, _tstring> > ret;
	for (auto &item : _mapConfig) {
		auto &k = item.first;
		if (k.find(prefix) == 0) {
			ret.push_back(std::make_pair(item.first, item.second));
		}
	}

	return ret;
}
