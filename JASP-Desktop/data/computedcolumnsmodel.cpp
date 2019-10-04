#include "computedcolumnsmodel.h"
#include "utilities/jsonutilities.h"
#include "utilities/qutils.h"
#include "sharedmemory.h"
#include "log.h"

ComputedColumnsModel::ComputedColumnsModel(Analyses * analyses, DataSetPackage * dataSetPackage)
	: QObject(dataSetPackage), _analyses(analyses), _package(dataSetPackage)
{
	connect(_package,	&DataSetPackage::dataSetChanged,				this,		&ComputedColumnsModel::datasetLoadedChanged					);

	connect(this,		&ComputedColumnsModel::datasetLoadedChanged,	this,		&ComputedColumnsModel::computeColumnJsonChanged				);
	connect(this,		&ComputedColumnsModel::datasetLoadedChanged,	this,		&ComputedColumnsModel::computeColumnRCodeChanged			);
	connect(this,		&ComputedColumnsModel::datasetLoadedChanged,	this,		&ComputedColumnsModel::computeColumnErrorChanged			);
	connect(this,		&ComputedColumnsModel::datasetLoadedChanged,	this,		&ComputedColumnsModel::computeColumnUsesRCodeChanged		);
	connect(this,		&ComputedColumnsModel::datasetLoadedChanged,	this,		&ComputedColumnsModel::computeColumnNameSelectedChanged		);
	connect(this,		&ComputedColumnsModel::refreshColumn,			_package,	&DataSetPackage::refreshColumn,								Qt::QueuedConnection);
	connect(this,		&ComputedColumnsModel::headerDataChanged,		_package,	&DataSetPackage::headerDataChanged,							Qt::QueuedConnection);
	connect(this,		&ComputedColumnsModel::refreshData,				_package,	&DataSetPackage::refresh,									Qt::QueuedConnection);

	connect(_analyses,	&Analyses::requestComputedColumnCreation,		this,		&ComputedColumnsModel::requestComputedColumnCreation,		Qt::UniqueConnection);
	connect(_analyses,	&Analyses::requestColumnCreation,				this,		&ComputedColumnsModel::requestColumnCreation,				Qt::UniqueConnection);
	connect(_analyses,	&Analyses::requestComputedColumnDestruction,	this,		&ComputedColumnsModel::requestComputedColumnDestruction,	Qt::UniqueConnection);
	connect(_analyses,	&Analyses::analysisRemoved,						this,		&ComputedColumnsModel::analysisRemoved						);

}

QString ComputedColumnsModel::computeColumnRCode()
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return "";

	return QString::fromStdString(computedColumns()->getRCode(_currentlySelectedName.toStdString()));
}

QString ComputedColumnsModel::computeColumnRCodeCommentStripped()
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return "";

	return QString::fromStdString(computedColumns()->getRCodeCommentStripped(_currentlySelectedName.toStdString()));
}

bool ComputedColumnsModel::computeColumnUsesRCode()
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return "";

	return computedColumns()->usesRCode(_currentlySelectedName.toStdString());
}

QString ComputedColumnsModel::computeColumnJson()
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return "";

	return QString::fromStdString(computedColumns()->getConstructorJson(_currentlySelectedName.toStdString()));
}

QString ComputedColumnsModel::computeColumnError()
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return "";

	return QString::fromStdString(computedColumns()->getError(_currentlySelectedName.toStdString()));
}

QString ComputedColumnsModel::computeColumnNameSelected()
{
	return _currentlySelectedName;
}

void ComputedColumnsModel::setComputeColumnRCode(QString newCode)
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return;

	if(computedColumns()->setRCode(_currentlySelectedName.toStdString(), newCode.toStdString()))
		emit computeColumnRCodeChanged();

	invalidate(_currentlySelectedName);
}


void ComputedColumnsModel::setComputeColumnJson(QString newJson)
{
	if(_currentlySelectedName == "" || computedColumns() == nullptr)
		return;

	if(computedColumns()->setConstructorJson(_currentlySelectedName.toStdString(), newJson.toStdString()))
		emit computeColumnJsonChanged();
}

