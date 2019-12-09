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


#include <boost/bind.hpp>

#include "options/bound.h"
#include "utilities/qutils.h"

#include <QQmlProperty>
#include <QQmlContext>

#include "widgets/listmodeltermsavailable.h"

#include "utils.h"
#include "dirs.h"
#include "utilities/settings.h"
#include "gui/messageforwarder.h"
#include "mainwindow.h"
#include "log.h"
#include "jaspcontrolbase.h"
#include "widgets/boundqmlcombobox.h"
#include "widgets/boundqmltextarea.h"
#include "widgets/boundqmllistviewterms.h"
#include "widgets/boundqmllistviewmeasurescells.h"
#include "widgets/boundqmllistviewlayers.h"
#include "widgets/boundqmlinputlist.h"
#include "widgets/boundqmlrepeatedmeasuresfactors.h"
#include "widgets/boundqmlfactorsform.h"
#include "widgets/boundqmltableview.h"
#include "widgets/qmllistviewtermsavailable.h"
#include "widgets/qmlexpander.h"


using namespace std;

AnalysisForm::AnalysisForm(QQuickItem *parent) : QQuickItem(parent)
{
	setObjectName("AnalysisForm");

	connect(this, &AnalysisForm::formCompleted, this, &AnalysisForm::formCompletedHandler);
}

QVariant AnalysisForm::requestInfo(const Term &term, VariableInfo::InfoType info) const
{
	try {
		switch(info)
		{
		case VariableInfo::VariableType:		return int(_package->getColumnType(term.asString()));
		case VariableInfo::VariableTypeName:	return columnTypeToQString(_package->getColumnType(term.asString()));
		case VariableInfo::Labels:				return _package->getColumnLabelsAsStringList(term.asString());
		}
	}
	catch(columnNotFound & e) {} //just return an empty QVariant right?
	catch(std::exception & e)
	{
		Log::log() << "AnalysisForm::requestInfo had an exception! " << e.what() << std::flush;
		throw e;
	}
	return QVariant();

}

void AnalysisForm::runRScript(QString script, QString controlName, bool whiteListedVersion)
{
	if(_analysis != nullptr && !_removed)
		emit _analysis->sendRScript(_analysis, script, controlName, whiteListedVersion);
}

void AnalysisForm::refreshAnalysis()
{
	_analysis->refresh();
}

void AnalysisForm::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
	if (change == ItemChange::ItemSceneChange && !value.window)
		_cleanUpForm();
	QQuickItem::itemChange(change, value);
}

void AnalysisForm::_cleanUpForm()
{
	_removed = true;
	for (JASPControlWrapper* control : _orderedControls)
		// controls will be automatically deleted by the deletion of AnalysisForm
		// But they must be first disconnected: sometimes an event seems to be triggered before the item is completely destroyed
		control->cleanUp();
}

void AnalysisForm::runScriptRequestDone(const QString& result, const QString& controlName)
{	
	if(_removed)
		return;

	BoundQMLItem* item = dynamic_cast<BoundQMLItem*>(getControl(controlName));

	if (item)
		item->rScriptDoneHandler(result);
	else
		Log::log() << "Unknown item " << controlName.toStdString() << std::endl;
}

