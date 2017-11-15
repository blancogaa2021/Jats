//
// Copyright (C) 2013-2017 University of Amsterdam
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

#include "tabbar.h"
#include <QMessageBox>
#include "preferencesdialog.h"
#include "widgets/ribbonbutton.h"
#include "module.h"

using namespace std;

TabBar::TabBar(QWidget *parent) :
	QWidget(parent)
{

	_helpTab = NULL;

	_background = new QWidget(this);
	_background->setObjectName("background");

	_backgroundLayout = new QGridLayout(this);
	_backgroundLayout->setMargin(0);
	_backgroundLayout->addWidget(_background);

	_layout = new QHBoxLayout(_background);
	_layout->setMargin(0);
	_background->setLayout(_layout);

	setLayout(_backgroundLayout);

	_layout->addStretch(1);

	_aboutDialog = new AboutDialog(this);
	_preferencesDialog = new PreferencesDialog(this);
	_modulesButton = NULL;
}

QStringList TabBar::getCurrentModules()
{
	QStringList result;
	foreach (QPushButton *button, _tabButtons)
	{
		QString name = button->objectName();
		if (Module::isModuleName(name))
			result.append(name);
	}

	return result;
}

void TabBar::addModulesPlusButton()
{
	QPushButton *button = new QPushButton("", this);
	button->setToolTip("Load module");
	button->setIcon(QIcon(":/icons/addition.png"));
	_modulesButton = button;
	_signalModulesMapper = new QSignalMapper(this);
	connect(_modulesButton, SIGNAL(pressed()), this, SLOT(handleModuleButton()));

	QStringList currentModules = getCurrentModules();

	//Modules menu
	QMenu *modulesmenu   = new QMenu(this);

	for (auto it = Module::AllModules.begin(); it != Module::AllModules.end(); ++it)
	{
		const Module& module = it->second;
#ifndef QT_DEBUG
		if (module.released())
		{
#endif
			QAction *action = new QAction(module.displayName(),modulesmenu);
			QString name = module.name();
			action->setObjectName(name);
			action->setCheckable(true);
			action->setChecked(currentModules.indexOf(name) >= 0);
			modulesmenu->addAction(action);
			_signalModulesMapper->setMapping(action, name);
			connect(action, SIGNAL(triggered()), _signalModulesMapper, SLOT(map()));
#ifndef QT_DEBUG
		}
#endif
	}
	connect(_signalModulesMapper, SIGNAL(mapped(QString)), this, SLOT(toggleModule(QString)));
	modulesmenu->acceptDrops();
	button->setMenu(modulesmenu);
	_layout->insertWidget(_tabButtons.size(), button);

	button->setStyleSheet("border-top-left-radius:4px;border-top-right-radius:4px;");
	button->setObjectName("Modules");
	button->setCheckable(true);
	connect(button, SIGNAL(clicked()), this, SLOT(tabSelectedHandler()));

	_tabButtons.append(button);
}

void TabBar::addTab(QString name)
{
	foreach (QPushButton *button, _tabButtons)
	{
		if (button->objectName() == name)
			return;
	}

	QPushButton *button = new QPushButton(name, this);

	if (Module::isModuleName(name))
	{
		button->setText(Module::getModule(name).displayName());
		_layout->insertWidget(_tabButtons.size()-1, button);
		_currentModule = button;
	}
	else
		_layout->insertWidget(_tabButtons.size(), button);

	button->setStyleSheet("border-top-left-radius:4px;border-top-right-radius:4px;");
	if (_tabButtons.size() == 0)
		button->setObjectName("first"); //just to give it the proper (blue) stylesheet
	else
		button->setObjectName(name);

	button->setCheckable(true);
	connect(button, SIGNAL(clicked()), this, SLOT(tabSelectedHandler()));

	_tabButtons.append(button);
	button->clicked();

}

void TabBar::removeTab(QString tabName)
{
	QPushButton *lastButton = NULL;
	// Init lastButton in case with first button.
	foreach (QPushButton *button, _tabButtons)
		if (button->objectName() == "first")
			lastButton = button;

	foreach (QPushButton *button, _tabButtons)
	{
		QString buttonName = button->objectName();
		if (buttonName == tabName)
		{
			if (lastButton->objectName() == "first")
			{
				// Check whether another available module exists
				foreach (QPushButton *button2, _tabButtons)
					if (button2 != button && Module::isModuleName(button2->objectName()))
						lastButton = button2;
			}
			if (button == _currentTab)
			{
				_currentTab = lastButton;
				_currentTab->clicked();
			}
			if (button == _currentModule)
			{
				if (Module::isModuleName(lastButton->objectName()))
					_currentModule = lastButton;
				else
					_currentModule = NULL;
			}
			_tabButtons.removeAll(button);
			delete button;

			return;
		}
		if (buttonName != "Modules")
			lastButton = button;
	}
}


