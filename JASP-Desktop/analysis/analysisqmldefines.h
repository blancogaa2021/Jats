#ifndef ANALYSISQMLDEFINES_H
#define ANALYSISQMLDEFINES_H

#include "enumutilities.h"

DECLARE_ENUM(qmlControlType, JASPControl, CheckBox, Switch, TextField, RadioButtonGroup, VariablesListView, ComboBox, RepeatedMeasuresFactorsList, TableView, Slider, TextArea, Button, FactorsForm);
DECLARE_ENUM(qmlListViewType, AssignedVariables, Pairs, Interaction, AvailableVariables, RepeatedMeasures, Layers, AvailableInteraction);
DECLARE_ENUM(qmlDropMode, None, Replace, Insert)
DECLARE_ENUM(qmlAssignType, Cross, MainEffects, Interaction, All2Way, All3Way, All4Way, All5Way)



#endif // ANALYSISQMLDEFINES_H