void ComputedColumnsModel::setComputeColumnNameSelected(QString newName)
{
	if(_currentlySelectedName != newName)
	{
		_currentlySelectedName = newName;

		emit computeColumnJsonChanged();
		emit computeColumnRCodeChanged();
		emit computeColumnErrorChanged();
		emit computeColumnUsesRCodeChanged();
		emit computeColumnNameSelectedChanged();
	}
}

bool ComputedColumnsModel::areLoopDependenciesOk(std::string columnName)
{

	return areLoopDependenciesOk(columnName, (*computedColumns())[columnName].analysis() != nullptr ? "" : (*computedColumns())[columnName].rCodeCommentStripped());
}

bool ComputedColumnsModel::areLoopDependenciesOk(std::string columnName, std::string code)
{
	try
	{
		(*computedColumns())[columnName].checkForLoopInDepenedencies(code);
	}
	catch(std::logic_error & e)
	{
		validate(QString::fromStdString(columnName)); //To stop loading gif

		if(computedColumns()->setError(columnName, e.what()) && _currentlySelectedName.toStdString() == columnName)
			emit computeColumnErrorChanged();


		return false;
	}

	return true;
}

void ComputedColumnsModel::emitSendComputeCode(QString columnName, QString code, columnType colType)
{
	if(areLoopDependenciesOk(columnName.toStdString(), code.toStdString()))
		emit sendComputeCode(columnName, code, colType);
}

void ComputedColumnsModel::sendCode(QString code, QString json)
{
	setComputeColumnJson(json);

	if(code == "")
		setComputeColumnRCode(code);
	else
		sendCode(code);
}


void ComputedColumnsModel::sendCode(QString code)
{
	std::string columnName = _currentlySelectedName.toStdString();
	setComputeColumnRCode(code);
	emitSendComputeCode(_currentlySelectedName, computeColumnRCodeCommentStripped(), _package->getColumnType(columnName));
}

void ComputedColumnsModel::validate(QString columnName)
{
	try
	{
		(*computedColumns())[columnName.toStdString()].validate();
		emitHeaderDataChanged(columnName);
	} catch(columnNotFound & ){}
}

void ComputedColumnsModel::invalidate(QString columnName)
{
	try
	{
		(*computedColumns())[columnName.toStdString()].invalidate();
		emitHeaderDataChanged(columnName);
	} catch(columnNotFound & ){}
}

void ComputedColumnsModel::invalidateDependents(std::string columnName)
{
	for(ComputedColumn * col : *computedColumns())
		if(col->dependsOn(columnName))
			invalidate(QString::fromStdString(col->name()));
}


void ComputedColumnsModel::emitHeaderDataChanged(QString name)
{
	try
	{
		int index = _package->getColumnIndex(name.toStdString());
		emit headerDataChanged(Qt::Horizontal, index, index);
	}
	catch(...){}
}

void ComputedColumnsModel::revertToDefaultInvalidatedColumns()
{
	for(ComputedColumn * col : *computedColumns())
		if(col->isInvalidated())
			_package->columnSetDefaultValues(col->name());
}

void ComputedColumnsModel::computeColumnSucceeded(QString columnNameQ, QString warningQ, bool dataChanged)
{
	std::string columnName	= columnNameQ.toStdString(),
				warning		= warningQ.toStdString();

	bool shouldNotifyQML = _currentlySelectedName.toStdString() == columnName;

	if(computedColumns()->setError(columnName, warning) && shouldNotifyQML)
		emit computeColumnErrorChanged();

	if(dataChanged)
			emit refreshColumn(tq(columnName));

	validate(QString::fromStdString(columnName));

	if(dataChanged)
		checkForDependentColumnsToBeSent(columnName);
}

void ComputedColumnsModel::computeColumnFailed(QString columnNameQ, QString errorQ)
{
	std::string columnName	= columnNameQ.toStdString(),
				error		= errorQ.toStdString();

	bool shouldNotifyQML = _currentlySelectedName.toStdString() == columnName;

	if(areLoopDependenciesOk(columnName) && computedColumns()->setError(columnName, error) && shouldNotifyQML)
		emit computeColumnErrorChanged();

	_package->columnSetDefaultValues(columnName);

	validate(QString::fromStdString(columnName));
	invalidateDependents(columnName);

}

