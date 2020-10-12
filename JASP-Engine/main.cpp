//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "engine.h"
#include "timers.h"
#include "log.h"
#include <fstream>
#include <boost/nowide/args.hpp>
#include <boost/filesystem.hpp>
#include <codecvt>

#ifdef _WIN32
void openConsoleOutput(unsigned long slaveNo, unsigned parentPID)
{

		FreeConsole();

		// create a separate new console window
		AllocConsole();

		// attach the new console to this application's process
		AttachConsole(GetCurrentProcessId());

		static std::string title = "jaspEngine " + std::to_string(slaveNo) + " for parent JASP at PID " + std::to_string(parentPID);
		SetConsoleTitleA(title.c_str());

		// reopen the std I/O streams to redirect I/O to the new console
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
}
#endif

/* THe following code might be useful if we ever want to get a bunch of utf16 from windows instead of local MBCS codepage crap
 #ifdef _WIN32
int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
{
	if(argc > 4)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strCvt;
		
		unsigned long	slaveNo			= wcstoul(argv[1], NULL, 10),
						parentPID		= wcstoul(argv[2], NULL, 10);
		std::string		logFileBase		= strCvt.to_bytes(argv[3]),
						logFileWhere	= strCvt.to_bytes(argv[4]);	
			
#else*/
int main(int argc, char *argv[])
{
	if(argc > 4)
	{
		unsigned long	slaveNo			= strtoul(argv[1], NULL, 10),
						parentPID		= strtoul(argv[2], NULL, 10);
		std::string		logFileBase		= argv[3],
						logFileWhere	= argv[4];

		Log::logFileNameBase = logFileBase;
		Log::initRedirects();
		Log::setLogFileName(logFileBase + " Engine " + std::to_string(slaveNo) + ".log");
		Log::setWhere(logTypeFromString(logFileWhere));
		Log::setEngineNo(slaveNo);

		Log::log() << "Log and possible redirects initialized!" << std::endl;
		Log::log() << "jaspEngine started and has slaveNo " << slaveNo << " and it's parent PID is " << parentPID << std::endl;

		//Should help with converting korean/japanese/etc to utf-8.
		boost::filesystem::path::imbue(std::locale( std::locale(), new std::codecvt_utf8_utf16<wchar_t>() ) );

		try
		{
			JASPTIMER_START(Engine Starting);
			Engine e(slaveNo, parentPID);
			JASPTIMER_STOP(Engine Starting);

			e.run();

		}
		catch (std::exception & e)
		{
			Log::log() << "Engine had an uncaught exception of: " << e.what() << "\n";
			throw e;
		}

		JASPTIMER_PRINTALL();

		Log::log() << "jaspEngine " << slaveNo << " child of " << parentPID << " stops." << std::endl;
		exit(0);
	}

	std::cout << "Engine started in testing mode because it didn't receive 4 arguments." << std::endl;

	const char * testFileName = "testFile.txt";
	std::ofstream writeTestFile(testFileName);

	std::cout << "Opening testfile \"" << testFileName << "\" " << (writeTestFile.is_open() ? "Succeeded!" : "Failed!") << std::endl;

	if(writeTestFile.is_open())
	{
		writeTestFile << "Hello beautiful world!" << std::endl;

		std::cout << "After writing something into the testfile the state of the outputstream is: " << (writeTestFile.good() ? "good" : "bad") << "." << std::endl;

		writeTestFile.close();
	}

	std::cout << "Thank you for helping us make JASP better!" << std::endl;

	exit(1);
}
