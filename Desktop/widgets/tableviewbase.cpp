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

#include "log.h"
#include <QTimer>
#include "tableviewbase.h"
#include "listmodeljagsdatainput.h"
#include "../analysis/analysisform.h"
#include "listmodelfiltereddataentry.h"
#include "listmodelmultinomialchi2test.h"
#include "listmodelrepeatedmeasuresfactors.h"
#include "listmodelcustomcontrasts.h"
#include "boundcontrolcontraststableview.h"
#include "boundcontrolfilteredtableview.h"

TableViewBase::TableViewBase(QQuickItem* parent)
	: JASPListControl(parent)
{
	_controlType = ControlType::TableView;
}

void TableViewBase::setUpModel()
{
	QString tableType	= property("tableType").toString(),
			itemType	= property("itemType").toString();

	switch (modelType())
	{
	case ModelType::MultinomialChi2Model	:
	{
		setInitialColumnCount(1);
		setDefaultEmptyValue("1");
		_tableModel	= new ListModelMultinomialChi2Test(	this, tableType	);
		break;
	}
	case ModelType::JAGSDataInputModel		:
	{
		setDefaultEmptyValue("...");
		_tableModel	= new ListModelJAGSDataInput(		this, tableType	);
		break;
	}
	case ModelType::CustomContrasts			:
	{
		_tableModel = new ListModelCustomContrasts(		this, tableType	);
		setDefaultEmptyValue("0");
		break;
	}
	case ModelType::FilteredDataEntryModel	:	_tableModel = new ListModelFilteredDataEntry(	this, tableType	); break;
	case ModelType::Simple					:
	default									:
	{
		setDefaultEmptyValue("1");
		_tableModel = new ListModelTableViewBase(     this, tableType );
		break;
	}
	}

	JASPListControl::setUpModel();

	_tableModel->setItemType(itemType);

	connect(this, SIGNAL(addColumn()),						this, SLOT(addColumnSlot()));
	connect(this, SIGNAL(removeColumn(int)),				this, SLOT(removeColumnSlot(int)));
	connect(this, SIGNAL(addRow()),							this, SLOT(addRowSlot()));
	connect(this, SIGNAL(removeRow(int)),					this, SLOT(removeRowSlot(int)));
	connect(this, SIGNAL(reset()),							this, SLOT(resetSlot()));
	connect(this, SIGNAL(itemChanged(int, int, QString, QString)),	this, SLOT(itemChangedSlot(int, int, QString, QString)));

	connect(_tableModel, &ListModelTableViewBase::columnCountChanged,	this, &TableViewBase::columnCountChanged);
	connect(_tableModel, &ListModelTableViewBase::rowCountChanged,		this, &TableViewBase::rowCountChanged);
	connect(_tableModel, &ListModelTableViewBase::variableCountChanged,	this, &TableViewBase::variableCountChanged);
}

void TableViewBase::setUp()
{
	switch (modelType())
	{
	case ModelType::CustomContrasts			: _boundControl = new BoundControlContrastsTableView(this); break;
	case ModelType::FilteredDataEntryModel	: _boundControl = new BoundControlFilteredTableView(this); break;
	case ModelType::MultinomialChi2Model	:
	case ModelType::JAGSDataInputModel		:
	case ModelType::Simple					:
	default									: _boundControl = new BoundControlTableView(this);
	}

	JASPListControl::setUp();

	// form is not always known in the constructor, so all references to form (and dataset) must be done here
	connect(form(),		&AnalysisForm::refreshTableViewModels,			this, &TableViewBase::refreshMe	);
	_tableModel->setup();
}

void TableViewBase::addColumnSlot()
{
	if (_tableModel)
		_tableModel->addColumn();
}

void TableViewBase::removeColumnSlot(int col)
{
	if (_tableModel)
		_tableModel->removeColumn(col);
}

void TableViewBase::addRowSlot()
{
	if (_tableModel)
		_tableModel->addRow();
}

void TableViewBase::removeRowSlot(int row)
{
	if (_tableModel)
		_tableModel->removeRow(row);
}

void TableViewBase::resetSlot()
{
	if (_tableModel)
		_tableModel->reset();
}

void TableViewBase::itemChangedSlot(int col, int row, QString value, QString type)
{
	if (_tableModel)
	{
		if (_tableModel->valueOk(value))	_tableModel->itemChanged(col, row, value, type);
		else								QTimer::singleShot(0, _tableModel, &ListModelTableViewBase::refreshModel);
	}
}

void TableViewBase::rScriptDoneHandler(const QString & result)
{
	if(_tableModel)
		_tableModel->rScriptDoneHandler(result);
}

void TableViewBase::refreshMe()
{
	if(_tableModel)
		_tableModel->refreshModel();
}

void TableViewBase::termsChangedHandler()
{
	if (_boundControl)
		_boundControl->updateOption();
}