void AnalysisForm::_addControlWrapper(JASPControlWrapper* controlWrapper)
{
	switch(controlWrapper->item()->controlType())
	{
	case JASPControlBase::ControlType::Expander:
	{
		QMLExpander* expander = dynamic_cast<QMLExpander*>(controlWrapper);
		_expanders.push_back(expander);
		break;
	}
	case JASPControlBase::ControlType::TextArea:
	{
		BoundQMLTextArea* boundQMLTextArea = dynamic_cast<BoundQMLTextArea*>(controlWrapper);
		ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(boundQMLTextArea->model());
		if (availableModel)
		{
			if (boundQMLTextArea->modelHasAllVariables())
				_allAvailableVariablesModels.push_back(availableModel);
			_modelMap[controlWrapper->name()] = availableModel;
		}

		break;
	}
	case JASPControlBase::ControlType::ComboBox:
	{
		BoundQMLComboBox* boundQMLComboBox = dynamic_cast<BoundQMLComboBox*>(controlWrapper);
		ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(boundQMLComboBox->model());
		if (availableModel)
		{
			if (boundQMLComboBox->modelHasAllVariables())
				_allAvailableVariablesModels.push_back(availableModel);
			_modelMap[controlWrapper->name()] = availableModel;
		}
		break;
	}
	case JASPControlBase::ControlType::RepeatedMeasuresFactorsList:
	case JASPControlBase::ControlType::InputListView:
	case JASPControlBase::ControlType::ComponentsList:
	case JASPControlBase::ControlType::FactorsForm:
	case JASPControlBase::ControlType::TableView:
	case JASPControlBase::ControlType::VariablesListView:
	{
		QMLListView* listView = dynamic_cast<QMLListView*>(controlWrapper);
		_modelMap[controlWrapper->name()] = listView->model();

		QMLListViewTermsAvailable* listViewTermsAvailable = dynamic_cast<QMLListViewTermsAvailable*>(listView);
		if (listViewTermsAvailable)
		{
			ListModelTermsAvailable* availableModel = dynamic_cast<ListModelTermsAvailable*>(listViewTermsAvailable->model());

			if (availableModel)
			{
				if (!listViewTermsAvailable->hasSource())
					_allAvailableVariablesModels.push_back(availableModel);
				else
					_allAvailableVariablesModelsWithSource.push_back(availableModel);
			}
		}
		break;
	}
	default:
		break;
	}

}

void AnalysisForm::addControl(JASPControlBase *control)
{
	if (control->isBound())
	{
		const QString& name = control->name();
		if (name.isEmpty())
			_errorMessages.append(tr("A control %1 has no name").arg(control->controlType()));
		else if (_controls.keys().contains(name))
			_errorMessages.append(tr("2 controls have the same name: %1").arg(name));
		else
		{
			JASPControlWrapper* wrapper = control->getWrapper();
			_controls[name] = wrapper;
			_addControlWrapper(wrapper);
		}
	}
	else
		_addControlWrapper(control->getWrapper());
}

void AnalysisForm::_setUpControls()
{
	_analysis->setUsesJaspResults(QQmlProperty(this, "usesJaspResults").read().toBool());

	_orderExpanders();
	_setUpRelatedModels();
	_setUpItems();

	if (!_errorMessagesItem)
		Log::log()  << "No errorMessages Item found!" << std::endl;

	_setErrorMessages();
}

void AnalysisForm::_setUpRelatedModels()
{
	for (ListModel* model : _modelMap.values())
	{
		QMLListView* listView = model->listView();
		QList<QVariant> dropKeyList = listView->getItemProperty("dropKeys").toList();
		QString dropKey				= dropKeyList.isEmpty() ? listView->getItemProperty("dropKeys").toString() : dropKeyList[0].toString(); // The first key gives the default drop item.

		if (!dropKey.isEmpty())
		{
			ListModel* targetModel = _modelMap[dropKey];
			if (targetModel)
				_relatedModelMap[listView] = targetModel;
			else
				_errorMessages.append(tr("Cannot find a source %1 for VariableList %2").arg(dropKey).arg(listView->name()));
		}
		else
		{
			bool draggable = listView->getItemProperty("draggabble").toBool();
			if (draggable)
				_errorMessages.append(tr("No drop key found for %1").arg(listView->name()));
		}

	}
}

