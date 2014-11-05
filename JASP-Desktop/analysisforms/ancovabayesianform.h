#ifndef ANCOVABAYESIANFORM_H
#define ANCOVABAYESIANFORM_H

#include "analysisform.h"

#include "widgets/tablemodelvariablesassigned.h"
#include "widgets/tablemodelanovamodel.h"

namespace Ui {
class AncovaBayesianForm;
}

class AncovaBayesianForm : public AnalysisForm
{
	Q_OBJECT
	
public:
	explicit AncovaBayesianForm(QWidget *parent = 0);
	~AncovaBayesianForm();

private slots:
	void factorsChanged();
	
private:
	Ui::AncovaBayesianForm *ui;

	TableModelVariablesAssigned *_dependentListModel;
	TableModelVariablesAssigned *_fixedFactorsListModel;
	TableModelVariablesAssigned *_randomFactorsListModel;
	TableModelVariablesAssigned *_covariatesListModel;
	TableModelVariablesAssigned *_wlsWeightsListModel;

	TableModelAnovaModel *_anovaModel;

	TableModelVariablesAvailable *_factorsAvailableListModel;
};

#endif // ANCOVABAYESIANFORM_H
