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
	
	IntegerField { visible: false; name: "plotHeightDescriptivesPlotLegend"     ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotHeightDescriptivesPlotNoLegend"   ; defaultValue: 300 }
	IntegerField { visible: false; name: "plotWidthDescriptivesPlotLegend"      ; defaultValue: 450 }
	IntegerField { visible: false; name: "plotWidthDescriptivesPlotNoLegend"    ; defaultValue: 350 }
	
	
	VariablesForm
	{
		height: 520
		FactorsList { name: "repeatedMeasuresFactors"; title: qsTr("Repeated Measures Factors"); height: 180 }
		AssignedVariablesList
		{
			name: "repeatedMeasuresCells"
			title: qsTr("Repeated Measures Cells")
			allowedColumns: ["scale"]
			listViewType: "MeasuresCells"
			syncModels: "repeatedMeasuresFactors"
			height: 140
		}
		AssignedVariablesList
		{
			name: "betweenSubjectFactors"
			title: qsTr("Between Subject Factors")
			allowedColumns: ["ordinal", "nominal"]
			itemType: "fixedFactors"
		}
		AssignedVariablesList
		{
			name: "covariates"
			title: qsTr("Covariates")
			allowedColumns: ["scale"]
		}
	}
	
	
	ExpanderButton
	{
		text: qsTr("Model")
		
		VariablesForm
		{
			height: 150
			availableVariablesList { name: "withinComponents"; title: qsTr("Repeated Measures Components"); syncModels: ["repeatedMeasuresFactors"] }
			AssignedVariablesList {  name: "withinModelTerms"; title: qsTr("Model terms"); listViewType: "Interaction" }
		}
		
		VariablesForm
		{
			height: 150
			availableVariablesList { name: "betweenComponents"; title: qsTr("Between Subjects Components"); syncModels: ["betweenSubjectFactors", "covariates"] }
			AssignedVariablesList {  name: "betweenModelTerms"; title: qsTr("Model terms"); listViewType: "Interaction" }
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
		
		CheckBox { text: qsTr("Sphericity tests")		; name: "sphericityTests"}
		CheckBox { text: qsTr("Sphericity corrections")	; name: "sphericityCorrections"; id: sphericityCorrections }
		RowLayout
		{
			Layout.leftMargin: Theme.indentationLength
			enabled: sphericityCorrections.checked
			CheckBox { text: qsTr("None")				; name: "sphericityNone"				; checked: true }
			CheckBox { text: qsTr("Greenhouse-Geisser")	; name: "sphericityGreenhouseGeisser"	; checked: true }
			CheckBox { text: qsTr("Huynth-Feidt")		; name: "sphericityHuynhFeldt"			; checked: true }
		}
		CheckBox { text: qsTr("Homogeneity Tests")		; name: "homogeneityTests" }
	}
	
	ExpanderButton
	{
		text: qsTr("Contrasts")
		ContrastsList { syncModels: ["repeatedMeasuresFactors", "betweenSubjectFactors"] }
	}
	
	ExpanderButton
	{
		text: qsTr("Post Hoc Tests")
		
		VariablesForm
		{
			height: 150
			availableVariablesList { name: "postHocTestsAvailable"; syncModels: ["repeatedMeasuresFactors", "betweenSubjectFactors"] }
			AssignedVariablesList {  name: "postHocTestsVariables" }
		}
		
		RowLayout
		{
			CheckBox { text: qsTr("Effect Size");						name: "postHocTestEffectSize" }
			CheckBox { text: qsTr("Pool error term for RM factors");	name: "postHocTestPooledError" }
		}
		
		GroupBox
		{
			title: qsTr("Correction")
			CheckBox { text: qsTr("Tukey")          ; name: "postHocTestsTukey"     ; checked: true }
			CheckBox { text: qsTr("Scheffe")        ; name: "postHocTestsScheffe"                   }
			CheckBox { text: qsTr("Bonferroni")     ; name: "postHocTestsBonferroni"                }
			CheckBox { text: qsTr("Holm")           ; name: "postHocTestsHolm"                      }
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Descriptives Plots")
		
		VariablesForm
		{
			height: 150
			availableVariablesList { name: "descriptivePlotsVariables"; title: qsTr("Factors");			syncModels: ["repeatedMeasuresFactors", "betweenSubjectFactors"] }
			AssignedVariablesList {  name: "plotHorizontalAxis";		title: qsTr("Horizontal axis"); singleItem: true }
			AssignedVariablesList {  name: "plotSeparateLines";			title: qsTr("Separate lines");	singleItem: true }
			AssignedVariablesList {  name: "plotSeparatePlots";			title: qsTr("Separate plots");	singleItem: true }
		}
		
		TextField { text: qsTr("Label y-axis") ; name: "labelYAxis"}
		GroupBox
		{
			title: qsTr("Display")
			
			RowLayout
			{
				CheckBox { text: qsTr("Display error bars"); name: "plotErrorBars" }
				CheckBox { text: qsTr("Pool SE across RM factors"); name: "usePooledStandErrorCI" }
			}
			
			RadioButtonGroup
			{
				name: "errorBarType"
				RadioButton { text: qsTr("Confidence Interval"); name: "confidenceInterval"; checked: true; id: confidenceInterval }
				PercentField {indent: true; text: qsTr("Interval"); name: "confidenceIntervalInterval"; defaultValue: 95; enabled: confidenceInterval.checked}
				RadioButton { text: qsTr("Standard error"); name: "standardError" }
			}
		}
	}
	
	ExpanderButton
	{
		text: qsTr("Additional Options")
		GroupBox
		{
			title: qsTr("Marginal means")
			debug: true
			
			VariablesForm
			{
				height: 150
				availableVariablesList { name: "marginalMeansTermsAvailable" ; syncModels: "withinModelTerms"; showVariableTypeIcon: false }
				AssignedVariablesList {  name: "marginalMeansTerms"; showVariableTypeIcon: false }
			}
			
			CheckBox { text: qsTr("Compare marginal means to 0"); name: "marginalMeansCompareMainEffects"; id: marginalMeansCompareMainEffects }
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
			height: 150
			availableVariablesList { name: "effectsVariables";	title: qsTr("Factors"); syncModels: ["repeatedMeasuresFactors", "betweenSubjectFactors"] }
			AssignedVariablesList {  name: "simpleFactor";		title: qsTr("Simple effect factor");	singleItem: true }
			AssignedVariablesList { name: "moderatorFactorOne";	title: qsTr("Moderator factor 1");		singleItem: true }
			AssignedVariablesList { name: "moderatorFactorTwo";	title: qsTr("Moderator factor 2");		singleItem: true }
		}
		
		CheckBox { text: qsTr("Pool error terms"); name: "poolErrorTermSimpleEffects"}
	}
	
	ExpanderButton
	{
		text: qsTr("Nonparametrics")
		
		VariablesForm
		{
			height: 150
			availableVariablesList { name: "kruskalVariablesAvailable"; title: qsTr("Factors"); syncModels: ["repeatedMeasuresFactors", "betweenSubjectFactors"] }
			AssignedVariablesList {  name: "friedmanWithinFactor";		title: qsTr("RM Factor") }
			AssignedVariablesList {  name: "friedmanBetweenFactor";		title: qsTr("Optional grouping factor"); singleItem: true }
		}
		
		CheckBox { text: qsTr("Connover's post hoc tests") ; name: "connoverTest"}
	}
}
