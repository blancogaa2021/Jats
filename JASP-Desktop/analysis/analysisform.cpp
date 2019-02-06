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

#include "analysisform.h"

#include <QDebug>
#include <boost/bind.hpp>

#include "options/bound.h"
#include "utilities/qutils.h"

#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include "widgets/boundqmlcheckbox.h"
#include "widgets/boundqmlcombobox.h"
#include "widgets/boundqmlslider.h"
#include "widgets/boundqmltextinput.h"
#include "widgets/boundqmltextarea.h"
#include "widgets/boundqmlradiobuttons.h"
#include "widgets/boundqmllistviewpairs.h"
#include "widgets/boundqmllistviewinteraction.h"
#include "widgets/boundqmllistviewterms.h"
#include "widgets/boundqmllistviewmeasurescells.h"
#include "widgets/boundqmllistviewlayers.h"
#include "widgets/boundqmlrepeatedmeasuresfactors.h"
#include "widgets/boundqmltableview.h"
#include "widgets/qmllistviewtermsavailable.h"
#include "widgets/listmodeltermsavailable.h"

#include "utilities/qutils.h"
#include "utils.h"
#include "dirs.h"
#include "utilities/settings.h"
#include "gui/messageforwarder.h"
#include "mainwindow.h"

using namespace std;

AnalysisForm::AnalysisForm(QQuickItem *parent) : QQuickItem(parent)
{
	setObjectName("AnalysisForm");
	_mainVariables = nullptr;

	_options = nullptr;
	_dataSet = nullptr;

	_hasIllegalValue = false;
	
	connect(this, &AnalysisForm::formCompleted, this, &AnalysisForm::formCompletedHandler);
}

bool AnalysisForm::hasIllegalValue() const
{
	return _hasIllegalValue;
}

const QString &AnalysisForm::illegalValueMessage() const
{
	return _illegalMessage;
}

QVariant AnalysisForm::requestInfo(const Term &term, VariableInfo::InfoType info) const
{
	try {
		switch(info)
		{
		case VariableInfo::VariableType:		return _dataSet->column(term.asString()).columnType();
		case VariableInfo::VariableTypeName:	return MainWindow::columnTypeToString(_dataSet->column(term.asString()).columnType());
		case VariableInfo::Labels:
		{
			QStringList values;
			Labels &labels = _dataSet->column(term.asString()).labels();
			for (const auto & label : labels)
				values.append(tq(label.text()));

			return values;
		}
		}
	}
	catch(columnNotFound e) {} //just return an empty QVariant right?
	catch(std::exception e)
	{
#ifdef JASP_DEBUG
		std::cout << "AnalysisForm::requestInfo had an exception! " << e.what() << std::flush;
#endif
		throw e;
	}
	return QVariant();

}

void AnalysisForm::updateIllegalStatus()
{
	QString message;
	bool illegal = false;

	for (const Bound *bound : _bounds)
	{
		if (bound->isIllegal())
		{
			if ( ! illegal)
				message = bound->illegalMessage();

			illegal = true;
		}
	}

	if (illegal != _hasIllegalValue || message != _illegalMessage)
	{
		_hasIllegalValue = illegal;
		_illegalMessage = message;

		emit illegalChanged(this);
	}
}

void AnalysisForm::illegalValueHandler(Bound *source)
{
	updateIllegalStatus();
}

void AnalysisForm::runRScript(QString script, QString controlName)
{
	emit _analysis->sendRScript(_analysis, script, controlName);
}

void AnalysisForm::runScriptRequestDone(const QString& result, const QString& controlName)
{	
	BoundQMLItem* item = dynamic_cast<BoundQMLItem*>(getControl(controlName));
	if (item)
		item->rScriptDoneHandler(result);
#ifdef JASP_DEBUG
	else
		std::cout << "Unknown item " << controlName.toStdString() << std::endl;
#endif

}

