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

#include "datasettablemodel.h"

#include <iostream>
#include <fstream>

#include <QSize>
#include <QDebug>
#include <QQmlEngine>

#include "qutils.h"

using namespace std;

DataSetTableModel::DataSetTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
	_dataSet = NULL;

	_nominalTextIcon	= QIcon(":/icons/variable-nominal-text.svg");
	_nominalIcon		= QIcon(":/icons/variable-nominal.svg");
	_ordinalIcon		= QIcon(":/icons/variable-ordinal.svg");
	_scaleIcon			= QIcon(":/icons/variable-scale.svg");
}

QVariant DataSetTableModel::getColumnTypesWithCorrespondingIcon()
{
	QVariantList ColumnTypeAndIcons;

	//enum ColumnType { ColumnTypeUnknown = 0, ColumnTypeNominal = 1, ColumnTypeNominalText = 2, ColumnTypeOrdinal = 4, ColumnTypeScale = 8 };

	ColumnTypeAndIcons.push_back(QVariant(QString("")));
	ColumnTypeAndIcons.push_back(QVariant(QString("../icons/variable-nominal.svg")));
	ColumnTypeAndIcons.push_back(QVariant(QString("../icons/variable-nominal-text.svg")));
	ColumnTypeAndIcons.push_back(QVariant(QString("")));
	ColumnTypeAndIcons.push_back(QVariant(QString("../icons/variable-ordinal.svg")));
	for(int i=0; i<3;i++)
		ColumnTypeAndIcons.push_back(QVariant(QString("")));

	ColumnTypeAndIcons.push_back(QVariant(QString("../icons/variable-scale.svg")));


	return QVariant(ColumnTypeAndIcons);
}

void DataSetTableModel::setDataSet(DataSet* dataSet)
{
    beginResetModel();
	_dataSet = dataSet;
    endResetModel();
	notifyColumnFilterStatusChanged();
}


int DataSetTableModel::rowCount(const QModelIndex &parent) const
{
	if (_dataSet == NULL)
		return 0;

	return parent.isValid() ? 0 : _dataSet->rowCount();
}

int DataSetTableModel::columnCount(const QModelIndex &parent) const
{
	if (_dataSet == NULL)
		return 0;

	return parent.isValid() ? 0 : _dataSet->columnCount();
}

QVariant DataSetTableModel::data(const QModelIndex &index, int role) const
{
	if (_dataSet == NULL)
		return QVariant();

	int column = index.column();


	if(column > -1 && column < columnCount())
	{
		if(role == Qt::DisplayRole)
			return tq(_dataSet->column(column)[index.row()]);
		else if(role == (int)specialRoles::active)
			return getRowFilter(index.row());
		else if(role == (int)specialRoles::lines)
		{
			bool iAmActive = getRowFilter(index.row()).toBool();

			bool left = iAmActive, right = iAmActive && index.column() == columnCount() - 1; //always draw left line and right line only if last col
			bool up = iAmActive || (index.row() > 0 && data(this->index(index.row() - 1, index.column()), (int)specialRoles::active).toBool()); //draw upper line if i am active or if not when my upstairs neighbour is active
			bool down = (iAmActive && index.row() == rowCount() - 1) || (!iAmActive && index.row() < rowCount() - 1 && data(this->index(index.row() + 1, index.column()), (int)specialRoles::active).toBool()); //draw down line only if i am the last row or if i am inactive and my downstairs neighbour is active

			return	(left ?		1 : 0) +
					(right ?	2 : 0) +
					(up ?		4 : 0) +
					(down ?		8 : 0);
		}


	}

    return QVariant();
}

QVariant DataSetTableModel::columnTitle(int column) const
{
	if(column >= 0 && column < _dataSet->columnCount())
	{
		QString value = tq(_dataSet->column(column).name());
		return QVariant(value);
	}
	else
		return QVariant();
}

QVariant DataSetTableModel::columnIcon(int column) const
{
	if(column >= 0 && column < _dataSet->columnCount())
	{
		Column &columnref = _dataSet->column(column);
		return QVariant(columnref.columnType());
	}
	else
		return QVariant(-1);
}

QVariant DataSetTableModel::columnHasFilter(int column) const
{
	if(_dataSet != NULL && column >= 0 && column < _dataSet->columnCount())
	{
		QString value = tq(_dataSet->column(column).name());
		return QVariant(_dataSet->column(column).hasFilter());
	}
	else
		return QVariant(false);
}

