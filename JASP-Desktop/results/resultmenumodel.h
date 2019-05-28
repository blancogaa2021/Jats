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

#ifndef RESULTMENUMODEL_H
#define RESULTMENUMODEL_H

#include <QAbstractListModel>
#include <QStringList>

#include "utilities/jsonutilities.h"
#include "results/resultmenuentry.h"


class ResultMenuModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum {
		DisplayRole,
		NameRole,
		MenuImageSourceRole,
		JSFunctionRole,
		IsSeparatorRole,
		IsEnabledRole
	};

	ResultMenuModel(QObject *parent) : QAbstractListModel(parent) {}

	int										rowCount(const QModelIndex &parent = QModelIndex())			const override	{	return _resultMenuEntries.size();	}
	QVariant								data(const QModelIndex &index, int role = Qt::DisplayRole)	const override;
	virtual QHash<int, QByteArray>			roleNames()													const override;

	Q_INVOKABLE void						setOptions(QString, QStringList);
	Q_INVOKABLE QString						getJSFunction(int index)									const			{	return _resultMenuEntries.at(index).jsFunction();	}
	Q_INVOKABLE QString						getName(int index)											const			{	return _resultMenuEntries.at(index).name();			}

private:
	std::vector<ResultMenuEntry>			_resultMenuEntries;
};

#endif  // RESULTMENUMODEL_H
