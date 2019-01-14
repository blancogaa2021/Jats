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

import QtQuick 2.8
import QtQuick.Layouts 1.3
import JASP.Controls 1.0
import JASP.Widgets 1.0
import JASP.Theme 1.0

Form
{
	usesJaspResults: false
	
	CheckBox { name: "simulatepval"; checked: false; visible: false }
	
	VariablesForm
	{
		height: 170
		marginBetweenVariablesLists: 15
		AssignedVariablesList { name: "factor";		title: qsTr("Factor");			singleItem: true; allowedColumns: ["ordinal", "nominal"] }
		AssignedVariablesList { name: "counts";		title: qsTr("Counts");			singleItem: true; allowedColumns: ["ordinal", "scale"] }
		AssignedVariablesList { name: "exProbVar";	title: qsTr("Expected Counts"); singleItem: true; allowedColumns: ["ordinal", "scale"] }
	}
	
	RadioButtonGroup
	{
		id: hypothesisGroup
		name: "hypothesis"
		title: qsTr("Alt. Hypothesis")
		RadioButton { value: "multinomialTest";	text: qsTr("Multinomial test");	 checked: true	}
		RadioButton { value: "expectedProbs";	text: qsTr("χ² test"); id: expectedProbs			}
		
		Chi2TestTableView
		{
			name: "tableWidget"
			width: form.availableWidth - hypothesisGroup.leftPadding
			visible: expectedProbs.checked
			syncModels: "factor"
		}
	}
	
	GridLayout
	{
		GroupBox
		{
			title: qsTr("Additional Statistics")
			CheckBox { name: "descriptives"; text: qsTr("Descriptives"); id: descriptives	}
			RowLayout
			{
				Layout.leftMargin: Theme.indentationLength
				enabled: descriptives.checked
				CheckBox { name: "confidenceInterval"; text: qsTr("Confidence interval")	}
				PercentField { name: "confidenceIntervalInterval"; defaultValue: 95			}
			}
			CheckBox { name: "VovkSellkeMPR"; text: qsTr("Vovk-Dellke maximum p-ratio")		}
		}
		ColumnLayout
		{
			RadioButtonGroup
			{
				name: "countProp"
				title: qsTr("Display")
				RadioButton { value: "descCounts";	text: qsTr("Counts"); checked: true		}
				RadioButton { value: "descProps";	text: qsTr("Proportions")				}
			}
			
			GroupBox
			{
				title: qsTr("Plots")
				CheckBox { name: "descriptivesPlot"; text: qsTr("Descriptives plot");  id: descriptivesPlot }
				PercentField { name: "descriptivesPlotConfidenceInterval"; text: qsTr("Confidence interval");  defaultValue: 95; enabled: descriptivesPlot.checked }
			}
		}
	}
}
