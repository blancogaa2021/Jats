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

#include "analysisqmlform.h"
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>

#include <QQuickWidget>

#include "widgets/boundqmlcheckbox.h"
#include "widgets/boundqmlcombobox.h"
#include "widgets/boundqmlslider.h"
#include "widgets/boundqmltextinput.h"
#include "widgets/boundqmltextarea.h"
#include "widgets/boundqmlradiobuttons.h"
#include "widgets/boundqmllistviewpairs.h"
#include "widgets/boundqmllistviewanovamodels.h"
#include "widgets/boundqmllistviewterms.h"
#include "widgets/boundqmllistviewmeasurescells.h"
#include "widgets/boundqmlfactorslist.h"
#include "widgets/boundqmltableview.h"
#include "widgets/qmllistviewtermsavailable.h"
#include "widgets/listmodeltermsavailable.h"

#include "utilities/qutils.h"
#include "utils.h"
#include "dirs.h"
#include "utilities/settings.h"

using namespace std;

AnalysisQMLForm::AnalysisQMLForm(QObject *parent, Analysis* analysis)
	:  AnalysisForm("AnalysisQMLForm", parent)
	, _quickWidget(new QQuickWidget())
	, _analysis(analysis)
	, _errorMessagesItem(nullptr)
{
	_quickWidget->engine()->addImportPath("qrc:///components");
	_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

	connect(_quickWidget,	&QQuickWidget::statusChanged,	this,	&AnalysisQMLForm::statusChangedWidgetHandler);
	connect(_quickWidget,	&QQuickWidget::sceneGraphError,	this,	&AnalysisQMLForm::sceneGraphErrorHandler);
	connect(&_QMLwatcher,	&QFileSystemWatcher::fileChanged, this, &AnalysisQMLForm::QMLFileModifiedHandler);

	bool debug = false;
#ifdef JASP_DEBUG
	debug = true;
	_quickWidget->engine()->clearComponentCache();
#endif
	_quickWidget->engine()->rootContext()->setContextProperty("DEBUG_MODE", debug);

	QString pathToQMLFile = _getAnalysisQMLPath();

	if (!pathToQMLFile.isEmpty())
	{
#ifdef JASP_DEBUG
		std::cout << "Succesfully loaded QML file path for analysis: " << pathToQMLFile.toStdString() << ", now being set as source of qquickwidget, actually loaded and parsed." << std::endl;
#endif
		_quickWidget->setSource(QUrl(pathToQMLFile));
		_parseQML();
	}
}

QString AnalysisQMLForm::_getAnalysisQMLPath()
{
	QString path = QString::fromStdString(_analysis->qmlFormPath());

	if (_analysis->isDynamicModule())
	{
		QString ospath = path;
		if (ospath.startsWith("file:"))
			ospath.remove(0, 5);
		if (!_QMLwatcher.files().contains(ospath)) 
		{
			if (!_QMLwatcher.addPath(ospath))
				qDebug() << "Could not add watcher to " << ospath;
		}		
	}

	return path;
}

void AnalysisQMLForm::rScriptDoneHandler(QVariant key, const QString &result)
{
	BoundQMLItem* item = dynamic_cast<BoundQMLItem*>(getControl(key.toString()));
	if (item)
		item->rScriptDoneHandler(result);
#ifdef JASP_DEBUG
	else
		std::cout << "Unknown item " << key.toString().toStdString() << std::endl;
#endif
		
}

