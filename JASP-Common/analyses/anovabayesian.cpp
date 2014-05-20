#include "anovabayesian.h"

#include "options/options.h"
#include "options/option.h"
#include "options/optionvariables.h"
#include "options/optionboolean.h"
#include "options/optioninteger.h"
#include "options/optionintegerarray.h"
#include "options/optionlist.h"
#include "options/optionnumber.h"
#include "options/optionvariable.h"
#include "options/optionstable.h"

AnovaBayesian::AnovaBayesian(int id)
	: Analysis(id, "AnovaBayesian")
{
}

Options *AnovaBayesian::createDefaultOptions()
{
	Options *options = new Options();

	options->add("dependent", new OptionVariable());
	options->add("fixedFactors", new OptionVariables());
	options->add("randomFactors", new OptionVariables());

	Options *termsTemplate = new Options();
	termsTemplate->add("components", new OptionVariables());

	options->add("modelTerms", new OptionsTable(termsTemplate));

	options->add("outputEffects", new OptionBoolean());
	options->add("posteriorEstimates", new OptionBoolean());
	options->add("posteriorDistributions", new OptionBoolean());

	return options;
}
