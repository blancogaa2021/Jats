//
// Copyright (C) 2013-2021 University of Amsterdam
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

#include "boundcontroltableview.h"
#include "tableviewbase.h"
#include "listmodeltableviewbase.h"

BoundControlTableView::BoundControlTableView(TableViewBase* tableView)
	: BoundControlBase(tableView), _tableView(tableView)
{
}

bool BoundControlTableView::isJsonValid(const Json::Value &value)
{
	return value.isArray();
}

Json::Value BoundControlTableView::createJson()
{
	Json::Value result(Json::arrayValue);

	QVariant modelVar = _tableView->property("values");
	QList<QVariant> list = modelVar.toList();

	Json::Value levels(Json::arrayValue);
	Json::Value values(Json::arrayValue);

	for (const QVariant& itemVariant : list)
	{
		levels.append(fq(itemVariant.toString()));
		values.append(fq(_tableView->defaultEmptyValue()));
	}

	for (int row=list.length(); row < _tableView->initialRowCount(); row++)
	{
		levels.append(fq(_tableView->tableModel()->getDefaultRowName(size_t(row))));
		values.append(fq(_tableView->defaultEmptyValue()));
	}

	for (int i = 0; i < _tableView->initialColumnCount(); i++)
	{
		Json::Value row(Json::objectValue);
		row["levels"] = levels;
		row["name"] = fq(_tableView->tableModel()->getDefaultColName(size_t(i)));
		row["values"] = values;
		result.append(row);
	}

	return result;
}

void BoundControlTableView::fillTableTerms(const Json::Value &value, ListModelTableViewBase::TableTerms &tableTerms)
{
	int index = 0;

	for (const Json::Value& row : value)
	{
		tableTerms.colNames.push_back(tq(row["name"].asString()));
		if (index == 0)
		{
			for (const Json::Value& level : row["levels"])
				tableTerms.rowNames.push_back(tq(level.asString()));
		}
		tableTerms.values.push_back({});
		for (const Json::Value& value : row["values"])
		{
			QVariant val;
			if (value.isInt())			val = value.asInt();
			else if (value.isDouble())	val = value.asDouble();
			else if (value.isString())	val = tq(value.asString());
			tableTerms.values[tableTerms.values.size() - 1].push_back(val);
		}

		if (_tableView->tableModel()->isRCodeColumn(index))
			setIsRCode("values");
		index++;
	}

}

void BoundControlTableView::bindTo(const Json::Value &value)
{
	BoundControlBase::bindTo(value);

	ListModelTableViewBase::TableTerms tableTerms;
	fillTableTerms(value, tableTerms);

	_tableView->tableModel()->initTableTerms(tableTerms);
}

void BoundControlTableView::fillBoundValue(Json::Value &boundValue, const  ListModelTableViewBase::TableTerms &tableTerms)
{
	Json::Value levels(Json::arrayValue);
	for (const QString& rowName : tableTerms.rowNames)
		levels.append(fq(rowName));

	for (int colIndex = 0; colIndex < tableTerms.colNames.size(); colIndex++)
	{
		Json::Value row(Json::objectValue);
		row["name"] = fq(tableTerms.colNames[colIndex]);
		row["levels"] = levels;

		Json::Value values(Json::arrayValue);
		for (const QVariant& val : tableTerms.values[colIndex])
		{
			if (val.type() == QVariant::Int)			values.append(val.toInt());
			else if (val.type() == QVariant::Double)	values.append(val.toDouble());
			else										values.append(fq(val.toString()));
		}
		row["values"] = values;

		boundValue.append(row);
	}
}

void BoundControlTableView::updateOption()
{
	Json::Value boundValue(Json::arrayValue);
	const ListModelTableViewBase::TableTerms& tableTerms = _tableView->tableModel()->tableTerms();

	fillBoundValue(boundValue, tableTerms);

	setBoundValue(boundValue);
}
