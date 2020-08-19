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

#ifndef ANALYSISFORM_H
#define ANALYSISFORM_H

#include <QMap>

#include "dataset.h"
#include "options/bound.h"
#include "options/options.h"
#include "options/optionvariables.h"

#include "analysis/options/variableinfo.h"
#include "analysis.h"

#include <QQuickItem>

#include "analysis.h"
#include "boundqmlitem.h"
#include "widgets/listmodel.h"
#include "options/variableinfo.h"
#include "widgets/listmodeltermsavailable.h"
#include "gui/messageforwarder.h"
#include "utilities/qutils.h"



class ListModelTermsAssigned;
class BoundQMLItem;
class JASPControlBase;
class QMLExpander;

class AnalysisForm : public QQuickItem, public VariableInfoProvider
{
	Q_OBJECT
	Q_PROPERTY(QString		errors				READ errors											NOTIFY errorsChanged			)
	Q_PROPERTY(QString		warnings			READ warnings										NOTIFY warningsChanged			)
	Q_PROPERTY(bool			needsRefresh		READ needsRefresh									NOTIFY needsRefreshChanged		)
	Q_PROPERTY(bool			hasVolatileNotes	READ hasVolatileNotes								NOTIFY hasVolatileNotesChanged	)
	Q_PROPERTY(bool			runOnChange			READ runOnChange		WRITE setRunOnChange		NOTIFY runOnChangeChanged		)
	Q_PROPERTY(QString		info				READ info				WRITE setInfo				NOTIFY infoChanged				)
	Q_PROPERTY(QString		helpMD				READ helpMD											NOTIFY helpMDChanged			)
	Q_PROPERTY(QVariant		analysis			READ analysis			WRITE setAnalysis			NOTIFY analysisChanged			)

public:
	explicit					AnalysisForm(QQuickItem * = nullptr);
				void			bindTo();
				void			unbind();

				void			runRScript(QString script, QString controlName, bool whiteListedVersion);
				
				void			itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;

				void			setMustBe(		std::set<std::string>						mustBe);
				void			setMustContain(	std::map<std::string,std::set<std::string>> mustContain);

				bool			runOnChange()	{ return _runOnChange; }
				void			setRunOnChange(bool change);

				bool			runWhenThisOptionIsChanged(Option* option);
					
public slots:
				void			runScriptRequestDone(const QString& result, const QString& requestId);
				void			dataSetChangedHandler();
				void			dataSetColumnsChangedHandler();
				void			replaceVariableNameInListModels(const std::string & oldName, const std::string & newName);
				void			setInfo(QString info);
				void			setAnalysis(QVariant analysis);


signals:
				void			sendRScript(QString script, int key);
				void			formChanged(Analysis* analysis);
				void			formCompleted();
				void			dataSetChanged();
				void			refreshTableViewModels();
				void			errorMessagesItemChanged();
				void			languageChanged();
				void			needsRefreshChanged();
				void			hasVolatileNotesChanged();
				void			runOnChangeChanged();
				void			valueChanged(JASPControlBase* item);
				void			infoChanged();
				void			helpMDChanged();
				void			errorsChanged();
				void			warningsChanged();
				void			analysisChanged();

protected:
				QVariant		requestInfo(const Term &term, VariableInfo::InfoType info) const override;

public:
	ListModel			*	getRelatedModel(QMLListView* listView)	{ return _relatedModelMap[listView]; }
	ListModel			*	getModel(const QString& modelName)		{ return _modelMap.count(modelName) > 0 ? _modelMap[modelName] : nullptr; } // Maps create elements if they do not exist yet
	Options				*	getAnalysisOptions()					{ return _analysis->options(); }
	JASPControlWrapper	*	getControl(const QString& name)			{ return _controls[name]; }
	void					addListView(QMLListView* listView, QMLListView* sourceListView);
	QMLExpander			*	nextExpander(QMLExpander* expander)		{ return _nextExpanderMap[expander]; }
	BoundQMLItem		*	getBoundItem(Option* option)			{ return _optionControlMap[option]; }

