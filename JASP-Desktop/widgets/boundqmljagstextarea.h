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

#ifndef BOUNDQMLJAGSTEXTAREA_H
#define BOUNDQMLJAGSTEXTAREA_H

#include "boundqmltextarea.h"
#include "analysis/options/options.h"

class BoundQMLJAGSTextArea : public BoundQMLTextArea
{
public:
	BoundQMLJAGSTextArea(JASPControlBase* item);

	void	bindTo(Option *option)						override;
	Option* createOption()								override;
	bool	isOptionValid(Option* option)				override;
	bool	isJsonValid(const Json::Value& optionValue) override;

	Option* boundTo()									override { return _options; }

public slots:
	void	checkSyntax()								override;

private:
	Options*					_options = nullptr;
	QSet<QString>				_usedColumnNames;
};

#endif // BOUNDQMLJAGSTEXTAREA_H