void AnalysisForm::_setUpItems()
{
	QList<JASPControlWrapper*> controls = _controls.values();
	for (JASPControlWrapper* control : controls)
		control->setUp();

	// set the order of the BoundItems according to their dependencies (for binding purpose)
	for (JASPControlWrapper* control : controls)
	{
		QVector<JASPControlWrapper*> depends = control->depends();
		int index = 0;
		while (index < depends.length())
		{
			JASPControlWrapper* depend = depends[index];
			const QVector<JASPControlWrapper*>& dependdepends = depend->depends();
			for (JASPControlWrapper* dependdepend : dependdepends)
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
			  [](JASPControlWrapper* a, JASPControlWrapper* b) {
					return a->depends().length() < b->depends().length(); 
			});

	for (JASPControlWrapper* control : controls)
	{
		_orderedControls.push_back(control);
	}
}

void AnalysisForm::_orderExpanders()
{
	for (QMLExpander* expander : _expanders)
	{
		bool foundExpander = false;
		for (QObject* sibling : expander->item()->parent()->parent()->children())
		{
			if (sibling->objectName() == "Section")
			{
				QObject* button = sibling->property("button").value<QObject*>();
				JASPControlBase* control = qobject_cast<JASPControlBase*>(button);
				if (control && control->controlType() == JASPControlBase::ControlType::Expander)
				{
					if (foundExpander)
					{
						_nextExpanderMap[expander] = dynamic_cast<QMLExpander*>(control->getWrapper());
						break;
					}
					if (control->getWrapper() == expander)
						foundExpander = true;
				}
			}
		}
		expander->setUp();
	}
}

void AnalysisForm::addListView(QMLListView* listView, QMLListView* source)
{
	_modelMap[listView->name()] = listView->model();
	_relatedModelMap[listView] = source->model();
	_relatedModelMap[source] = listView->model();
}

void AnalysisForm::reset()
{
	_analysis->clearOptions();
    _analysis->reload();
}

void AnalysisForm::exportResults()
{
    _analysis->exportResults();
}

void AnalysisForm::_setErrorMessages()
{
	if (_errorMessagesItem)
	{
		if (!_errorMessages.isEmpty())
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
		else
		{
			QQmlProperty(_errorMessagesItem, "text").write(QVariant::fromValue(QString()));
			_errorMessagesItem->setVisible(false);
		}
	}
}

void AnalysisForm::_setAllAvailableVariablesModel(bool refreshAssigned)
{
	if (_allAvailableVariablesModels.size() == 0)
		return;

	std::vector<std::string> columnNames = _package->getColumnNames();


	for (ListModelTermsAvailable* model : _allAvailableVariablesModels)
	{
		model->initTerms(columnNames);

		if (refreshAssigned)
		{
			QMLListViewTermsAvailable* qmlAvailableListView = dynamic_cast<QMLListViewTermsAvailable*>(model->listView());
			if (qmlAvailableListView)
			{
				const QList<ListModelAssignedInterface*>& assignedModels = qmlAvailableListView->assignedModel();	
				for (ListModelAssignedInterface* modelAssign : assignedModels)
					modelAssign->refresh();
			}
		}
	}

	if (refreshAssigned)
		for (ListModelTermsAvailable * model : _allAvailableVariablesModelsWithSource)
			model->resetTermsFromSourceModels(true);

}

void AnalysisForm::bindTo()
{
	if (_options != nullptr)
		unbind();

	const Json::Value& optionsFromJASPFile = _analysis->optionsFromJASPFile();
	_package = _analysis->getDataSetPackage();
	_options = _analysis->options();
	QVector<ListModelAvailableInterface*> availableModelsToBeReset;

	_options->blockSignals(true);
	
	_setAllAvailableVariablesModel();	
	
	for (JASPControlWrapper* control : _orderedControls)
	{
		BoundQMLItem* boundControl = dynamic_cast<BoundQMLItem*>(control);
		if (boundControl)
		{
			std::string name = boundControl->name().toStdString();
			Option* option   = _options->get(name);

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
					const Json::Value& optionValue = optionsFromJASPFile[name];
					if (optionValue != Json::nullValue)
					{
						if (!boundControl->isJsonValid(optionValue))
						{
							std::string labelStr;
							QVariant label = boundControl->getItemProperty("label");

							if (!label.isNull())
								labelStr = label.toString().toStdString();

							if (labelStr.empty())
							{
								label = boundControl->getItemProperty("title");
								labelStr = label.toString().toStdString();
							}

							if (labelStr.empty())
								labelStr = name;

							addError(tq("Control " + labelStr + " was loaded with a wrong kind of value. The file probably comes from an older version of JASP.<br>"
										+ "That means that the results currently displayed do not correspond to the options selected.<br>Refreshing the analysis may change the results"));
						}
						else
							option->set(optionValue);
					}
				}
				_options->add(name, option);
			}

			boundControl->bindTo(option);
		}
		else
		{
			QMLListViewTermsAvailable* availableListControl = dynamic_cast<QMLListViewTermsAvailable *>(control);
			if (availableListControl && availableListControl->hasSource())
			{
				ListModelAvailableInterface* availableModel = availableListControl->availableModel();
				// The availableList control are not bound with options, but they have to be updated from their sources when the form is initialized.
				// The availableList cannot signal their assigned models now because they are not yet bound (the controls are ordered by dependency)
				// When the options come from a JASP file, an assigned model needs sometimes the available model (eg. to determine the kind of terms they have).
				// So in this case resetTermsFromSourceModels has to be called now but with updateAssigned argument set to false.
				// When the options come from default options (from source models), the availableList needs sometimes to signal their assigned models (e.g. when addAvailableVariablesToAssigned if true).
				// As their assigned models are not yet bound, resetTermsFromSourceModels (with updateAssigned argument set to true) must be called afterwards.
				if (availableModel)
				{
					if (optionsFromJASPFile != Json::nullValue || _analysis->isDuplicate())
						availableModel->resetTermsFromSourceModels(false);
					else
						availableModelsToBeReset.push_back(availableModel);
				}
			}
		}
	}

	for (ListModelAvailableInterface* availableModel : availableModelsToBeReset)
		availableModel->resetTermsFromSourceModels(true);
	
	_options->blockSignals(false, false);
}

