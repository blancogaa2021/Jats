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
	
	IntegerField { visible: false; name: "plotWidthQQPlot"                      ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotHeightQQPlot"                     ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotHeightDescriptivesPlotLegend"     ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotHeightDescriptivesPlotNoLegend"   ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotWidthDescriptivesPlotLegend"      ; defaultValue: 430 }
	IntegerField { visible: false; name: "plotWidthDescriptivesPlotNoLegend"    ; defaultValue: 350 }
	
	VariablesForm
	{
		height: 400
		AssignedVariablesList
		{
			name: "dependent"
			title: qsTr("Dependent Variable")
			singleItem: true
			allowedColumns: ["scale"]
		}
		AssignedVariablesList
		{
			name: "fixedFactors"
			title: qsTr("Fixed Factors")
			allowedColumns: ["ordinal", "nominal"]
		}
		AssignedVariablesList
		{
			name: "randomFactors"
			title: qsTr("Random Factors")
			allowedColumns: ["ordinal", "nominal"]
			debug: true
		}
		AssignedVariablesList
		{
			name: "covariates"
			title: qsTr("Covariates")
			allowedColumns: ["scale"]
		}
		AssignedVariablesList
		{
			name: "wlsWeights"
			title: qsTr("WLS Weights")
			singleItem: true
			allowedColumns: ["scale"]
		}
	}
	
	
	ExpanderButton
	{
		text: qsTr("Model")
		
		VariablesForm
		{
			height: 200
			availableVariablesList { name: "components"; title: qsTr("Components"); syncModels: ["fixedFactors", "randomFactors", "covariates"] }
			AssignedVariablesList  { name: "modelTerms"; title: qsTr("Model terms"); listViewType: "Interaction" }
		}
		
		ComboBox
		{
			name: "sumOfSquares"
			currentIndex: 2
			text: qsTr("Sum of squares")
			model: ListModel
			{
				ListElement {key: "Type \u2160"; value: "type1"}
				ListElement {key: "Type \u2161"; value: "type2"}
				ListElement {key: "Type \u2162"; value: "type3"}
			}
		}
		
	}
	
	ExpanderButton
	{
		text: qsTr("Assumption Checks")
		
		CheckBox { text: qsTr("Homogeneity tests")      ; name: "homogeneityTests"}
		CheckBox { text: qsTr("Q-Q plot of residuals")  ; name: "qqPlot"}
		CheckBox { text: qsTr("Factor covariate independence check")  ; name: "factorCovariateIndependence"; debug: true}
	}
	
	ExpanderButton
	{
		text: qsTr("Contrasts")
		
		ContrastsList {}
		
		CheckBox { text: qsTr("Assume equal variances") ; name: "contrastAssumeEqualVariance"; checked: true}
		RowLayout
		{
			CheckBox { text: qsTr("Confidence intervals")  ; name: "confidenceIntervalsContrast"; id: confidenceIntervalsContrast }
			PercentField { indent: true; name: "confidenceIntervalIntervalContrast"; defaultValue: 95; enabled: confidenceIntervalsContrast.checked}
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Post Hoc Tests")
		
		VariablesForm
		{
			height: 200
			availableVariablesList { name: "postHocTestsAvailable"; syncModels: "fixedFactors" }
			AssignedVariablesList {  name: "postHocTestsVariables" }
		}
		
		GroupBox
		{
			CheckBox { text: qsTr("Effect Size")        ; name: "postHocTestEffectSize"}
			RowLayout
			{
				CheckBox { text: qsTr("Confidence intervals")  ; name: "confidenceIntervalsPostHoc"; id: confidenceIntervalsPostHoc }
				PercentField {name: "confidenceIntervalIntervalPostHoc"; defaultValue: 95; enabled: confidenceIntervalsPostHoc.checked }
			}
		}
		
		GridLayout
		{
			GroupBox
			{
				title: qsTr("Correction")
				CheckBox { text: qsTr("Tukey")          ; name: "postHocTestsTukey"     ; checked: true }
				CheckBox { text: qsTr("Scheffe")        ; name: "postHocTestsScheffe"                   }
				CheckBox { text: qsTr("Bonferroni")     ; name: "postHocTestsBonferroni"                }
				CheckBox { text: qsTr("Holm")           ; name: "postHocTestsHolm"                      }
			}
			GroupBox
			{
				title: qsTr("Type")
				CheckBox { text: qsTr("Standard")       ; name: "postHocTestsTypeStandard"     ; checked: true }
				CheckBox { text: qsTr("Games-Howell")   ; name: "postHocTestsTypeGames"                  }
				CheckBox { text: qsTr("Dunnett")        ; name: "postHocTestsTypeDunnett"                }
				CheckBox { text: qsTr("Dunn")           ; name: "postHocTestsTypeDunn"                   }
			}
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Descriptives Plots")
		
		VariablesForm {
			height: 200
			availableVariablesList { title: qsTr("Factors")			; name: "descriptivePlotsVariables" ; syncModels: "fixedFactors" }
			AssignedVariablesList {	title: qsTr("Horizontal axis")	; name: "plotHorizontalAxis"    ; singleItem: true }
			AssignedVariablesList {	title: qsTr("Separate lines")	; name: "plotSeparateLines"     ; singleItem: true }
			AssignedVariablesList { title: qsTr("Separate plots")	; name: "plotSeparatePlots"     ; singleItem: true }
		}
		
		GroupBox
		{
			title: qsTr("Display")
			CheckBox { text: qsTr("Display error bars"); name: "plotErrorBars" }
			
			RadioButtonGroup
			{
				name: "errorBarType"
				RadioButton { text: qsTr("Confidence Interval"); name: "confidenceInterval"; checked: true; id: confidenceInterval }
				PercentField { indent: true; text: qsTr("Interval"); name: "confidenceIntervalInterval"; defaultValue: 95; enabled: confidenceInterval.checked}
				RadioButton { text: qsTr("Standard error"); name: "standardError" }
			}
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Additional Options")
		Label { text: qsTr("Marginal means") }
		
		VariablesForm
		{
			height: 200
			availableVariablesList { name: "marginalMeansTermsAvailable" ; syncModels: "modelTerms"; showVariableTypeIcon: false }
			AssignedVariablesList {	 name: "marginalMeansTerms"; showVariableTypeIcon: false }
		}
		
		CheckBox { text: qsTr("Compare marginal means to 0")    ; name: "marginalMeansCompareMainEffects"; id: marginalMeansCompareMainEffects }
		ComboBox 
		{
			name: "marginalMeansCIAdjustment"
			indent: true
			text: qsTr("Confidence interval adjustment")
			model: ListModel 
			{
				ListElement {key: "None"; value: "none"}
				ListElement {key: "Bonferro"; value: "bonferroni"}
				ListElement {key: "Sidak"; value: "sidak"}
			}
			enabled: marginalMeansCompareMainEffects.checked
		}
		
		GroupBox
		{
			title: qsTr("Display")
			CheckBox { text: qsTr("Descriptive statistics")     ; name: "descriptives" }
			CheckBox { text: qsTr("Estimates of effect size")   ; name: "effectSizeEstimates"   ; id: effectSizeEstimates }
			Row
			{
				Layout.leftMargin: Theme.indentationLength
				enabled: effectSizeEstimates.checked
				CheckBox { text: qsTr("η²")         ; name: "effectSizeEtaSquared"; checked: true }
				CheckBox { text: qsTr("partial η²") ; name: "effectSizePartialEtaSquared" }
				CheckBox { text: qsTr("ω²")         ; name: "effectSizeOmegaSquared" }
			}
			CheckBox { text: qsTr("Vovk-Sellke maximum p-ratio"); name: "VovkSellkeMPR" }
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Simple Main Effects")
		
		VariablesForm
		{
			height: 200
			availableVariablesList { title: qsTr("Factors")				; name: "effectsVariables"      ; syncModels: "fixedFactors" }
			AssignedVariablesList {	title: qsTr("Simple effect factor")	; name: "simpleFactor"          ; singleItem: true }
			AssignedVariablesList {	title: qsTr("Moderator factor 1")	; name: "moderatorFactorOne"    ; singleItem: true }
			AssignedVariablesList {	title: qsTr("Moderator factor 2")	; name: "moderatorFactorTwo"    ; singleItem: true }
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Nonparametrics")
		
		VariablesForm
		{
			height: 200
			availableVariablesList { name: "kruskalVariablesAvailable"; title: qsTr("Kruskal-Wallis test"); syncModels: "fixedFactors" }
			AssignedVariablesList {	name: "kruskalVariablesAssigned" }
		}
		
		CheckBox { text: qsTr("Dunn's post hoc test")        ; name: "dunnTest"}
	}
	
}
