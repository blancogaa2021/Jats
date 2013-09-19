#ifndef BACKSTAGEFORM_H
#define BACKSTAGEFORM_H

#include <QWidget>

#include "asyncloader.h"
#include "dataset.h"

namespace Ui {
class BackStageForm;
}

class BackStageForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit BackStageForm(QWidget *parent = 0);
    ~BackStageForm();

signals:
	void dataSetSelected(QString filename);
    
private:
    Ui::BackStageForm *ui;

private slots:
    void fileItemSelected();

};

#endif // BACKSTAGEFORM_H