int DataSetTableModel::columnsFilteredCount()
{
	if(_dataSet == NULL) return 0;

	int colsFiltered = 0;

	for(int column=0; column<_dataSet->columnCount(); column++)
		if(_dataSet->column(column).hasFilter())
			colsFiltered++;

	return colsFiltered;
}

void DataSetTableModel::resetAllFilters()
{
	for(int col=0; col<_dataSet->columnCount(); col++)
		_dataSet->column(col).resetFilter();

	emit allFiltersReset();
	notifyColumnFilterStatusChanged();

}

int DataSetTableModel::getMaximumColumnWidthInCharacters(int columnIndex) const
{
	if(columnIndex < 0 || columnIndex >= _dataSet->columnCount()) return 0;

	Column & col = _dataSet->column(columnIndex);

	int extraPad = 2;

	switch(col.columnType())
	{
	case Column::ColumnTypeScale:
		return 6 + extraPad; //default precision of stringstream is 6 (and sstream is used in displaying scale values) + some padding because of dots and whatnot

	case Column::ColumnTypeUnknown:
		return 0;

	default:
	{
		int tempVal = 0;

		for(int labelIndex=0; labelIndex < col.labels().size(); labelIndex++)
			tempVal = std::max(tempVal, (int)col.labels().getLabelFromRow(labelIndex).length());

		return tempVal + extraPad;
	}
	}

}

QVariant DataSetTableModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	if (_dataSet == NULL)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			QString value = tq(_dataSet->column(section).name());
			return QVariant(value);
		}
		else
		{
			return QVariant(section + 1);
		}
	}
	else if(role == (int)specialRoles::maxColString) //A query from DataSetView for the maximumlength string to be expected! This to accomodate columnwidth
	{
		//calculate some maximum string?
		QString dummyText = headerData(section, orientation, Qt::DisplayRole).toString() + "XXXX"; //Bit of padding for filtersymbol and columnIcon
		int colWidth = getMaximumColumnWidthInCharacters(section);

		while(colWidth > dummyText.length())
			dummyText += "X";

		return dummyText;
	}
	else if (role == Qt::TextAlignmentRole)
	{
		return QVariant(Qt::AlignCenter);
	}

	return QVariant();
}

QHash<int, QByteArray> DataSetTableModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractTableModel::roleNames ();


	roles[(int)specialRoles::active]		= QString("active").toUtf8();
	roles[(int)specialRoles::lines]			= QString("lines").toUtf8();
	roles[(int)specialRoles::maxColString]	= QString("maxColString").toUtf8();

	return roles;
}


bool DataSetTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	/*if (_dataSet == NULL)
		return false;

	bool ok;

	Column &column = _dataSet->columns()[index.column()];
	if (column.dataType() == Column::DataTypeInt)
	{
		int v = value.toInt(&ok);
		if (ok)
			column.setValue(index.row(), v);
		else
			emit badDataEntered(index);

		return ok;
	}*/

	//_dataSet->columns()[index.column()].setValue(index.row(), v);

	return true;
}

Qt::ItemFlags DataSetTableModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool DataSetTableModel::setColumnType(int columnIndex, Column::ColumnType newColumnType)
{
	if (_dataSet == NULL)
		return true;

	bool changed = _dataSet->column(columnIndex).changeColumnType(newColumnType);
	emit headerDataChanged(Qt::Horizontal, columnIndex, columnIndex);

	return changed;
}

Column::ColumnType DataSetTableModel::getColumnType(int columnIndex)
{
	return _dataSet->column(columnIndex).columnType();
}

void DataSetTableModel::refreshColumn(Column * column)
{
	for(int col=0; col<_dataSet->columns().size(); col++)
		if(&(_dataSet->columns()[col]) == column)
			emit dataChanged(index(0, col), index(rowCount()-1, col));
}

int DataSetTableModel::setColumnTypeFromQML(int columnIndex, int newColumnType)
{
	setColumnType(columnIndex, (Column::ColumnType)newColumnType);
	emit headerDataChanged(Qt::Orientation::Horizontal, columnIndex, columnIndex);
	return getColumnType(columnIndex);

}
