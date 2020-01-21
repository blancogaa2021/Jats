//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef COLUMNENCODER_H
#define COLUMNENCODER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "jsonredirect.h"

///Class to "encode" the names of columns
/// It can be used both directly, through columnEncoder()->, in that scenario it only en- and decodes actual columnNames from the dataset.
/// If you want to en- or decode other names then you instantiate a separate copy and use it's functions.
class ColumnEncoder
{
	typedef std::map<std::string, std::string>	colMap;
	typedef std::vector<std::string>			colVec;
	typedef std::set<ColumnEncoder *>			ColumnEncoders;

private:						ColumnEncoder() { invalidateAll(); }
public:
								ColumnEncoder(std::string prefix, std::string postfix = "._Encoded");
								~ColumnEncoder();
	static ColumnEncoder	*	columnEncoder();

	static	bool				isColumnName(const std::string & in)							{ return columnEncoder()->shouldEncode(in); }
	static	bool				isEncodedColumnName(const std::string & in)						{ return columnEncoder()->shouldDecode(in); }
	static	void				setCurrentColumnNames(const std::vector<std::string> & names)	{ columnEncoder()->setCurrentNames(names);	}

			bool				shouldEncode(const std::string & in);
			bool				shouldDecode(const std::string & in);
			void				setCurrentNames(const std::vector<std::string> & names);
			void				setCurrentNamesFromOptionsMeta(const std::string & options);

			std::string			encode(const std::string &in);
			std::string			decode(const std::string &in);


			///Replace all occurences of columnNames in a string by their encoded versions, taking into account the presence of word boundaries and parentheses.
			std::string			encodeRScript(std::string text, std::set<std::string> * columnNamesFound);

			///Replace all occurences of columnNames in a string by their encoded versions, regardless of word boundaries or parentheses.
	static	std::string			encodeAll(const std::string & text) { return replaceAll(text, encodingMap(), originalNames()); }

			///Replace all occurences of encoded columnNames in a string by their decoded versions, regardless of word boundaries or parentheses.
	static	std::string			decodeAll(const std::string & text) { return replaceAll(text, decodingMap(), encodedNames());  }

			///Replace all occurences of columnNames in a string by their encoded versions in all json-names and string-values, regardless of word boundaries or parentheses.
	static	void				encodeJson(Json::Value & json, bool replaceNames = false);

			///Replace all occurences of encoded columnNames in a string by their decoded versions in all json-names and string-values, regardless of word boundaries or parentheses.
	static	void				decodeJson(Json::Value & json, bool replaceNames = true);

private:
	static	std::string			replaceAll(std::string		text, const std::map<std::string, std::string> & map, const std::vector<std::string> & names);
	static	void				replaceAll(Json::Value &	json, const std::map<std::string, std::string> & map, const std::vector<std::string> & names, bool replaceNames);
	static	std::vector<size_t>	getPositionsColumnNameMatches(const std::string & text, const std::string & columnName);
			void				collectExtraEncodingsFromMetaJson(const Json::Value & in, std::vector<std::string> & namesCollected) const;
	static	void				sortVectorBigToSmall(std::vector<std::string> & vec);
	static	const colMap	&	encodingMap();
	static	const colMap	&	decodingMap();
	static	const colVec	&	originalNames();
	static	const colVec	&	encodedNames();
	static	void				invalidateAll();

	static	bool				_encodingMapInvalidated,
								_decodingMapInvalidated,
								_originalNamesInvalidated,
								_encodedNamesInvalidated;

	static ColumnEncoder	*	_columnEncoder;
	static ColumnEncoders	*	_otherEncoders;

	colMap						_encodingMap,
								_decodingMap;
	colVec						_originalNames,
								_encodedNames;

	std::string					_encodePrefix  = "JaspColumn_.",
								_encodePostfix = "._Encoded";
};

#endif // COLUMNENCODER_H
