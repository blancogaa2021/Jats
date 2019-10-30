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

#include "option.h"
#include "../analysis.h"

using namespace std;

void Option::blockSignals(bool block, bool notifyOnceUnblocked)
{
	if (block)
	{
		_signalsBlocked++;
	}
	else
	{
		_signalsBlocked--;
		if (_signalsBlocked < 0)
			_signalsBlocked = 0;

		if (_signalsBlocked == 0 && notifyOnceUnblocked && _shouldSignalChangedOnceUnblocked)
		{
			changed(this);
			_shouldSignalChangedOnceUnblocked = false;
		}
	}
}

bool Option::isTransient() const
{
	return _isTransient;
}

void Option::notifyChanged()
{
	if (_signalsBlocked)
		_shouldSignalChangedOnceUnblocked = true;
	else
		changed(this);
}

Json::Value Option::defaultMetaEntryContainingColumn() const
{
	Json::Value meta		= Json::objectValue;
	meta["containsColumn"]	= true;

	return meta;
}


