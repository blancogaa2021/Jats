//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//


#ifndef ANALYSISENTRY_H
#define ANALYSISENTRY_H

#include <string>
#include <vector>
#include "jsonredirect.h"

namespace Modules
{
class DynamicModule;

class AnalysisEntry
{
public:
	AnalysisEntry(Json::Value & analysisEntry, DynamicModule * dynamicModule);
	AnalysisEntry();

	std::string		title()					const { return _title;				}
	std::string		function()				const { return _function;			}
	std::string		qml()					const { return _qml;				}
	std::string		icon()					const;
	bool			isSeparator()			const { return _isSeparator;		}
	bool			isGroupTitle()			const { return _isGroupTitle;		}
	bool			isAnalysis()			const { return _isAnalysis;			}
	bool			isEnabled()				const { return _isEnabled;			}

	DynamicModule*	dynamicModule()			const;
	std::string		qmlFilePath()			const;
	std::string		getFullRCall()			const;
	Json::Value		getDefaultResults()		const;
	Json::Value		asJsonForJaspFile()		const;

	std::string		codedReference()		const;
	std::string		buttonMenuString()		const;

private:
	std::string		_title		= "",
					_function	= "",
					_qml		= "";
	DynamicModule*	_dynamicModule = nullptr;
	bool			_isSeparator = false;
	bool			_isGroupTitle = false;
	bool			_isAnalysis = false;
	bool			_isEnabled = true;
	std::string		_icon		= "";

};

typedef std::vector<AnalysisEntry*> AnalysisEntries;

}

#endif // ANALYSISENTRY_H
