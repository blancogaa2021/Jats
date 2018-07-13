#include "optioncomputedcolumn.h"
#include "computedcolumn.h"

void OptionComputedColumn::init(const Json::Value &data)
{
	set(data.get("default", "").asString());
}

Json::Value OptionComputedColumn::asJSON() const
{
	return Json::Value(value());
}

void OptionComputedColumn::setValue(const std::string &newValue)
{
	std::string columnName = newValue;

	if(_value != "")
	{
		if(_value != columnName)	requestComputedColumnDestruction(_value);
		else						return;
	}

	OptionString::setValue(columnName);

	if(columnName != "" && requestComputedColumnCreation(columnName) == NULL)
		OptionString::setValue("");
}

Option *OptionComputedColumn::clone() const
{
	return new OptionComputedColumn(_value);
}
