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

#ifndef BOUNDQMLREPEATEDMEASURESFACTORS_H
#define BOUNDQMLREPEATEDMEASURESFACTORS_H

#include "analysis/boundqmlitem.h"
#include "listmodelrepeatedmeasuresfactors.h"
#include "qmllistview.h"
#include "analysis/options/optionstable.h"

class BoundQMLRepeatedMeasuresFactors :  public QMLListView, public BoundQMLItem
{
	Q_OBJECT
	
public:
	BoundQMLRepeatedMeasuresFactors(QQuickItem* item, AnalysisForm* form);	

	virtual ListModel* model() OVERRIDE		{ return _factorsModel; }
	virtual Option* boundTo() OVERRIDE		{ return _boundTo; }
	
	virtual void bindTo(Option *option) OVERRIDE;
	virtual Option* createOption() OVERRIDE;
	virtual bool isOptionValid(Option* option) OVERRIDE;	

protected slots:
	virtual void modelChangedHandler() OVERRIDE;
	
private:
	ListModelRepeatedMeasuresFactors* _factorsModel;
	OptionsTable* _boundTo;
	
};

#endif // BOUNDQMLREPEATEDMEASURESFACTORS_H
