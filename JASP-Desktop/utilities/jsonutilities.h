#ifndef JSONUTILITIES_H
#define JSONUTILITIES_H

#include "jsonredirect.h"
#include <string>
#include <set>


class JsonUtilities
{
public:
	static std::set<std::string>	convertDragNDropFilterJSONToSet(std::string jsonStr);

	static std::string				removeColumnsFromDragNDropFilterJSON(const std::string & jsonStr,			  std::string columnName);
	static void						removeColumnsFromDragNDropFilterJSON(Json::Value & json,				const std::string & columnName);

	static std::string				replaceColumnNamesInDragNDropFilterJSON(const std::string & jsonStr,	const std::map<std::string, std::string> & changeNameColumns);
	static void						replaceColumnNamesInDragNDropFilterJSON(Json::Value & json,				const std::map<std::string, std::string> & changeNameColumns);

private:
	JsonUtilities() {}
};

#endif // JSONUTILITIES_H