void AnalysisQMLForm::_parseQML()
{	
	QQuickItem *root = _quickWidget->rootObject();

	_analysis->setUsesJaspResults(QQmlProperty(root, "usesJaspResults").read().toBool());
	
	map<QString, QString>	dropKeyMap;
	QList<QString>			controlNames;
		
	for (QQuickItem* quickItem : root->findChildren<QQuickItem *>())
	{
		if (quickItem->objectName() == "errorMessagesBox")
		{
			_errorMessagesItem = quickItem;
			continue;			
		}
		
		QString controlTypeStr = QQmlProperty(quickItem, "controlType").read().toString();
		if (controlTypeStr.isEmpty())
			continue;
		
		if (! QQmlProperty(quickItem, "isBound").read().toBool())
			continue;
		
#ifndef QT_DEBUG
		bool isDebug = QQmlProperty(quickItem, "debug").read().toBool();
		if (isDebug)
			continue;
#endif
		
		bool isVisible = QQmlProperty(quickItem, "visible").read().toBool();
		QString controlName = QQmlProperty(quickItem, "name").read().toString();
		
		if (isVisible)
		{
			if (controlName.isEmpty())
			{
				_errorMessages.append(QString::fromLatin1("A control ") + controlTypeStr + QString::fromLatin1(" has no name"));
				continue;
			}
			if (controlNames.contains(controlName))
			{
				_errorMessages.append(QString::fromLatin1("2 controls have the same name: ") + controlName);
				continue;
			}
			controlNames.append(controlName);
		}
		
		QMLItem *control = nullptr;
		qmlControlType controlType = qmlControlTypeFromQString(controlTypeStr);

		if (!isVisible && controlType == qmlControlType::VariablesListView)
			continue;
		
		switch(controlType)
		{
		case qmlControlType::CheckBox:			//fallthrough:
		case qmlControlType::Switch:			control = new BoundQMLCheckBox(quickItem,		this);	break;
		case qmlControlType::TextField:			control = new BoundQMLTextInput(quickItem,		this);	break;
		case qmlControlType::ButtonGroup:		control = new BoundQMLRadioButtons(quickItem,	this);	break;
		case qmlControlType::Slider:			control = new BoundQMLSlider(quickItem,			this);	break;
		case qmlControlType::TextArea:
		{
			BoundQMLTextArea* boundQMLTextArea = new BoundQMLTextArea(quickItem,	this);
			control = boundQMLTextArea;
			ListModelTermsAvailable* allVariablesModel = boundQMLTextArea->allVariablesModel();
			if (allVariablesModel)
				_allAvailableVariablesModels.push_back(allVariablesModel);
			break;
		}
		case qmlControlType::ComboBox:
		{
			BoundQMLComboBox* boundQMLComboBox = new BoundQMLComboBox(quickItem, this);
			control = boundQMLComboBox;
			ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(boundQMLComboBox->model());
			if (availableModel)
			{
				if (boundQMLComboBox->hasAllVariablesModel)
					_allAvailableVariablesModels.push_back(availableModel);
			}
			break;
		}
		case qmlControlType::FactorsList:
		{
			BoundQMLFactorsList* factorList = new BoundQMLFactorsList(quickItem, this);
			control = factorList;
			_modelMap[controlName] = factorList->model();
			break;
		}
		case qmlControlType::TableView:
		{
			BoundQMLTableView* tableView = new BoundQMLTableView(quickItem, this);
			control = tableView;
			break;
		}
		case qmlControlType::VariablesListView:
		{
			QMLListView* listView = nullptr;
			QString			listViewTypeStr = QQmlProperty(quickItem, "listViewType").read().toString();
			qmlListViewType	listViewType;

			try							{ listViewType	= qmlListViewTypeFromQString(listViewTypeStr);	}
			catch(std::out_of_range)	{ _errorMessages.append(QString::fromLatin1("Unknown listViewType: ") + listViewType + QString::fromLatin1(". Cannot set a model to the VariablesList")); }

			switch(listViewType)
			{
			case qmlListViewType::AssignedVariables:	listView = new BoundQMLListViewTerms(quickItem, this); break;
			case qmlListViewType::AssignedPairs:		listView = new BoundQMLListViewPairs(quickItem,this); break;
			case qmlListViewType::AssignedAnova:		listView = new BoundQMLListViewAnovaModels(quickItem, this); break;
			case qmlListViewType::MeasuresCells:		listView = new BoundQMLListViewMeasuresCells(quickItem, this); break;
			case qmlListViewType::AvailableVariables:
			{
				QMLListViewTermsAvailable* availableVariablesListView = new QMLListViewTermsAvailable(quickItem, this);
				listView = availableVariablesListView;
				ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(availableVariablesListView->model());
				_availableVariablesModels.push_back(availableModel);
				
				if (availableVariablesListView->syncModelsList().isEmpty()) // If there is no syncModels, set all available variables.
					_allAvailableVariablesModels.push_back(availableModel);
				break;
			}
			default:
				_errorMessages.append(QString::fromLatin1("Unused (in AnalysisQMLForm::_parseQML) listViewType: ") + qmlListViewTypeToQString(listViewType) + QString::fromLatin1(". Cannot set a model to the VariablesList"));
				break;
			}
			
			_modelMap[controlName] = listView->model();
			control = dynamic_cast<QMLItem*>(listView);
				
			QList<QVariant> dropKeyList = QQmlProperty(quickItem, "dropKeys").read().toList();
			QString dropKey				= dropKeyList.isEmpty() ? QQmlProperty(quickItem, "dropKeys").read().toString() : dropKeyList[0].toString(); // The first key gives the default drop item.
			
			if (!dropKey.isEmpty())
				dropKeyMap[controlName] = dropKey;
			else
			{
				bool draggable = QQmlProperty(quickItem, "draggabble").read().toBool();
				if (draggable)
					_errorMessages.append(QString::fromLatin1("No drop key found for ") + controlName);
			}

			break;
		}
		case qmlControlType::JASPControl:
		default:
			_errorMessages.append(QString::fromLatin1("Unknown type of JASPControl ") + controlName + QString::fromLatin1(" : ") + controlTypeStr);			
		}
		
		if (control)
			_controls[control->name()] = control;
	}
	
	for (auto const& pair : dropKeyMap)
	{
		ListModel* sourceModel = _modelMap[pair.first];
		ListModel* targetModel = _modelMap[pair.second];

		if (sourceModel && targetModel)
		{
			QMLListView* sourceListView = sourceModel->listView();
			_relatedModelMap[sourceListView] = targetModel;
		}
		else
			_errorMessages.append(QString::fromLatin1("Cannot find a ListView for ") + (!sourceModel ? pair.first : pair.second));
	}
	
	_setUpItems();
	
	if (!_errorMessagesItem)
		qDebug() << "No errorMessages Item found!!!";
	
	_setErrorMessages();
}

