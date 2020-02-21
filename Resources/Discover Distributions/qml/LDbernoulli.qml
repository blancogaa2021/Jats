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

Form {
	id: form

	Section
	{
		expanded: true
		title: qsTr("Show Distribution")
		Group
		{
			title: qsTr("Parameter")
			Layout.columnSpan: 2
			DoubleField{ name: "prob"; label: qsTr("p"); id: prob; min: 0; max: 1; defaultValue: 0.5 }
		}

		Group
		{
			title: qsTr("Display")
			CheckBox{ label: qsTr("Explanatory text"); name: "explanatoryText"}
			CheckBox{ label: qsTr("Parameters, support, and moments"); name: "parsSupportMoments" }
			CheckBox{ label: qsTr("Formulas"); name: "formulas"; visible: false}
			CheckBox{ label: qsTr("Probability mass function"); id: plotPMF; name: "plotPMF"; checked: true }
			CheckBox{ label: qsTr("Cumulative distribution function"); id: plotCMF; name: "plotCMF"; checked: false }
		}

		Group
		{
			title: qsTr("Options")
			enabled: plotPMF.checked || plotCMF.checked
			visible: false
			Group
			{
				title: qsTr("Highlight")
				Group
				{
					columns: 2
					CheckBox{ name: "highlightDensity"; label: qsTr("Mass"); id: highlightDensity }
					CheckBox{ name: "highlightProbability"; label: qsTr("Cumulative Probability"); id: highlightProbability }
				}
				Row
				{
					spacing: jaspTheme.columnGroupSpacing
					IntegerField
					{
						name: "min"; label: qsTr("Limits"); afterLabel: qsTr("≤ X ≤"); id: min;
						negativeValues: false; defaultValue: 0; max: parseInt(max.value)
					}
					IntegerField
					{
						name: "max"; label: ""; id: max;
						min: parseInt(min.value); defaultValue: 5
					}
				}
			}

		}
	}

	Section
	{
		title: qsTr("Generate and Display Data")
		Group
		{
			title: qsTr("Generate new variable from Bernoulli") + " (p = " + prob.value + ")"
			AddColumnField{ name: "newVariableName"; text: qsTr("Variable name: "); fieldWidth: 120; placeholderText: qsTr("e.g., random bernoulli") }
			IntegerField{   name: "sampleSize"; label: qsTr("Number of samples: "); min: 1; defaultValue: dataSetModel.rowCount(); max: dataSetModel.rowCount()}
			Button{name: "simulateNowButton"; label: qsTr("Draw samples"); id: simulateNowButton; onClicked:{
					if (simulateNow.checked) simulateNow.checked = false; else simulateNow.checked = true
				}}
			CheckBox{name: "simulateNow"; visible: false; id: simulateNow}
		}
		VariablesForm
		{
			height: 100
			visible: true
			AvailableVariablesList { name: "allVariables" }
			AssignedVariablesList  { name: "variable"; label: qsTr("Get variable from data set"); singleVariable: true }
		}

		Group
		{
			title: qsTr("Statistics")
			CheckBox{ name: "summary"; label: qsTr("Descriptives"); checked: true }
		}

		Group
		{
			title: qsTr("Plots")
			CheckBox
			{
				name: "histogram";  label: qsTr("Bar plot"); childrenOnSameRow: true
			}
			CheckBox{ name: "ecdf"; label: qsTr("Empirical cumulative distribution"); visible: false }
		}
	}

	Section
	{
		title: qsTr("Estimate Parameters")

		Group
		{
			CheckBox{ name: "methodMLE";      label: qsTr("Maximum likelihood"); visible: true  }
			CheckBox{ name: "methodMoments";  label: qsTr("Method of moments");  visible: false }
			CheckBox{ name: "methodUnbiased"; label: qsTr("Unbiased estimator"); visible: false }
		}

		Group
		{
			title: qsTr("Output")
			CheckBox
			{
				name: "outputEstimates"; label: qsTr("Estimates"); checked: true
				CheckBox{ name: "outputSE"; label: qsTr("Std. error"); checked: false }
				CheckBox
				{
					name: "ciInterval"; label: qsTr("Confidence interval"); childrenOnSameRow: true
					PercentField{ name: "ciIntervalInterval"; label: ""; defaultValue: 95 }
				}
			}

			CheckBox{ name: "outputVarCov"; label: qsTr("Variance-covariance"); checked: false; visible: false }
			CheckBox{ name: "outputCor";    label: qsTr("Correlation"); checked: false; visible: false }
		}
	}
	Section
	{
		title: qsTr("Assess Fit")

		Group
		{
			title: qsTr("Plots")
			columns: 1
			CheckBox{ name: "estPMF"; label: qsTr("Histogram vs. theoretical pmf") }
			CheckBox{ name: "qqplot"; label: qsTr("Q-Q plot"); visible: false      }
			CheckBox{ name: "estCDF"; label: qsTr("Empirical vs. theoretical cdf") }
			CheckBox{ name: "ppplot"; label: qsTr("P-P plot")                      }
		}
	}
}
