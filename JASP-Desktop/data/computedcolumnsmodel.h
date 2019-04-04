#ifndef COMPUTEDCOLUMNSCODEITEM_H
#define COMPUTEDCOLUMNSCODEITEM_H

#include <QQuickItem>
#include <QObject>
#include "computedcolumns.h"
#include "datasetpackage.h"
#include "analysis/analyses.h"

class ComputedColumnsModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool		computeColumnUsesRCode		READ computeColumnUsesRCode													NOTIFY computeColumnUsesRCodeChanged	)
	Q_PROPERTY(QString	computeColumnRCode			READ computeColumnRCode				WRITE setComputeColumnRCode				NOTIFY computeColumnRCodeChanged		)
	Q_PROPERTY(QString	computeColumnJson			READ computeColumnJson														NOTIFY computeColumnJsonChanged			)
	Q_PROPERTY(QString	computeColumnError			READ computeColumnError														NOTIFY computeColumnErrorChanged		)
	Q_PROPERTY(QString	computeColumnNameSelected	READ computeColumnNameSelected		WRITE setComputeColumnNameSelected		NOTIFY computeColumnNameSelectedChanged )
	Q_PROPERTY(bool		datasetLoaded				READ datasetLoaded															NOTIFY datasetLoadedChanged				)
	Q_PROPERTY(QString lastCreatedColumn			READ lastCreatedColumn														NOTIFY lastCreatedColumnChanged			)

public:
	explicit	ComputedColumnsModel(Analyses * analyses, QObject * parent);

				bool	datasetLoaded()					{ return _package != NULL; }
				QString	computeColumnRCode();
				QString computeColumnRCodeCommentStripped();
				QString computeColumnError();
				QString computeColumnNameSelected();
				QString computeColumnJson();
				QString lastCreatedColumn() const { return _lastCreatedColumn; }
				bool	computeColumnUsesRCode();

				void	setComputeColumnRCode(QString newCode);
				void	setDataSetPackage(DataSetPackage * package);
				void	setComputeColumnNameSelected(QString newName);
				void	setComputeColumnJson(QString newJson);

				void	packageSynchronized(const std::vector<std::string> & changedColumns, const std::vector<std::string> & missingColumns, const std::map<std::string, std::string> & changeNameColumns, bool rowCountChanged);

	Q_INVOKABLE	void	removeColumn();
	Q_INVOKABLE void	sendCode(QString code);
	Q_INVOKABLE void	sendCode(QString code, QString json);

	Q_INVOKABLE bool	isColumnNameFree(QString name)						{ return _package->isColumnNameFree(name.toStdString()); }

				ComputedColumn*		createComputedColumn(QString name, int columnType, ComputedColumn::computedType computeType, Analysis * analysis = nullptr);
	Q_INVOKABLE void				createComputedColumn(QString name, int columnType, bool jsonPlease)									{ createComputedColumn(name, columnType, jsonPlease ? ComputedColumn::computedType::constructorCode : ComputedColumn::computedType::rCode);	}

				ComputedColumns*	computedColumnsPointer() { return _package->computedColumnsPointer(); }

				bool				areLoopDependenciesOk(std::string columnName);
				bool				areLoopDependenciesOk(std::string columnName, std::string code);

	Q_INVOKABLE bool	showAnalysisFormForColumn(QString columnName);



private:
				void	validate(QString name);
				void	setAnalyses(Analyses * analyses)				{ _analyses = analyses; }
				void	emitHeaderDataChanged(QString name);
				void	revertToDefaultInvalidatedColumns();
				void	checkForDependentAnalyses(std::string columnName);
				void	invalidate(QString name);
				void	invalidateDependents(std::string columnName);
				void	checkForDependentColumnsToBeSent(std::string columnName, bool refreshMe = false);
				void	emitSendComputeCode(QString columnName, QString code, Column::ColumnType colType);
				void	clearColumn(std::string columnName);
signals:
				void	datasetLoadedChanged();
				void	computeColumnRCodeChanged();
				void	computeColumnErrorChanged();
				void	computeColumnJsonChanged();
				void	refreshColumn(Column * column);
				void	computeColumnNameSelectedChanged();
				void	headerDataChanged(Qt::Orientation orientation, int first, int last);
				void	sendComputeCode(QString columnName, QString code, Column::ColumnType columnType);
				void	computeColumnUsesRCodeChanged();
				void	dataSetChanged(DataSet * newDataSet);
				void	refreshData();
				void	showAnalysisForm(Analysis *analysis);
				void	lastCreatedColumnChanged(QString lastCreatedColumn);

public slots:
				void				computeColumnSucceeded(QString columnName, QString warning, bool dataChanged);
				void				computeColumnFailed(QString columnName, QString error);
				void				checkForDependentColumnsToBeSentSlot(std::string columnName)					{ checkForDependentColumnsToBeSent(columnName, false); }
				ComputedColumn *	requestComputedColumnCreation(QString columnName, Analysis * analysis);
				void				requestComputedColumnDestruction(QString columnName);
				void				recomputeColumn(std::string columnName);
				void				setLastCreatedColumn(QString lastCreatedColumn);

private:

	QString				_currentlySelectedName	= "",
						_lastCreatedColumn		= "";
	ComputedColumns		*_computedColumns		= nullptr;
	DataSetPackage		*_package				= nullptr;
	Analyses			*_analyses				= nullptr;
};

#endif // COMPUTEDCOLUMNSCODEITEM_H

