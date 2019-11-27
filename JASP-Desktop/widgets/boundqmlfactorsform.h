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

#ifndef BOUNDQMLFACTORSFORM_H
#define BOUNDQMLFACTORSFORM_H

#include "analysis/boundqmlitem.h"
#include "listmodelfactorsform.h"
#include "qmllistview.h"
#include "analysis/options/optionstable.h"




class BoundQMLFactorsForm :  public QMLListView, public BoundQMLItem
{
	Q_OBJECT
	
public:
	BoundQMLFactorsForm(JASPControlBase* item);

	ListModel*	model()										override { return _factorsModel; }
	Option*		boundTo()									override { return _boundTo; }
	void		bindTo(Option *option)						override;
	Option*		createOption()								override;
	bool		isOptionValid(Option* option)				override;
	bool		isJsonValid(const Json::Value& optionValue) override;

protected slots:
	void modelChangedHandler() override;
	void addListViewSlot(BoundQMLListViewTerms* listView);
	
private:
	ListModelFactorsForm*	_factorsModel;
	OptionsTable*			_boundTo;
	QString					_availableVariablesListName;
	JASPControlBase*		_availableVariablesListItem;
	int						_initNumberFactors;
};

#endif // BOUNDQMLFACTORSFORM_H
