#include "descriptivesform.h"
#include "ui_descriptivesform.h"

#include <boost/foreach.hpp>

#include <QDebug>

DescriptivesForm::DescriptivesForm(QWidget *parent) :
	AnalysisForm("DescriptivesForm", parent),
	ui(new Ui::DescriptivesForm)
{
	ui->setupUi(this);

	ui->listAvailableFields->setModel(&_availableVariablesModel);
	ui->listAvailableFields->setDoubleClickTarget(ui->main_fields);

	TableModelVariablesAssigned *model = new TableModelVariablesAssigned(this);
	model->setSource(&_availableVariablesModel);

	ui->main_fields->setModel(model);
	ui->main_fields->setDoubleClickTarget(ui->listAvailableFields);

	ui->buttonAssign_main_fields->setSourceAndTarget(ui->listAvailableFields, ui->main_fields);

	ui->pageStatistics->hide();
	ui->pageCharts->hide();
	ui->pageFormat->hide();
}

DescriptivesForm::~DescriptivesForm()
{
    delete ui;
}