void AnalysisForm::unbind()
{
	if (_options == nullptr)
		return;
	
	for (JASPControlWrapper* control : _orderedControls)
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
	_lastAddedErrorTimestamp = Utils::currentSeconds();
	_setErrorMessages();
}

void AnalysisForm::clearErrors()
{
	if (Utils::currentSeconds() - _lastAddedErrorTimestamp > 5)
	{
		_errorMessages.clear();
		_setErrorMessages();
	}
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
		_analysis	= qobject_cast<Analysis *>(analysisVariant.value<QObject *>());
		_package	= _analysis->getDataSetPackage();

		_setUpControls();

		bool isNewAnalysis = _analysis->options()->size() == 0 && _analysis->optionsFromJASPFile().size() == 0;

		bindTo();
		_analysis->resetOptionsFromJASPFile();
		_analysis->initialized(this, isNewAnalysis);
	}
}

void AnalysisForm::dataSetChangedHandler()
{
	if (!_removed)
	{
		_setAllAvailableVariablesModel(true);
		emit dataSetChanged();
	}
}

void AnalysisForm::setControlIsDependency(QString controlName, bool isDependency)
{
	if(_controls.count(controlName) > 0)
		_controls[controlName]->setItemProperty("isDependency", isDependency);
}

void AnalysisForm::setControlMustContain(QString controlName, QStringList containThis)
{
	if(_controls.count(controlName) > 0)
		_controls[controlName]->setItemProperty("dependencyMustContain", containThis);
}

void AnalysisForm::setMustBe(std::set<std::string> mustBe)
{
	for(const std::string & mustveBeen : _mustBe)
		if(mustBe.count(mustveBeen) == 0)
			setControlIsDependency(mustveBeen, false);

	_mustBe = mustBe;

	for(const std::string & mustBecome : _mustBe)
		setControlIsDependency(mustBecome, true); //Its ok if it does it twice, others will only be notified on change
}

void AnalysisForm::setMustContain(std::map<std::string,std::set<std::string>> mustContain)
{
	//For now ignore specific thing that must be contained
	for(const auto & nameContainsPair : _mustContain)
		if(mustContain.count(nameContainsPair.first) == 0)
			setControlMustContain(nameContainsPair.first, {});

	_mustContain = mustContain;

	for(const auto & nameContainsPair : _mustContain)
		setControlMustContain(nameContainsPair.first, nameContainsPair.second); //Its ok if it does it twice, others will only be notified on change

}