///Called from datatype changed
void ComputedColumnsModel::recomputeColumn(std::string columnName)
{
	if(!_package->isColumnComputed(columnName))
		return;

	//It will be found because we just checked for it in isColumnComputed
	ComputedColumn * col = &((*computedColumns())[columnName]);

	if(col->codeType() == ComputedColumn::computedType::analysis || col->codeType() == ComputedColumn::computedType::analysisNotComputed)
		return;

	_package->columnSetDefaultValues(columnName);
	computedColumns()->findAllColumnNames();


	checkForDependentColumnsToBeSent(columnName, true);
}

void ComputedColumnsModel::checkForDependentColumnsToBeSent(std::string columnName, bool refreshMe)
{
	for(ComputedColumn * col : *computedColumns())
		if(	col->codeType() != ComputedColumn::computedType::analysis				&&
			col->codeType() != ComputedColumn::computedType::analysisNotComputed	&&
			(
					col->dependsOn(columnName) ||
					(refreshMe && col->name() == columnName)
			) )
			invalidate(QString::fromStdString(col->name()));

	for(ComputedColumn * col : *computedColumns())
		if(	col->codeType() != ComputedColumn::computedType::analysis				&&
			col->codeType() != ComputedColumn::computedType::analysisNotComputed	&&
			col->iShouldBeSentAgain() )
			emitSendComputeCode(QString::fromStdString(col->name()), QString::fromStdString(col->rCodeCommentStripped()), _package->getColumnType(columnName));

	checkForDependentAnalyses(columnName);
}

void ComputedColumnsModel::checkForDependentAnalyses(std::string columnName)
{
	assert(_analyses != nullptr);

	_analyses->applyToAll([&](Analysis * analysis)
		{
			std::set<std::string> usedCols = analysis->usedVariables();

			if(usedCols.count(columnName) > 0)
			{
				bool allColsValidated = true;

				for(ComputedColumn * col : *computedColumns())
					if(usedCols.count(col->name()) > 0 && col->isInvalidated())
						allColsValidated = false;

				if(allColsValidated)
					analysis->refresh();
			}
		});
}

void ComputedColumnsModel::removeColumn()
{
	if(_currentlySelectedName == "")
		return;

	requestComputedColumnDestruction(_currentlySelectedName);

	setComputeColumnNameSelected("");
}

void ComputedColumnsModel::packageSynchronized(const std::vector<std::string> & changedColumns, const std::vector<std::string> & missingColumns, const std::map<std::string, std::string> & changeNameColumns, bool rowCountChanged)
{
	computedColumns()->findAllColumnNames();

	for(ComputedColumn * col : *computedColumns())
	{
		bool invalidateMe = rowCountChanged;

		for(const std::string & changed : changedColumns)
			if(col->dependsOn(changed, false))
				invalidateMe = true;

		bool containsAChangedName = false;
		for(const auto & changedNames : changeNameColumns)
			if(col->dependsOn(changedNames.first, false))
				containsAChangedName = true;

		if(containsAChangedName)
		{
			invalidateMe = true;
			col->replaceChangedColumnNamesInRCode(changeNameColumns);
			col->setConstructorJson(JsonUtilities::replaceColumnNamesInDragNDropFilterJSON(col->constructorJson(), changeNameColumns));

			if(col->name() == _currentlySelectedName.toStdString())
			{
				emit computeColumnJsonChanged();
				emit computeColumnRCodeChanged();
			}
		}

		for(const std::string & missing : missingColumns)
			if(col->dependsOn(missing, false))
			{
				invalidateMe = true;
				col->setConstructorJson(JsonUtilities::removeColumnsFromDragNDropFilterJSON(col->constructorJson(), missing));

				if(col->codeType() == ComputedColumn::computedType::constructorCode)
					col->setRCode("stop('Certain columns where removed from the definition of this computed column.')");

				if(col->name() == _currentlySelectedName.toStdString())
				{
					emit computeColumnJsonChanged();
					emit computeColumnRCodeChanged();
				}
			}



		if(invalidateMe)
			invalidate(QString::fromStdString(col->name()));

	}

	computedColumns()->findAllColumnNames();

	for(ComputedColumn * col : *computedColumns())
	{
		col->findDependencies(); //columnNames might have changed right? so check it again

		if(col->iShouldBeSentAgain())
			emitSendComputeCode(QString::fromStdString(col->name()), QString::fromStdString(col->rCodeCommentStripped()), _package->getColumnType(col->name()));
	}
}


