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

#include "rowcontrols.h"
#include "analysis/analysisform.h"
#include "analysis/jaspcontrolbase.h"

#include "log.h"

#include <QQmlContext>

// Cannot do this code in the constructor: the Component create function will call the addJASPControl method (JASPControlBase, en ListView),
// but to call the ListView needs to have already the instance of the RowControls to be able to call addJASPControl.
void RowControls::init(int row, const QString& key, bool isNew)
{
	QMLListView* listView = _parentModel->listView();
	int col = 0;
	for (QQmlComponent* comp : _rowComponents)
	{
		QQmlContext* context = new QQmlContext(qmlContext(listView->item()), listView->item());
		if (_isDummy)
			context->setContextProperty("noDirectSetup", true);
		context->setContextProperty("hasContextForm", true);
		context->setContextProperty("form", listView->form());
		context->setContextProperty("listView", listView);
		context->setContextProperty("colIndex", col);
		context->setContextProperty("isNew", isNew);
		context->setContextProperty("fromRowComponents", _rowControlsVarMap);
		context->setContextProperty("rowIndex",	row);
		context->setContextProperty("rowValue", key);

		QQuickItem* obj = qobject_cast<QQuickItem*>(comp->create(context));

		if (obj)
		{
			_contextMap[obj] = context;
			_rowObjects.push_back(QVariant::fromValue(obj));
		}
		else
			Log::log() << "Could not create control in ListView " << listView->name() << std::endl;

		col++;
	}
}

void RowControls::setContext(int row, const QString &key)
{
	for (auto & itemContext : _contextMap)
	{
		// Cannot use qmlContext(item) : setContextProperty would generate: 'Cannot set property on internal context.' error
		itemContext.second->setContextProperty("rowIndex",	row);
		itemContext.second->setContextProperty("rowValue", key);
		itemContext.second->setContextProperty("isNew", false);
		itemContext.first->setParentItem(nullptr);
	}
}

bool RowControls::addJASPControl(JASPControlWrapper *control)
{
	bool success = false;
	QMLListView* listView = _parentModel->listView();

	if (!control->isBound())
		success = true;
	else if (control->name().isEmpty())
		listView->addControlError(tr("A row component in %1 does not have a name").arg(listView->name()));
	else if (_rowControlsVarMap.contains(control->name()))
		listView->addControlError(tr("2 row components in %1 have the same name").arg(listView->name()).arg(control->name()));
	else
	{
		QQmlContext* context = qmlContext(control->item());
		bool isDummy = context->contextProperty("noDirectSetup").toBool();

		_rowControlsVarMap[control->name()] = QVariant::fromValue(control->item());
		_rowJASPWrapperMap[control->name()] = control;
		BoundQMLItem* boundItem = dynamic_cast<BoundQMLItem*>(control);
		if (boundItem && !isDummy)
		{
			Option* option = _rowOptions.contains(boundItem->name()) ? _rowOptions[boundItem->name()] : boundItem->createOption();
			boundItem->bindTo(option);
		}

		success = true;
	}

	return success;
}
