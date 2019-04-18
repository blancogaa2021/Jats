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

Form {
	usesJaspResults: false
	
	VariablesForm
	{
		AvailableVariablesList	{ name: "allVariablesList" }
		AssignedVariablesList	{ name: "dependent";	title: qsTr("Dependent Variable");		allowedColumns: ["scale"];	singleVariable: true	}
		AssignedVariablesList	{ name: "covariates";	title: qsTr("Covariates");				allowedColumns: ["scale"]							}
		AssignedVariablesList	{ name: "wlsWeights";	title: qsTr("WLS Weights (optional)");	allowedColumns: ["scale"];	singleVariable: true	}
	}
	
	BayesFactorType {}

	Group
	{
		title: qsTr("Output")
		columns: 2

		CheckBox{ name: "postSummaryTable"; label: qsTr("Posterior summary"); id: postSummaryTable }

		DropDown
		{
			name: "summaryType"
			enabled: postSummaryTable.checked || postSummaryPlot.checked
			indexDefaultValue: 3
			values: [
				{ label: "Best model",			value: "best"		},
				{ label: "Most complex model",	value: "complex"	},
				{ label: "Median model",		value: "median"		},
				{ label: "Model averaged",		value: "averaged"	}
			]
		}

		CheckBox
		{
			name: "postSummaryPlot"
			label: qsTr("Plot of coefficients")
			id: postSummaryPlot
			CheckBox { name: "omitIntercept"; label: qsTr("Omit intercept") }

		}

		PercentField
		{
			name: "posteriorSummaryPlotCredibleIntervalValue"
			label: qsTr("Credible interval")
			enabled: postSummaryTable.checked || postSummaryPlot.checked
			defaultValue: 95
		}
	}

	RadioButtonGroup
	{
		name: "bayesFactorOrder"
		title: qsTr("Order")
		RadioButton { value: "bestModelTop"; label: qsTr("Compare to best model"); checked: true	}
		RadioButton { value: "nullModelTop"; label: qsTr("Compare to null model")					}
	}

	RadioButtonGroup
	{
		name: "shownModels"
		title: qsTr("Limit no. models shown")
		RadioButton { value: "unlimited"; label: qsTr("No") }
		RadioButton { 
			value: "limited"
			label: qsTr("Yes, show best")
			checked: true
			childrenOnSameRow: true
			IntegerField { name: "numShownModels"; defaultValue: 10; min: 1}
		}

	}

	Group
	{
		title: qsTr("Data")
		CheckBox { name: "descriptives"; label: qsTr("Descriptives") }
	}
	
	Section
	{
		title: qsTr("Model")
		
		VariablesForm
		{
			height: 200
			
			AvailableVariablesList
			{
				name: "availableTerms"
				title: qsTr("Components")
				width: parent.width / 4
				source: ['covariates']
			}
			AssignedVariablesList
			{
				name: "modelTerms"
				title: qsTr("Model terms")
				width: parent.width * 5 / 9
				listViewType: "Interaction"
				ExtraControlColumn {
					type: "CheckBox"
					name: "isNuisance"
					title: qsTr("Add to null model")
					purpose: "nuisance"					
				}
			}
		}
		
	}
	
	Section
	{
		title: qsTr("Plots")
		
		Group
		{
			title: qsTr("Coefficients")
			CheckBox { name: "plotInclusionProbabilities";	label: qsTr("Inclusion probabilities")			}
			CheckBox { name: "plotCoefficientsPosterior";	label: qsTr("Marginal posterior distributions")	}
		}

		Group
		{
			title: qsTr("Models")
			CheckBox { name: "plotLogPosteriorOdds";	label: qsTr("Log posterior odds")				}
			CheckBox { name: "plotModelComplexity";		label: qsTr("Log(P(data)M)) vs. model size")	}
			CheckBox { name: "plotModelProbabilities";	label: qsTr("Model probabilities")				}
		}

		Group
		{
			title: qsTr("Residuals")
			CheckBox { name: "plotResidualsVsFitted";	label: qsTr("Residuals vs. fitted")				  }
			CheckBox { name: "plotQQplot";	            label: qsTr("Q-Q plot of model averaged residuals")}
		}
	}
	
	Section
	{
		title: qsTr("Advanced Options")
		
		RadioButtonGroup
		{
			name: "priorRegressionCoefficients"
			title: qsTr("Prior")

			RadioButton { value: "AIC";			label: qsTr("AIC")		}
			RadioButton { value: "BIC";			label: qsTr("BIC")		}
			RadioButton { value: "EB-global";	label: qsTr("EB-global")	}
			RadioButton { value: "EB-local";	label: qsTr("EB-local")	}
			RadioButton { value: "g-prior";		label: qsTr("g-prior")	}
			GridLayout
			{
				rowSpacing: Theme.rowGroupSpacing
				columnSpacing: 0
				Group
				{
					RadioButton { value: "hyper-g";			label: qsTr("Hyper-g!");			id: hyperg			}
					RadioButton { value: "hyper-g-laplace";	label: qsTr("Hyper-g-Laplace");		id: hyperglaplace	}
					RadioButton { value: "hyper-g-n";		label: qsTr("Hyper-g-n");			id: hypergn			}
				}
				DoubleField
				{
					name: "alpha";
					label: qsTr("alpha");
					enabled: hyperg.checked || hyperglaplace.checked || hypergn.checked
					defaultValue: 3.0
				}
				RadioButton { value: "JZS"; label: qsTr("JZS"); checked: true; id: jzs }
				DoubleField
				{
					name: "rScale"
					label: qsTr("r scale")
					enabled: jzs.checked
					fieldWidth: 50
					defaultValue: 0.354
					max: 100000
				}
			}
		}
			
		ColumnLayout
		{
			RadioButtonGroup
			{
				name: "modelPrior"
				title: qsTr("Model prior")
				RadioButton
				{
					value: "beta.binomial"; label: qsTr("Beta binomial"); checked: true
					childrenOnSameRow: true
					childrenArea.columnSpacing: 1
					DoubleField { name: "betaBinomialParamA"; label: qsTr("a");  defaultValue: 1 }
					DoubleField { name: "betaBinomialParamB"; label: qsTr("b");  defaultValue: 1 }
				}
				RadioButton
				{
					value: "Bernoulli"; label: qsTr("Bernouilli")
					childrenOnSameRow: true
					DoubleField { name: "bernoulliParam"; label: qsTr("p"); defaultValue: 0.5; max: 1; decimals: 2 }
				}
				RadioButton { value: "uniform"; label: qsTr("Uniform") }
			}

			RadioButtonGroup
			{
				name: "samplingMethod"
				title: qsTr("Sampling method")
				RadioButton
				{
					value: "BAS"; label: qsTr("BAS"); checked: true
					childrenOnSameRow: true
					IntegerField { name: "numberOfModels"; label: qsTr("No. models"); defaultValue: 0; max: 100000000 }
				}
				RadioButton
				{
					value: "MCMC"; label: qsTr("MCMC")
					childrenOnSameRow: true
					IntegerField { name: "iterationsMCMC"; label: qsTr("No. samples"); defaultValue: 0; max: 100000000 }
				}
			}

			Group
			{
				title: qsTr("Numerical accuracy")
				IntegerField
				{
					name: "nSimForCRI"
					label: qsTr("No. samples for credible interval")
					defaultValue: 1000
					fieldWidth: 50
					min: 100
					max: 1000000
				}
			}
		}
	}
	
}
