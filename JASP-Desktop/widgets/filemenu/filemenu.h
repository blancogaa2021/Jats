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

#pragma once

#ifndef FILEMENU_H
#define FILEMENU_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QVector>

#include "widgets/filemenu/recentfiles.h"
#include "widgets/filemenu/currentfile.h"
#include "widgets/filemenu/computer.h"
#include "widgets/filemenu/osf.h"
#include "widgets/filemenu/datalibrary.h"

#include "data/fileevent.h"
#include "filemenulistitem.h"
#include "actionbuttons.h"
#include "resourcebuttonsvisible.h"

class MainWindow;

class FileMenu : public QObject
{
	Q_OBJECT

	Q_PROPERTY(ActionButtons::FileOperation fileoperation		READ fileoperation				WRITE setFileoperation				NOTIFY fileoperationChanged		)
	Q_PROPERTY(DataLibrary *  datalibrary						READ datalibrary													NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(CurrentFile * currentFile						READ currentFile													NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(RecentFiles * recentFiles						READ recentFiles													NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(Computer * computer								READ computer														NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(OSF * osf										READ osf															NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(bool visible										READ visible					WRITE setVisible					NOTIFY visibleChanged			)
	Q_PROPERTY(ActionButtons * actionButtons					READ actionButtons													NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(ResourceButtons * resourceButtons				READ resourceButtons												NOTIFY dummyChangedNotifier		)
	Q_PROPERTY(ResourceButtonsVisible * resourceButtonsVisible	READ resourceButtonsVisible											NOTIFY dummyChangedNotifier		)

public:

	enum FileLocation { Recent = 0, Current, ThisComputer, Osf, Examples, CountLocations };

	Q_ENUM(FileMenuListItemType)

	explicit FileMenu(QObject *parent = nullptr);
	virtual ~FileMenu() {}
	
	void		setFileoperation(const ActionButtons::FileOperation fo);

	void		setOnlineDataManager(OnlineDataManager *odm);
	FileEvent *	open(const QString &filepath);
	FileEvent *	save();
	void		sync();
	FileEvent *	close();
			
	void			setCurrentDataFile(const QString &path);
	void			setDataFileWatcher(bool watch);
	
	void			setSaveMode(FileEvent::FileMode mode);
	Utils::FileType getCurrentFileType() const { return _currentFileType; }
	QString			getCurrentFilePath() const { return _currentFilePath; }
	QString			getDefaultOutFileName();
	bool			isCurrentFileReadOnly() const { return _currentFileReadOnly; }

	ActionButtons::FileOperation	fileoperation()				const	{ return _fileoperation;			}
	DataLibrary					*	datalibrary()				const	{ return _DataLibrary;				}
	CurrentFile					*	currentFile()				const	{ return _CurrentFile;				}
	RecentFiles					*	recentFiles()				const	{ return _RecentFiles;				}
	Computer					*	computer()					const	{ return _Computer;					}
	OSF							*	osf()						const	{ return _OSF;						}
	bool							visible()					const	{ return _visible;					}
	ActionButtons				*	actionButtons()				const	{ return _actionButtons;			}
	ResourceButtons				*	resourceButtons()			const	{ return _resourceButtons;			}
	ResourceButtonsVisible		*	resourceButtonsVisible()	const	{ return _resourceButtonsVisible;	}

signals:
	void fileoperationChanged();
	void dataSetIORequest(FileEvent *event);
	void exportSelected(QString filename);
	void visibleChanged(bool visible);
	void dummyChangedNotifier();

public slots:
	void analysisAdded(Analysis *analysis);
	void setSyncFile(FileEvent *event);
	void dataAutoSynchronizationChanged(bool on) { setDataFileWatcher(on); }
	void dataSetIOCompleted(FileEvent *event);
	void dataFileModifiedHandler(QString path);
	void fileOperationClicked(const ActionButtons::FileOperation action);
	void setVisible(bool visible);
	void showFileMenu();
	void resourceButtonClicked(const int buttonType);

private slots:
	void dataSetOpenRequestHandler(QString path) { open(path); }
	void dataSetOpenCurrentRequestHandler(QString path);
	void dataSetIORequestHandler(FileEvent *event);

private:
			bool checkSyncFileExists(const QString &path);
			void clearSyncData();
	static	bool clearOSFFromRecentList(QString path);

private:
	OnlineDataManager			*	_odm						= nullptr;
	CurrentFile					*	_CurrentFile				= nullptr;
	RecentFiles					*	_RecentFiles				= nullptr;
	Computer					*	_Computer					= nullptr;
	OSF							*	_OSF						= nullptr;
	DataLibrary					*	_DataLibrary				= nullptr;
	ActionButtons				*	_actionButtons				= nullptr;
	ResourceButtons				*	_resourceButtons			= nullptr;
	ResourceButtonsVisible		*	_resourceButtonsVisible		= nullptr;

	QFileSystemWatcher				_watcher;
	FileEvent::FileMode				_mode						= FileEvent::FileOpen;
	QString							_currentFilePath;
	Utils::FileType					_currentFileType			= Utils::FileType::unknown;
	bool							_currentFileReadOnly		= false,
									_visible					= false;
	ActionButtons::FileOperation	_fileoperation				= ActionButtons::Open;
};

#endif // FILEMENU_H
