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
import JASP.Controls 1.0
import JASP.Widgets 1.0

// All Analysis forms must be built with the From QML item
Form
{
	columns: 1
	VariablesForm
	{
		AvailableVariablesList { name: "allVariablesList" }
		AssignedVariablesList { name: "variables";	title: qsTr("Variables") }
		AssignedVariablesList { name: "splitby";	title: qsTr("Split"); singleVariable: true; allowedColumns: ["ordinal", "nominal"] }
	}

	CheckBox { name: "frequencyTables"; label: qsTr("Frequency tables (nominal and ordinal variables)") }

	Section
	{
		title: qsTr("Plots")

		Group
		{
			CheckBox
			{
				name: "plotVariables"; label: qsTr("Distribution plots")
				CheckBox { name: "distPlotDensity"; label: qsTr("Display density") }
			}
			CheckBox { name: "plotCorrelationMatrix"; label: qsTr("Correlation plots") }
			CheckBox
			{
				name: "splitPlots"; label: qsTr("Boxplots")
				CheckBox { name: "splitPlotOutlierLabel"; label: qsTr("Label Outliers") }
				CheckBox
				{
					name: "splitPlotColour"; label: qsTr("Color"); enableChildrenOnChecked: false
					Group
					{
						CheckBox { name: "splitPlotBoxplot";	label: qsTr("Boxplot Element"); checked: true	}
						CheckBox { name: "splitPlotViolin";		label: qsTr("Violin Elem∂ent")					}
						CheckBox { name: "splitPlotJitter";		label: qsTr("Jitter Element")					}
					}
				}
			}
		}
	}

	Section
	{
		title: qsTr("Statistics")

		Group
		{
			title: qsTr("Percentile Values")

			CheckBox { name: "percentileValuesQuartiles";	label: qsTr("Quartiles") }
			CheckBox
			{
				name: "percentileValuesEqualGroups"; label: qsTr("Cut points for: ")
				childrenOnSameRow: true
				IntegerField
				{
					name: "percentileValuesEqualGroupsNo"
					min: 1
					max: 1000
					defaultValue: 4
					afterLabel: qsTr(" equal groups")
				}
			}
			CheckBox
			{
				name: "percentileValuesPercentiles"; label: qsTr("Percentiles:")
				childrenOnSameRow: true
				TextField
				{
					inputType: "integerArray"
					name: "percentileValuesPercentilesPercentiles"
					fieldWidth: 60
				}
			}
		}

		Group
		{
			title: qsTr("Central Tendency")
			CheckBox { name: "mean";			label: qsTr("Mean");	checked: true	}
			CheckBox { name: "median";			label: qsTr("Median")					}
			CheckBox { name: "mode";			label: qsTr("Mode");					}
			CheckBox { name: "sum";				label: qsTr("Sum");						}
		}

		Group
		{
			title: qsTr("Dispersion")
			columns: 2
			CheckBox { name: "standardDeviation";	label: qsTr("Std.deviation"); checked: true	}
			CheckBox { name: "minimum";				label: qsTr("Minimum");		checked: true	}
			CheckBox { name: "variance";			label: qsTr("Variance")						}
			CheckBox { name: "maximum";				label: qsTr("Maximum");		checked: true	}
			CheckBox { name: "range";				label: qsTr("Range")						}
			CheckBox { name: "standardErrorMean";	label: qsTr("S. E. mean")					}
		}

		Group
		{
			title: qsTr("Distribution")
			CheckBox { name: "skewness";			label: qsTr("Skewness")						}
			CheckBox { name: "kurtosis";			label: qsTr("Kurtosis")						}
			CheckBox { name: "shapiro";			label: qsTr("Shapiro-Wilk test")						}
		}

		CheckBox { name: "statisticsValuesAreGroupMidpoints"; label: qsTr("Values are group midpoints"); debug: true }
	}

	Section
	{
		title: qsTr("Charts")
		debug: true
		RadioButtonGroup
		{
			name: "chartType";
			title: qsTr("Chart Type")
			RadioButton { value: "_1noCharts";		label: qsTr("None")			}
			RadioButton { value: "_2barCharts";		label: qsTr("Bar charts")	}
			RadioButton { value: "_3pieCharts";		label: qsTr("Pie charts")	}
			RadioButton { value: "_4histograms";	label: qsTr("Histograms")	}
		}

		RadioButtonGroup
		{
			name: "chartValues"
			title: qsTr("Chart Values")
			RadioButton { value: "_1frequencies";	label: qsTr("Frequencies")	}
			RadioButton { value: "_2percentages";	label: qsTr("Percentages")	}
		}
	}
}
