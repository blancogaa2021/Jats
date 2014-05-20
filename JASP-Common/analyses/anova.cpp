#include "anova.h"

#include "options/options.h"
#include "options/option.h"
#include "options/optionvariable.h"
#include "options/optionvariables.h"
#include "options/optionboolean.h"
#include "options/optioninteger.h"
#include "options/optionintegerarray.h"
#include "options/optionlist.h"
#include "options/optionnumber.h"
#include "options/optionstring.h"
#include "options/optionstable.h"

using namespace std;

Anova::Anova(int id)
	: Analysis(id, "Anova")
{
}

Options *Anova::createDefaultOptions()
{
	Options *options = new Options();

	// main

	options->add("dependent", new OptionVariable());
	options->add("fixedFactors", new OptionVariables());
	options->add("randomFactors", new OptionVariables());
	options->add("wlsWeights", new OptionVariable());


	// model

	Options *termsTemplate = new Options();
	termsTemplate->add("components", new OptionVariables());

	options->add("modelTerms", new OptionsTable(termsTemplate));
	options->add("sumOfSquares", new OptionList(list("type1", "type2", "type3"), "type3"));


	// contrasts

	Options *contrastsTemplate = new Options();
	contrastsTemplate->add("variable", new OptionVariable());

	contrastsTemplate->add("contrast", new OptionList(list(
		"none",
		"deviation",
		"simple",
		"difference",
		"helmert",
		"repeated",
		"polynomial")));

	options->add("contrasts", new OptionsTable(contrastsTemplate));


	// post hoc tests

	options->add("postHocTests/variables", new OptionVariables());
	options->add("postHocTests/bonferroni", new OptionBoolean());
    options->add("postHocTests/holm", new OptionBoolean());
    options->add("postHocTests/FDR", new OptionBoolean());
    options->add("postHocTests/hochberg", new OptionBoolean());
    options->add("postHocTests/hommel", new OptionBoolean());
    options->add("postHocTests/benjamini", new OptionBoolean());
    //options->add("postHocTests/REGWQ", new OptionBoolean());
    //options->add("postHocTests/SNK", new OptionBoolean());
	options->add("postHocTests/tukey", new OptionBoolean());
    //options->add("postHocTests/tukeyB", new OptionBoolean());
    //options->add("postHocTests/duncan", new OptionBoolean());
    //options->add("postHocTests/hochbergGT2", new OptionBoolean());
    //options->add("postHocTests/gabriel", new OptionBoolean());

	options->add("postHocTests/wallerDuncan", new OptionBoolean());
	options->add("postHocTests/wallerDuncanRatio", new OptionInteger(100));

	options->add("postHocTests/dunnett", new OptionBoolean());
	options->add("postHocTests/dunnettControl", new OptionList(list("first", "last"), "last"));
	options->add("postHocTests/dunnettSides", new OptionList(list("both", "lessThan", "greaterThan")));

	options->add("postHocTests/tamhaneT2", new OptionBoolean());
	options->add("postHocTests/dunnettT3", new OptionBoolean());
	options->add("postHocTests/gamesHowell", new OptionBoolean());
	options->add("postHocTests/dunnettC", new OptionBoolean());


	// options

	options->add("marginalMeans/terms", new OptionVariables());
	options->add("marginalMeans/compareMainEffects", new OptionBoolean());
	options->add("marginalMeans/ciAdjustment", new OptionList(list("LSD", "bonferroni")));

	options->add("misc/descriptives", new OptionBoolean());
	options->add("misc/effectSizeEstimates", new OptionBoolean());
	options->add("misc/observedPower", new OptionBoolean());
	options->add("misc/parameterEstimates", new OptionBoolean());
	options->add("misc/contrastCoefficients", new OptionBoolean());
	options->add("misc/homogeneityTests", new OptionBoolean());
	//options->add("misc/spreadVsLevelPlot", new OptionBoolean());
	//options->add("misc/residualPlot", new OptionBoolean());
	//options->add("misc/lackOfFit", new OptionBoolean());
	//options->add("misc/estimableFunction", new OptionBoolean());

	options->add("significanceLevel", new OptionNumber(.05, .0001, .5));

	return options;
}