	Options				*	options() { return _options; }
	void					addControl(JASPControlBase* control);

	Q_INVOKABLE void		clearFormErrors();
	Q_INVOKABLE void		clearFormWarnings();
	Q_INVOKABLE void		reset();
	Q_INVOKABLE void		exportResults();
	Q_INVOKABLE void		addFormError(const QString& message);
	Q_INVOKABLE void		refreshAnalysis();
	Q_INVOKABLE void		runAnalysis();

	void		addControlError(JASPControlBase* control, QString message, bool temporary = false, bool warning = false);
	void		clearControlError(JASPControlBase* control);
	void		cleanUpForm();
	void		refreshAvailableVariablesModels() { _setAllAvailableVariablesModel(true); }

	bool		hasError();

	bool		isOwnComputedColumn(const QString& col)			const	{ return _computedColumns.contains(col); }
	void		addOwnComputedColumn(const QString& col)				{ _computedColumns.push_back(col); }
	void		removeOwnComputedColumn(const QString& col)				{ _computedColumns.removeAll(col); }

	bool		needsRefresh()		const;
	bool		hasVolatileNotes()	const;

	QString		info()				const { return _info; }
	QString		helpMD()			const;
	QString		metaHelpMD()		const;
	QString		errors()			const {	return msgsListToString(_formErrors);	}
	QString		warnings()			const { return msgsListToString(_formWarnings);	}
	QVariant	analysis()			const { return QVariant::fromValue(_analysis);	}

protected:
	void		_setAllAvailableVariablesModel(bool refreshAssigned = false);
	QString		msgsListToString(const QStringList & list) const;

private:
	void		_addControlWrapper(JASPControlWrapper* controlWrapper);
	void		_setUpControls();
	void		_setUpRelatedModels();
	void		_setUpItems();
	void		_orderExpanders();
	QString		_getControlLabel(QString controlName);
	void		_addLoadingError(QStringList wrongJson);
	void		setControlIsDependency(	QString controlName, bool isDependency);
	void		setControlMustContain(	QString controlName, QStringList containThis);
	void		setControlIsDependency(	std::string controlName, bool isDependency)					{ setControlIsDependency(tq(controlName), isDependency);	}
	void		setControlMustContain(	std::string controlName, std::set<std::string> containThis)	{ setControlMustContain(tq(controlName), tql(containThis)); }
	QQuickItem* _getControlErrorMessageOfControl(JASPControlBase* jaspControl);
	void		setAnalysisUp();

private slots:
	   void		formCompletedHandler();
	   void		_formCompletedHandler();

protected:
	Analysis								*	_analysis			= nullptr;
	Options									*	_options			= nullptr;
	QMap<QString, JASPControlWrapper* >			_controls;

	///Ordered on dependencies within QML, aka an assigned variables list depends on the available list it is connected to.
	QVector<JASPControlWrapper*>				_dependsOrderedCtrls;
	QMap<Option*, BoundQMLItem*>				_optionControlMap;
	QMap<QMLListView*, ListModel* >				_relatedModelMap;
	QMap<QString, ListModel* >					_modelMap;
	QVector<QMLExpander*>						_expanders;
	QMap<QMLExpander*, QMLExpander*>			_nextExpanderMap;
	QMap<JASPControlWrapper*, QMLExpander*>		_controlExpanderMap;
	bool										_removed = false;
	std::set<std::string>						_mustBe;
	std::map<std::string,std::set<std::string>>	_mustContain;
	
private:
	std::vector<ListModelTermsAvailable*>		_allAvailableVariablesModels,
												_allAvailableVariablesModelsWithSource;
	QStringList									_formErrors,
												_formWarnings;

	QQmlComponent*								_controlErrorMessageComponent = nullptr;
	QList<QQuickItem*>							_controlErrorMessageCache;
	QList<QString>								_computedColumns;
	bool										_runOnChange	= true,
												_formCompleted = false;
	QString										_info;
};

#endif // ANALYSISFORM_H
