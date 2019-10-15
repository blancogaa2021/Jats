#define ENUM_DECLARATION_CPP
#include "log.h"
#include "boost/nowide/cstdio.hpp"
#include <chrono>
#ifdef WIN32
#include <io.h>
#endif

#include "utils.h"
#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/null.hpp"
#include "boost/nowide/fstream.hpp"

typedef boost::nowide::ofstream bofstream; //Use this to work around problems on Windows with utf8 conversion

static bofstream _logFile;// = bofstream();

std::string Log::logFileNameBase	= "";

logType		Log::_where				= logType::cout;
std::string	Log::_logFilePath		= "";
logError	Log::_logError			= logError::noProblem;
int			Log::_stdoutfd			= -1;


logType		Log::_default			=
#ifdef JASP_DEBUG
	logType::cout;
#else
	logType::null;
#endif

void Log::setDefaultDestination(logType newDestination)
{
	if(newDestination == logType::file) //It doesnt make any sense to have the default non-file logType be file...
		newDestination = logType::cout;

	if(_default == newDestination)
		return;

	bool setNewDefaultToWhere = _where == _default;

	_default = newDestination;

	if(setNewDefaultToWhere)
		redirectStdOut();
}

void Log::setWhere(logType where)
{
	if(where == _where)
		return;

	_where = where;

	redirectStdOut();
}

void Log::setLoggingToFile(bool logToFile)
{
	setWhere(logToFile ? logType::file : _default);
}

void Log::setLogFileName(const std::string & filePath)
{
	if(_logFilePath == filePath)
		return;

	_logFilePath = filePath;

	if(_where == logType::file)
		redirectStdOut();
}

void Log::initRedirects()
{
	_where			= _default;
}

void Log::redirectStdOut()
{
	switch(_where)
	{
	default:
		break;

	case logType::file:
	{
		if(_logFilePath == "")
		{
			_logError = logError::filePathNotSet;
			_where    = _default;
			redirectStdOut();
			return;
		}

		//_currentFile = freopen(_logFilePath.c_str(), "a", stdout);
		//if(!_currentFile)

		_logFile.open(_logFilePath.c_str(), std::ios_base::app | std::ios_base::out);

		if(_logFile.fail())
		{
			_logError	= logError::fileNotOpen;
			_where		= _default;
			redirectStdOut();
			return;
		}
		break;
	};
	}

	_logError = logError::noProblem;
}

Json::Value	Log::createLogCfgMsg()
{
	Json::Value json	= Json::objectValue;

	json["where"]		= logTypeToString(_where);

	return json;
}

void Log::parseLogCfgMsg(const Json::Value & json)
{
	setWhere(logTypeFromString(json["where"].asString()));
}

std::string Log::getTimestamp()
{
	static char buf[13];
	static auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	std::chrono::milliseconds duration = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) - startTime;
	long durationMilli = (std::chrono::duration_cast<std::chrono::milliseconds>(duration)).count();
	int milli = durationMilli % 1000;
	long durationSec = durationMilli / 1000;
	int sec = durationSec % 60;
	long durationMin = durationSec / 60;
	int min = durationMin % 60;
	long durationHour = durationMin / 60;
	int hour = durationHour % 60;

	std::sprintf(buf, "%02u:%02u:%02u.%03u", hour, min, sec, milli);
	return buf;
}

std::ostream & Log::log(bool addTimestamp)
{
	switch(_where)
	{
#ifndef __clang__
#ifdef __GNUG__
	default:				//Gcc is stupid and is not aware that the next three cases cover all
#endif
#endif
	case logType::cout:		return std::cout;
	case logType::null:
	{
		static boost::iostreams::stream<boost::iostreams::null_sink> nullstream((boost::iostreams::null_sink())); //https://stackoverflow.com/questions/8243743/is-there-a-null-stdostream-implementation-in-c-or-libraries
		return nullstream;
	}
	case logType::file:
		if (addTimestamp) _logFile << Log::getTimestamp() << ": ";
		return _logFile;
	}
}
