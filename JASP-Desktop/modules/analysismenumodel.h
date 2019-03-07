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

#ifndef ANALYSISMENUMODEL_H
#define ANALYSISMENUMODEL_H

#include <QAbstractListModel>
#include <QStringList>

#include "modules/analysisentry.h"


class AnalysisMenuModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum {
		DisplayRole,
		AnalysisFunctionRole,
		MenuImageSourceRole
	};

	AnalysisMenuModel(QObject *parent) : QAbstractListModel(parent) {}

	int										rowCount(const QModelIndex &parent = QModelIndex())			const override	{	return _analysisEntries.size();	}
	QVariant								data(const QModelIndex &index, int role = Qt::DisplayRole)	const override;
	virtual QHash<int, QByteArray>			roleNames()													const override;

	void 									setAnalysisEntries(std::vector<Modules::AnalysisEntry*> analysisEntries)	{	_analysisEntries = analysisEntries;	}
	std::vector<Modules::AnalysisEntry*>	getAnalysisEntries()														{	return _analysisEntries;			}
	Modules::AnalysisEntry*					getAnalysisEntry(const std::string& name);

	Q_INVOKABLE QString						getFirstAnalysisName()														{	return QString::fromStdString(_analysisEntries.at(0)->function());	}
	Q_INVOKABLE QString						getFirstAnalysisTitle()														{	return QString::fromStdString(_analysisEntries.at(0)->title());	}
	Q_INVOKABLE QString						getAnalysisName(int index)									const			{	return QString::fromStdString(_analysisEntries.at(index)->function());	}
	Q_INVOKABLE QString						getAnalysisTitle(int index)									const			{	return QString::fromStdString(_analysisEntries.at(index)->title());	}

private:
	Modules::AnalysisEntries _analysisEntries;
};

typedef std::vector<AnalysisMenuModel*> AnalysisMenuModels;

#endif
