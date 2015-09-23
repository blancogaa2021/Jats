#include "regressionlinearform.h"
#include "ui_regressionlinearform.h"

RegressionLinearForm::RegressionLinearForm(QWidget *parent) :
	AnalysisForm("RegressionLinearForm", parent),
	ui(new Ui::RegressionLinearForm)
{
	ui->setupUi(this);

	ui->listAvailableFields->setModel(&this->_availableVariablesModel);

	_dependentModel = new TableModelVariablesAssigned();
	_dependentModel->setSource(&_availableVariablesModel);
	_dependentModel->setVariableTypesSuggested(Column::ColumnTypeScale);
	_dependentModel->setVariableTypesAllowed(Column::ColumnTypeScale | Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	ui->dependent->setModel(_dependentModel);

	ui->method->addItem("Enter");
	ui->method->addItem("Backward");
	ui->method->addItem("Forward");
	ui->method->addItem("Stepwise");

	_covariatesModel = new TableModelVariablesAssigned();
	_covariatesModel->setSource(&_availableVariablesModel);
	_covariatesModel->setVariableTypesAllowed(Column::ColumnTypeScale | Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	_covariatesModel->setVariableTypesSuggested(Column::ColumnTypeScale);
	ui->covariates->setModel(_covariatesModel);

	_factorsModel = new TableModelVariablesAssigned();
	_factorsModel->setSource(&_availableVariablesModel);
	_factorsModel->setVariableTypesSuggested(Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	ui->factors->setModel(_factorsModel);

	_wlsWeightsModel = new TableModelVariablesAssigned();
	_wlsWeightsModel->setSource(&_availableVariablesModel);
	_wlsWeightsModel->setVariableTypesSuggested(Column::ColumnTypeScale);
	_wlsWeightsModel->setVariableTypesAllowed(Column::ColumnTypeScale | Column::ColumnTypeNominal | Column::ColumnTypeOrdinal);
	ui->wlsWeights->setModel(_wlsWeightsModel);

	ui->buttonAssignDependent->setSourceAndTarget(ui->listAvailableFields, ui->dependent);
	ui->buttonAssignCovariates->setSourceAndTarget(ui->listAvailableFields, ui->covariates);
	ui->buttonAssignFactors->setSourceAndTarget(ui->listAvailableFields, ui->factors);
	ui->buttonAssignWlsWeights->setSourceAndTarget(ui->listAvailableFields, ui->wlsWeights);

	_modelModel = new TableModelAnovaModel(this);
	_modelModel->setPiecesCanBeAssigned(false);
	ui->modelTerms->setModel(_modelModel);
	ui->modelTerms->hide();

	connect(_covariatesModel, SIGNAL(assignmentsChanging()), this, SLOT(factorsChanging()));
	connect(_covariatesModel, SIGNAL(assignmentsChanged()),  this, SLOT(factorsChanged()));
	connect(_covariatesModel, SIGNAL(assignedTo(Terms)), _modelModel, SLOT(addCovariates(Terms)));
	connect(_covariatesModel, SIGNAL(unassigned(Terms)), _modelModel, SLOT(removeVariables(Terms)));

	connect(_factorsModel, SIGNAL(assignmentsChanging()), this, SLOT(factorsChanging()));
	connect(_factorsModel, SIGNAL(assignmentsChanged()),  this, SLOT(factorsChanged()));
	connect(_factorsModel, SIGNAL(assignedTo(Terms)), _modelModel, SLOT(addFixedFactors(Terms)));
	connect(_factorsModel, SIGNAL(unassigned(Terms)), _modelModel, SLOT(removeVariables(Terms)));

	ui->panelStatistics->hide();
	ui->panelOptions->hide();
    ui->panelAssumptionChecks->hide();

#ifdef QT_NO_DEBUG
    ui->missingValues->hide();
	ui->factors->hide();
	ui->buttonAssignFactors->hide();
	ui->label_13->hide();
	ui->verticalSpacer->hide();
#else
    ui->missingValues->setStyleSheet("background-color: pink ;");
	ui->factors->setStyleSheet("background-color: pink ;");
	ui->buttonAssignFactors->setStyleSheet("background-color: pink ;");
	ui->label_13->setStyleSheet("background-color: pink ;");
#endif

}

void RegressionLinearForm::factorsChanging()
{
	if (_options != NULL)
		_options->blockSignals(true);
}

void RegressionLinearForm::factorsChanged()
{
	if (_options != NULL)
		_options->blockSignals(false);
}

RegressionLinearForm::~RegressionLinearForm()
{
	delete ui;
}

void RegressionLinearForm:: bindTo(Options *options, DataSet *dataSet)
{
	AnalysisForm::bindTo(options, dataSet);
	_modelModel->setVariables(Terms(), Terms(), _covariatesModel->assigned());
}
