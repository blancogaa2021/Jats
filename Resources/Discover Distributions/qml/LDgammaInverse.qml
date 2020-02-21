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

import "./common" as LD

Form {
	id: form

	Section
	{
		expanded: true
		title: qsTr("Show Distribution")
		Group
		{
			Layout.columnSpan: 2
			DropDown
			{
				name: "parametrization"
				id:   parametrization
				indexDefaultValue: 0
				label: qsTr("Parameters")
				values: [
					{ label: "k, θ",  value: "scale"},
					{ label: "α, β",  value: "rate" },
					{ label: "k, μ",  value: "mean"  }
				]
				visible: true
			}

			Group
			{
				columns: 1
				DoubleField
				{
					name: "shape"
					label: ["k", "α", "k"][parametrization.currentIndex]
					id: shape
					negativeValues: false
					defaultValue: 1
				}
				DoubleField
				{
					name: "par2"
					label: ["θ", "β", "μ"][parametrization.currentIndex]
					id: par2
					negativeValues: false
					defaultValue: 1
				}
			}

		}

		Group
		{
			title: qsTr("Display")
			CheckBox{ label: qsTr("Explanatory text"); name: "explanatoryText"}
			CheckBox{ label: qsTr("Parameters, support, and moments"); name: "parsSupportMoments" }
			CheckBox{ label: qsTr("Formulas"); name: "formulas"; visible: false}
			CheckBox{ label: qsTr("Probability density function"); id: plotPDF; name: "plotPDF"; checked: true }
			CheckBox{ label: qsTr("Cumulative distribution function"); id: plotCDF; name: "plotCDF"; checked: false }
			CheckBox{ label: qsTr("Quantile function"); id: plotQF; name: "plotQF"; checked: false }
		}

		LD.LDOptions
		{
			enabled				: plotPDF.checked || plotCDF.checked
			negativeValues		: false
			intervalMinmaxMin	: 1
			intervalMinmaxMax	: 2
			intervalLowerMax	: 1
			intervalUpperMin	: 1
		}
	}

	Section
	{
		title: qsTr("Generate and Display Data")
		Group
		{
			title: qsTr("Generate new variable from InvGamma") + " (" + ["k = ", "α = ", "k = "][parametrization.currentIndex] +
				   shape.value + [",θ = ", ",β = ", ",μ = "][parametrization.currentIndex] + par2.value + ")"
			AddColumnField{ name: "newVariableName"; text: qsTr("Variable name: "); fieldWidth: 120; placeholderText: qsTr("e.g., random invgamma") }
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
			AssignedVariablesList  { name: "variable"; label: qsTr("Get variable from data set"); allowedColumns: ["scale"]; singleVariable: true }
		}

		Group
		{
			title: qsTr("Statistics")
			CheckBox{ name: "summary"; label: qsTr("Descriptives"); checked: true  }
			CheckBox
			{
				name: "moments"; label: qsTr("First"); childrenOnSameRow: true
				IntegerField{name: "momentsUpTo"; afterLabel: qsTr("observed moments"); defaultValue: 2; min: 1; max: 10}
			}
		}

		Group
		{
			title: qsTr("Plots")
			CheckBox
			{
				name: "histogram";  label: qsTr("Histogram with"); childrenOnSameRow: true
				IntegerField{ name: "histogramBins"; afterLabel: qsTr("bins"); defaultValue: 30 }
			}
			CheckBox{ name: "ecdf"; label: qsTr("Empirical cumulative distribution") }
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
			columns: 2
			CheckBox{ name: "estPDF"; label: qsTr("Histogram vs. theoretical pdf") }
			CheckBox{ name: "qqplot"; label: qsTr("Q-Q plot")                      }
			CheckBox{ name: "estCDF"; label: qsTr("Empirical vs. theoretical cdf") }
			CheckBox{ name: "ppplot"; label: qsTr("P-P plot")                      }
		}

		Group
		{
			title: qsTr("Statistics")
			CheckBox{ name: "kolmogorovSmirnov";  label: qsTr("Kolmogorov-Smirnov")}
			CheckBox{ name: "cramerVonMisses";    label: qsTr("Cramér–von Mises")  }
			CheckBox{ name: "andersonDarling";    label: qsTr("Anderson-Darling")  }
			//CheckBox{ name: "shapiroWilk";        label: qsTr("Shapiro-Wilk")      }
		}

	}
}
