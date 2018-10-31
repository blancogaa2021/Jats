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

#ifndef OPENWIDGET_H
#define OPENWIDGET_H

#include <QWidget>

#include <QWebEngineView>
#include <QFileSystemWatcher>

#include "verticaltabwidget.h"
#include "fsbmrecentfiles.h"
#include "fsbmcurrentfile.h"
#include "fsbmdatalibrary.h"
#include "fsbrowser.h"

#include "backstagecomputer.h"
#include "backstageosf.h"
#include "backstagedatalibrary.h"
#include "backstagerecentfiles.h"
#include "backstagecurrentfile.h"

#include "fileevent.h"

namespace Ui {
class BackstageForm;
}

class OpenSaveWidget : public QWidget
{
	Q_OBJECT
public:
	enum FileLocation {Recent = 0, Current, Computer, OSF, Examples};

	explicit OpenSaveWidget(QWidget *parent = 0);

	VerticalTabWidget *tabWidget();
	void setSaveMode(FileEvent::FileMode mode);
	void setOnlineDataManager(OnlineDataManager *odm);
	bool changeTabIfCurrentFileEmpty();

	FileEvent* open();
	FileEvent* open(const QString &path);
	FileEvent* save();
	void sync();
	FileEvent *close();

	void setCurrentDataFile(const QString &path);
	void setDataFileWatcher(bool watch);

	Utils::FileType getCurrentFileType();
	QString getCurrentFilePath();
	bool isCurrentFileReadOnly();
	QString getDefaultOutFileName();

public slots:
	void dataSetIOCompleted(FileEvent *event);

signals:
	void dataSetIORequest(FileEvent *event);

private slots:
	void dataSetIORequestHandler(FileEvent *event);
	void dataSetOpenRequestHandler(QString path);
	void dataSetOpenExampleRequestHandler(QString path);
	void dataSetOpenCurrentRequestHandler(QString path);
	void dataFileModifiedHandler(QString path);
	void tabWidgetChanged(int id);
	void tabWidgetChanging(int index, bool &cancel);

private:
	bool checkSyncFileExists(const QString &path);
	void clearSyncData();

	static bool clearOSFFromRecentList(QString path);
	OnlineDataManager *_odm = NULL;

	QString _currentFilePath;
	Utils::FileType _currentFileType;
	bool _currentFileReadOnly;

	FileEvent::FileMode _mode;

	VerticalTabWidget *_tabWidget;
	Ui::BackstageForm *ui;
		
	BackstageRecentFiles *_bsRecentFiles;
	BackstageCurrentFile *_bsCurrentFile;
	BackstageComputer *_bsComputer;
	BackstageOSF *_bsOSF;
	BackstageDataLibrary *_bsDataLibrary;
	
	QFileSystemWatcher _watcher;
};

#endif // OPENWIDGET_H
