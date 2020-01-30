#ifndef UPGRADECHANGE_H
#define UPGRADECHANGE_H

#include <string>
#include <vector>
#include "jsonredirect.h"
#include "enumutilities.h"

namespace Modules {

typedef std::map<std::string, std::vector<std::string>> UpgradeMsgs; //option name -> list msgs. "" as option means entire analysis/form

DECLARE_ENUM(BoolOpType, AND, OR, NOT, XOR);
DECLARE_ENUM(ModifyType, Flatten); //Looks silly now but maybe we will have more then one possibility later ;)

extern const char * logId;

struct BoolOp
{
	BoolOp() : _noOp(true) {}
	BoolOp(Json::Value def);

	static bool optionContainsConditional(const Json::Value & conditional, const Json::Value & options);

	bool apply(const Json::Value & options) const;

	std::string toString() const;

private:
	BoolOpType					_type;
	std::vector<BoolOp>			_funcArgs;
	std::vector<Json::Value>	_valArgs;
	bool						_noOp = false;
};

class UpgradeChange
{
public:

	struct upgradeError  : public std::runtime_error
	{
		upgradeError(std::string msg) : std::runtime_error(msg) {}
		const char* what() const noexcept override;
	};

	UpgradeChange(const Json::Value & upgradeStep);

	void applyUpgrade(Json::Value & options, UpgradeMsgs & msgs) const;

private:
	void applyRename(	Json::Value & options, const std::string & oldName, const std::string & newName,	UpgradeMsgs & msgs)	const;
	void applyCopy(		Json::Value & options, const std::string & oldName, const std::string & newName,	UpgradeMsgs & msgs)	const;
	void applyRemove(	Json::Value & options, const std::string & name,									UpgradeMsgs & msgs)	const;
	void applySetValue(	Json::Value & options, const std::string & name,	const Json::Value & newValue,	UpgradeMsgs & msgs) const;
	void applySetBool(	Json::Value & options, const std::string & name,	const BoolOp	  & newBool ,	UpgradeMsgs & msgs) const;
	void applyModifier(	Json::Value & options, const std::string & name,	const ModifyType	modifier,	UpgradeMsgs & msgs) const;

	BoolOp								_conditional;
	std::map<std::string, std::string>	_optionMsgs,
										_optionsRenamed,
										_optionsCopied;
	std::map<std::string, Json::Value>	_optionsNewValue;
	std::map<std::string, BoolOp>		_optionsBoolOp;
	std::set<std::string>				_optionsRemoved;
	std::map<std::string, ModifyType>	_optionsModified;

};

}

#endif // UPGRADECHANGE_H