void TabBar::init()
{
	setFocusPolicy(Qt::NoFocus);
	addTab("File");
	addModulesPlusButton();
	addTab("Common"); // Common module must be added after the Plus Button,in order to set it at the right place
	setModulePlusMenu(); // A a check to Common in the menu

	RibbonButton *rb = new RibbonButton();
	rb->setIcon(QIcon(":/icons/summarize.svg"));
	rb->setPopupMode(QToolButton::InstantPopup);
	rb->setProperty("button-type", "summarize");
	rb->setMinimumSize(30,0);
	rb->setToolTip("Options");
	_layout->setContentsMargins(0,0,2,0);

	QMenu  *optionsmenu   = new QMenu(this);

	QAction *act_about = new QAction("About",optionsmenu);
	QAction *act_extrahelp = new QAction("Help",optionsmenu);
	QAction *act_preferences = new QAction("Preferences",optionsmenu);

	// About
	act_about->setObjectName("About");
	optionsmenu->addAction(act_about);
	optionsmenu->addSeparator();

	//Special Help
	act_extrahelp->setObjectName("Special Help");
	act_extrahelp->setCheckable(true);
	act_extrahelp->setChecked(false);
	optionsmenu->addAction(act_extrahelp);
	optionsmenu->addSeparator();

	// Preferences
	act_preferences->setObjectName("Preferences");
	optionsmenu->addAction(act_preferences);
	optionsmenu->addSeparator();

	optionsmenu->acceptDrops();

	rb->setMenu(optionsmenu);
	_layout->addWidget(rb);

	//Slots preferences
	connect(act_about, SIGNAL(triggered()), this, SLOT(showAbout()));
	connect(act_preferences, SIGNAL(triggered()), this, SLOT(showPreferences()));
	connect(act_extrahelp, SIGNAL(triggered()), this, SLOT(toggleHelp()));

}

void TabBar::showAbout()
{
	_aboutDialog->show();
	_aboutDialog->raise();
	_aboutDialog->activateWindow();
	//The last function performs the same operation as clicking the mouse on the title bar
	//If you want to ensure that the window is stacked on top as well you should also call raise(). 	//Note that the window must be visible, otherwise activateWindow() has no effect.
}

void TabBar::showPreferences()
{
	_preferencesDialog->show();
	_preferencesDialog->raise();
	_preferencesDialog->activateWindow();
}

void TabBar::toggleHelp()
{
	static bool on;
	on = ! on;
	helpToggledHandler(on);
}

void TabBar::setModulePlusMenu(QStringList usedModules)
{
	QStringList currentModules = getCurrentModules();
	foreach (QAction *action, _modulesButton->menu()->actions())
	{
		QString moduleName = action->objectName();
		if (usedModules.contains(moduleName))
		{
			if (!action->isChecked())
				action->setChecked(true);
			if (!currentModules.contains(moduleName))
				addTab(moduleName);
			action->setEnabled(false);
		}
		else
		{
			action->setEnabled(true);
			action->setChecked(currentModules.contains(moduleName));
		}
	}
}

void TabBar::toggleModule(QString name)
{
	bool on = true;
	foreach (QAction *action, _modulesButton->menu()->actions())
	{
		if (action->objectName() == name)
			on = action->isChecked();
	}
	if (on)
		addTab(name);
	else
		removeTab(name);
}

void TabBar::handleModuleButton()
{
	if (_modulesButton != NULL)
		_modulesButton->setAttribute(Qt::WA_UnderMouse, false);
}

int TabBar::count() const
{
	return _tabButtons.length();
}

PreferencesDialog *TabBar::getPreferencesDialog()
{
	return _preferencesDialog;
}

void TabBar::setCurrentModuleActive()
{
	if (_currentModule)
		setCurrentTab(_currentModule->objectName());
}

void TabBar::setExactPValues(bool exactPValues)
{
	emit setExactPValuesHandler(exactPValues);
}

void TabBar::setFixDecimals(QString numDecimals)
{
	emit setFixDecimalsHandler(numDecimals);
}

void TabBar::emptyValuesChanged()
{
	emit emptyValuesChangedHandler();
}

void TabBar::setCurrentTab(QString name)
{
	int i = 0, index = -1;

	foreach (QPushButton *button, _tabButtons)
	{
		if (button->objectName() == name)
		{
			button->setChecked(true);
			index = i;
			_currentTab = button;
			if (Module::isModuleName(name))
				_currentModule = button;
		}
		else
			button->setChecked(false);

		i++;
	}

	if (index >= 0)
		emit currentChanged(index);
}

void TabBar::tabSelectedHandler()
{
	QObject *source = this->sender();
	setCurrentTab(source->objectName());
}

QString TabBar::getCurrentActiveTab()
{
	return _currentTab ? _currentTab->objectName() : QString();
}

void TabBar::helpToggledHandler(bool on)
{
	emit helpToggled(on);
}
