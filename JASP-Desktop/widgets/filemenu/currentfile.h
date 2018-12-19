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


#ifndef CURRENTFILE_H
#define CURRENTFILE_H

#include "filemenuobject.h"
#include "currentfilelistmodel.h"
#include <QQuickWidget>

class CurrentFile : public FileMenuObject
{
	Q_OBJECT
	Q_PROPERTY(CurrentFileListModel * listModel READ listModel WRITE setListModel NOTIFY listModelChanged)
	
public:
	explicit CurrentFile(QObject *parent = nullptr);
	~CurrentFile();
	
	void setCurrentFilePath(const QString &path);
	void setCurrentDataFilePath(const QString &path);
    void setCurrentFileInfo(const QString &path, const Utils::FileType &type, const bool & readonly);
	void setCurrentFileType(const Utils::FileType &type);
	void setCurrentFileReadOnly(const bool & readonly);
	
	Utils::FileType getCurrentFileType();	
	bool isCurrentFileReadOnly();
	bool isOnlineFile(const QString &path);
		
	//Special
	CurrentFileListModel * getCurrentFileListModel();

	CurrentFileListModel * listModel() const { return _currentFileListModel; }

public slots:
	QString getCurrentFilePath();
	QString getCurrentDataFilePath();
	QString getCurrentDataFileName();
	QString getCurrentDataFolder();
	QString getHeaderText();		
	void syncFile(FileEvent *event);
	
	void setListModel(CurrentFileListModel * listModel);
signals:
	void listModelChanged(CurrentFileListModel * listModel);

private:
	CurrentFileListModel *_currentFileListModel = nullptr;
	
	QString _currentFilePath,
			_currentDataFilePath;

	Utils::FileType _currentFileType;
	bool _currentFileReadOnly;

};

#endif // CURRENTFILE_H