void AnalysisQMLForm::_setUpItems()
{
	QList<QMLItem*> controls = _controls.values();
	for (QMLItem* control : controls)
		control->setUp();
	
	// set the order of the BoundItems according to their dependencies (for binding purpose)
	for (QMLItem* control : controls)
	{
		QVector<QMLItem*> depends = control->depends();
		int index = 0;
		while (index < depends.length())
		{
			QMLItem* depend = depends[index];
			const QVector<QMLItem*>& dependdepends = depend->depends();
			for (QMLItem* dependdepend : dependdepends)
			{
				if (dependdepend == control)
					addError(tq("Circular dependency between control ") + control->name() + tq(" and ") + depend->name());
				else
				{
					if (control->addDependency(dependdepend))
						depends.push_back(dependdepend);
				}
			}
			index++;
		}
	}
	std::sort(controls.begin(), controls.end(), 
			  [](QMLItem* a, QMLItem* b) { 
					return a->depends().length() > b->depends().length(); 
			});
	
	for (QMLItem* control : controls)
	{
		BoundQMLItem* boundItem = dynamic_cast<BoundQMLItem*>(control);
		if (boundItem)
			_boundItemsOrdered.push_back(boundItem);
	}	
}

void AnalysisQMLForm::_setErrorMessages()
{
	if (!_errorMessages.isEmpty() && _errorMessagesItem)
	{
		QString text;
		if (_errorMessages.length() == 1)
			text = _errorMessages[0];
		else
		{
			text.append("<ul style=\"margin-bottom:0px\">");
			for (const QString& errorMessage : _errorMessages)
				text.append("<li>").append(errorMessage).append("</li>");
			text.append("</ul>");
		}
		QQmlProperty(_errorMessagesItem, "text").write(QVariant::fromValue(text));
		_errorMessagesItem->setVisible(true);
	}
}

void AnalysisQMLForm::_setAllAvailableVariablesModel()
{
	if (_allAvailableVariablesModels.size() == 0)
		return;
	
	vector<string> columnNames;

	if (_dataSet != nullptr)
		for (Column &column: _dataSet->columns())
			columnNames.push_back(column.name());

	for (ListModelTermsAvailable* model : _allAvailableVariablesModels)
		model->initTerms(columnNames);
}

void AnalysisQMLForm::bindTo(Options *options, DataSet *dataSet)
{
	if (_options != nullptr)
		unbind();

	_dataSet = dataSet;
	_options = options;
	
	_options->blockSignals(true);
	
	_setAllAvailableVariablesModel();	
	
	for (BoundQMLItem* item : _boundItemsOrdered)
	{
		string name = item->name().toStdString();
		Option* option = options->get(name);
		if (!option)
		{
			option = item->createOption();
			options->add(name, option);
		}
		item->bindTo(option);
		item->illegalChanged.connect(boost::bind(&AnalysisForm::illegalValueHandler, this, _1));
	}
	
	for (ListModelTermsAvailable* availableModel : _availableVariablesModels)
	{
		// The availableModel are not bound, but they have to be updated when the form is initialized.
		availableModel->resetTermsFromSyncModels();
	}

	_options->blockSignals(false);
	
	updateIllegalStatus();
}

void AnalysisQMLForm::unbind()
{
	_bounds.clear();
	updateIllegalStatus();

	if (_options == nullptr)
		return;
	
	for (BoundQMLItem* item : _boundItemsOrdered)
		item->unbind();

	_options = nullptr;
}

void AnalysisQMLForm::addError(const QString &error)
{
	_errorMessages.append(error);
	_setErrorMessages();
}

void AnalysisQMLForm::statusChangedWidgetHandler(QQuickWidget::Status status)
{
	if (status == QQuickWidget::Error)
	{
		QQuickWidget* widget = qobject_cast<QQuickWidget*>(sender());
		QString message;
		for (QQmlError error : widget->errors())
		{
			if (!message.isEmpty())
				message += '\n';
			message += error.toString();
		}
		std::cerr << "QMessageBox::warning(this, \"Error\", \"Error when loading analysis form: \n" << message.toStdString() << ");";
	}
}

void AnalysisQMLForm::QMLFileModifiedHandler(QString path)
{
	qDebug() << "Test QML file modified";
/*	_controls.clear();
	_boundItemsOrdered.clear();
	_relatedModelMap.clear();
	_availableVariablesModels.clear();
	_modelMap.clear();
	_allAvailableVariablesModels.clear();
	_errorMessagesItem = nullptr;
	_errorMessages.clear();	
*/	
	_quickWidget->engine()->clearComponentCache();
	emit formChanged(_analysis);
}