ComputedColumn * ComputedColumnsModel::createComputedColumn(QString name, int colType, ComputedColumn::computedType computeType, Analysis * analysis)
{
	bool success			= false;

	bool	createActualComputedColumn	= computeType != ComputedColumn::computedType::analysisNotComputed,
			showComputedColumn			= createActualComputedColumn && computeType != ComputedColumn::computedType::analysis;

	ComputedColumn  * createdColumn = nullptr;

	if(createActualComputedColumn)
	{
		createdColumn = computedColumns()->createComputedColumn(name.toStdString(), columnType(colType), computeType);
		createdColumn->setAnalysis(analysis);
	}
	else
		computedColumns()->createColumn(name.toStdString(), columnType(colType));

	emit refreshData();



	if(createActualComputedColumn)		setLastCreatedColumn(name);
	else								emit dataColumnAdded(name);
	if(showComputedColumn)				setShowThisColumn(name);

	return createdColumn;
}

ComputedColumn *	ComputedColumnsModel::requestComputedColumnCreation(QString columnName, Analysis * analysis)
{
	if(!_package->isColumnNameFree(columnName.toStdString()))
		return nullptr;

	return createComputedColumn(columnName, int(columnType::scale), ComputedColumn::computedType::analysis, analysis);
}

void ComputedColumnsModel::requestColumnCreation(QString columnName, Analysis * analysis, int columnType)
{
	if(_package->isColumnNameFree(columnName.toStdString()))
		createComputedColumn(columnName, columnType, ComputedColumn::computedType::analysisNotComputed, analysis);
}


void ComputedColumnsModel::requestComputedColumnDestruction(QString columnNameQ)
{
	if(columnNameQ == "")
		return;

	std::string columnName = columnNameQ.toStdString();

	int index = _package->getColumnIndex(columnName);

	computedColumns()->removeComputedColumn(columnName);

	emit headerDataChanged(Qt::Horizontal, index, _package->columnCount() + 1);

	_analyses->applyToAll([&](Analysis * analysis)
		{ analysis->removeUsedVariable(columnName); } );

	checkForDependentColumnsToBeSent(columnName);

	if(columnNameQ == lastCreatedColumn())
		setLastCreatedColumn("");

	if(columnNameQ == showThisColumn())
		setShowThisColumn("");
}

bool ComputedColumnsModel::showAnalysisFormForColumn(QString columnName)
{
	try
	{
		ComputedColumn * col = &(*computedColumns())[columnName.toStdString()];

		if(col->analysis() != nullptr)
		{
			emit showAnalysisForm(col->analysis());
			return true;
		}

	}
	catch(...) {}

	return false;
}

void ComputedColumnsModel::setLastCreatedColumn(QString lastCreatedColumn)
{
	if (_lastCreatedColumn == lastCreatedColumn)
		return;

	_lastCreatedColumn = lastCreatedColumn;
	emit lastCreatedColumnChanged(_lastCreatedColumn);
}

void ComputedColumnsModel::analysisRemoved(Analysis * analysis)
{
	if(computedColumns() == nullptr)
		return;

	std::set<QString> colsToRemove;

	for(auto * col : *computedColumns())
		if(col->analysis() == analysis)
			colsToRemove.insert(QString::fromStdString(col->name()));

	for(const QString & col : colsToRemove)
		requestComputedColumnDestruction(col);
}

void ComputedColumnsModel::setShowThisColumn(QString showThisColumn)
{
	if (_showThisColumn == showThisColumn)
		return;

	_showThisColumn = showThisColumn;
	emit showThisColumnChanged(_showThisColumn);
}