void AnalysisForm::_parseQML()
{
	QQuickItem *root = this;

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

		QString controlName = QQmlProperty(quickItem, "name").read().toString();

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

		QMLItem *control = nullptr;
		qmlControlType controlType = qmlControlTypeFromQString(controlTypeStr);

		switch(controlType)
		{
		case qmlControlType::CheckBox:			//fallthrough:
		case qmlControlType::Switch:			control = new BoundQMLCheckBox(quickItem,		this);	break;
		case qmlControlType::TextField:			control = new BoundQMLTextInput(quickItem,		this);	break;
		case qmlControlType::RadioButtonGroup:	control = new BoundQMLRadioButtons(quickItem,	this);	break;
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
		case qmlControlType::RepeatedMeasuresFactorsList:
		{
			BoundQMLRepeatedMeasuresFactors* factorList = new BoundQMLRepeatedMeasuresFactors(quickItem, this);
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
			case qmlListViewType::AssignedVariables:	listView = new BoundQMLListViewTerms(quickItem, this);			break;
			case qmlListViewType::AssignedPairs:		listView = new BoundQMLListViewPairs(quickItem,this);			break;
			case qmlListViewType::Interaction:			listView = new BoundQMLListViewInteraction(quickItem, this);	break;
			case qmlListViewType::MeasuresCells:		listView = new BoundQMLListViewMeasuresCells(quickItem, this);	break;
			case qmlListViewType::Layers:				listView = new BoundQMLListViewLayers(quickItem, this);			break;
			case qmlListViewType::AvailableVariables:
			{
				QMLListViewTermsAvailable* availableVariablesListView = new QMLListViewTermsAvailable(quickItem, this);
				listView = availableVariablesListView;
				ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(availableVariablesListView->model());
				if (availableVariablesListView->sourceModelsList().isEmpty()) // If there is no sourceModels, set all available variables.
					_allAvailableVariablesModels.push_back(availableModel);
				break;
			}
			default:
				_errorMessages.append(QString::fromLatin1("Unused (in AnalysisForm::_parseQML) listViewType: ") + qmlListViewTypeToQString(listViewType) + QString::fromLatin1(". Cannot set a model to the VariablesList"));
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

void AnalysisForm::_setUpItems()
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
					return a->depends().length() < b->depends().length(); 
			});

	for (QMLItem* control : controls)
	{
		_orderedControls.push_back(control);
	}	
}

void AnalysisForm::_setErrorMessages()
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

void AnalysisForm::_setAllAvailableVariablesModel()
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

void AnalysisForm::bindTo(Options *options, DataSet *dataSet, const Json::Value& optionsFromJASPFile)
{
	if (_options != nullptr)
		unbind();

	_dataSet = dataSet;
	_options = options;

	_options->blockSignals(true);
	
	_setAllAvailableVariablesModel();	
	
	for (QMLItem* control : _orderedControls)
	{
		BoundQMLItem* boundControl = dynamic_cast<BoundQMLItem*>(control);
		if (boundControl)
		{
			std::string name = boundControl->name().toStdString();
			Option* option = options->get(name);
			if (option && !boundControl->isOptionValid(option))
			{
				option = nullptr;
				addError(tq("Item " + name + " was loaded with a wrong kind of value." + (optionsFromJASPFile != Json::nullValue ? ". Probably the file comes from an older version of JASP." : "")));
			}
			if (!option)
			{
				option = boundControl->createOption();
				if (optionsFromJASPFile != Json::nullValue)
				{
					if (optionsFromJASPFile[name] != Json::nullValue)
						option->set(optionsFromJASPFile[name]);
				}
				options->add(name, option);
			}
			boundControl->bindTo(option);
			boundControl->illegalChanged.connect(boost::bind(&AnalysisForm::illegalValueHandler, this, _1));
		}
		else
		{
			QMLListViewTermsAvailable* availableListControl = dynamic_cast<QMLListViewTermsAvailable *>(control);
			// The availableListControl are not bound, but they have to be updated when the form is initialized.
			if (availableListControl)
				availableListControl->availableModel()->resetTermsFromSourceModels();
		}
	}

	_options->blockSignals(false, false);

	updateIllegalStatus();
}

void AnalysisForm::unbind()
{
	_bounds.clear();
	updateIllegalStatus();

	if (_options == nullptr)
		return;
	
	for (QMLItem* control : _orderedControls)
	{
		BoundQMLItem* boundControl = dynamic_cast<BoundQMLItem*>(control);
		if (boundControl)
			boundControl->unbind();
	}

	_options = nullptr;
}

void AnalysisForm::addError(const QString &error)
{
	_errorMessages.append(error);
	_setErrorMessages();
}

void AnalysisForm::formCompletedHandler()
{
	QTimer::singleShot(0, this, &AnalysisForm::_formCompletedHandler);
}

void AnalysisForm::_formCompletedHandler()
{
	QVariant analysisVariant = QQmlProperty(this, "analysis").read();
	if (!analysisVariant.isNull())
	{
		_analysis = qobject_cast<Analysis *>(analysisVariant.value<QObject *>());
		_parseQML();
		bool isNewAnalysis = _analysis->options()->size() == 0 && _analysis->optionsFromJASPFile().size() == 0;
		bindTo(_analysis->options(), _analysis->getDataSet(), _analysis->optionsFromJASPFile());
		_analysis->resetOptionsFromJASPFile();
		_analysis->initialized(this, isNewAnalysis);	
	}
}
