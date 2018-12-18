//
// Copyright (C) 2018 University of Amsterdam
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

#ifndef BACKSTAGEPAGE_H
#define BACKSTAGEPAGE_H

#include <QObject>

#include "data/fileevent.h"

class BackstagePage : public QObject
{
	Q_OBJECT
public:
	explicit BackstagePage(QObject *parent = 0);

	virtual void setMode(FileEvent::FileMode mode);

signals:
	void dataSetIORequest(FileEvent *event);

	void closeDataSetSelected();
	void exportSelected(QString filename);

protected:
	FileEvent::FileMode _mode;

};

#endif // BACKSTAGEPAGE_H
