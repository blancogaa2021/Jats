#include "tablemodelvariablesoptions.h"

#include <boost/foreach.hpp>
#include <sstream>

#include <QSize>

#include "options/optionlist.h"
#include "options/optionterms.h"

#include "utils.h"

using namespace std;

TableModelVariablesOptions::TableModelVariablesOptions(QObject *parent) :
	QAbstractTableModel(parent)
{
	_boundTo = NULL;
}

int TableModelVariablesOptions::rowCount(const QModelIndex &) const
{
	if (_boundTo == NULL)
		return 0;

	return _rows.size();
}

int TableModelVariablesOptions::columnCount(const QModelIndex &parent) const
{
	if (_boundTo == NULL)
		return 0;

	return _boundTo->rowTemplate()->size();
}

QVariant TableModelVariablesOptions::data(const QModelIndex &index, int role) const
{
	if (_boundTo == NULL)
		return QVariant();

	if (index.column() == 0)
	{
		if (role == Qt::DisplayRole)
		{	
			OptionTerms *option = static_cast<OptionTerms *>(_rows.at(index.row())->get(0));
			return Term(option->value().front()).asQString();
		}
		else
		{
			return QVariant();
		}
	}
	else
	{
		if (role != Qt::DisplayRole && role != Qt::EditRole)
			return QVariant();

		Option* option = _rows.at(index.row())->get(index.column());
		OptionList *list = dynamic_cast<OptionList *>(option);
		if (list == NULL)
			return QVariant("WTF");

		QString selected = tq(list->value());

		if (role == Qt::DisplayRole)
			return selected;

		QStringList items = tql(list->options());

		QList<QVariant> value;
		value.append(selected);
		value.append(items);

		return value;
	}

}

bool TableModelVariablesOptions::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (_boundTo == NULL)
		return false;

	Options *row = _rows.at(index.row());
	Option  *cell = row->get(index.column());
	OptionList *list = dynamic_cast<OptionList *>(cell);

	if (list == NULL)
		return false;

	string oldValue = list->value();
	string newValue = fq(value.toString());

	if (oldValue != newValue)
	{
		list->setValue(newValue);
		emit dataChanged(index, index);
		_boundTo->setValue(_rows);
	}

	return true;
}

Qt::ItemFlags TableModelVariablesOptions::flags(const QModelIndex &index) const
{
	if (_boundTo == NULL)
		return 0;

	if (index.column() == 0)
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant TableModelVariablesOptions::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (_boundTo == NULL)
		return QVariant();

	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		string name;
		Option *option;
		_boundTo->rowTemplate()->get(section, name, option);
		return tq(name);
	}

	if (role == Qt::SizeHintRole && orientation == Qt::Horizontal)
	{
		if (section == 0)
			return QSize(200, -1);
		else
			return QSize(80, -1);
	}

	return QVariant();
}

void TableModelVariablesOptions::setVariables(const Terms &variables)
{
	_variables = variables;

	if (_boundTo == NULL)
		return;

	beginResetModel();

	_rows.clear();

	BOOST_FOREACH(const Term &term, variables)
	{
		Options *row = static_cast<Options *>(_boundTo->rowTemplate()->clone());
		OptionTerms *termCell = static_cast<OptionTerms *>(row->get(0));
		termCell->setValue(term.scomponents());

		_rows.push_back(row);
	}

	endResetModel();

	_boundTo->setValue(_rows);
}

const Terms &TableModelVariablesOptions::variables() const
{
	return _variables;
}

void TableModelVariablesOptions::bindTo(Option *option)
{
	_boundTo = dynamic_cast<OptionsTable *>(option);

	if (_boundTo != NULL)
	{
		beginResetModel();
		_rows = _boundTo->value();
		endResetModel();
	}
}
