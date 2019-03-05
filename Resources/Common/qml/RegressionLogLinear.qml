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

Form
{
	usesJaspResults: false
	
	VariablesForm
	{
		AvailableVariablesList { name: "allVariablesList" }		
		AssignedVariablesList { name: "counts";		title: qsTr("Counts (optional)"); singleVariable: true	}
		AssignedVariablesList { name: "factors";	title: qsTr("Factors"); itemType: "fixedFactors"; allowedColumns: ["ordinal", "nominal"] }			
	}
	
	Section
	{
		title: qsTr("Model")
		
		VariablesForm
		{
			height: 200
			AvailableVariablesList { name: "availableTerms"; title: qsTr("Components"); width: parent.width / 4; source: ['factors'] }
			AssignedVariablesList {  name: "modelTerms";	 title: qsTr("Model terms"); width: parent.width * 5 / 9; listViewType: "Interaction" }
		}
	}
	
	Section
	{
		title: qsTr("Statistics")
		
		Group
		{
			title: qsTr("Regression Coefficient")
			CheckBox { name: "regressionCoefficientsEstimates";		label: qsTr("Estimates") }
			CheckBox
			{
				name: "regressionCoefficientsConfidenceIntervals";	label: qsTr("Confidence intervals")
				PercentField { name: "regressionCoefficientsConfidenceIntervalsInterval"; label: qsTr("Interval"); defaultValue: 95 }
			}
		}
		CheckBox { name: "VovkSellkeMPR"; label: qsTr("Vovk-Sellke maximum p-ratio") }
	}
}
