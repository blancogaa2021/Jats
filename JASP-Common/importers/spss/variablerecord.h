//
// Copyright (C) 2015-2016 University of Amsterdam
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

#ifndef VARIBABLERECORD_H
#define VARIBABLERECORD_H

#include "readablerecord.h"
#include "fileheaderrecord.h"

namespace spss
{

/**
 * @brief The VariableRecord class : decodes variable records
 *
 * Associated with record type rectype_variable = 2
 */
class VariableRecord: public ReadableRecord<rectype_variable>
{
public:

	/**
	 * @brief VariableRecord Ctor
	 * @param const Converters &fixer - Fixes Endianness
	 * @param fileType The record type value, as found in the file.
	 * @param fileHeader The file ehadewr we are associated with.
	 * @param fromStream The file to read from.
	 *
	 */
	VariableRecord(const NumericConverter &fixer, RecordTypes fileType, FileHeaderRecord * fileHeader, SPSSStream &fromStream);

	virtual ~VariableRecord();


	/*
	 * The data,
	 */
	SPSSIMPORTER_READ_ATTRIB(int32_t, rec_type)
	SPSSIMPORTER_READ_ATTRIB(int32_t, type)
	SPSSIMPORTER_READ_ATTRIB(int32_t, has_var_label)
	SPSSIMPORTER_READ_ATTRIB(int32_t, n_missing_values)
	SPSSIMPORTER_READ_ATTRIB(int32_t, print)
	SPSSIMPORTER_READ_ATTRIB(int32_t, write)
	SPSSIMPORTER_READ_ATTRIB(Char_8, nameInFile)
	SPSSIMPORTER_READ_ATTRIB(int32_t, label_len)
	SPSSIMPORTER_READ_ATTRIB(std::string, label) // label().length() == label_len
	SPSSIMPORTER_READ_ATTRIB(VecDbls, missing_values)

	// Not from the file, but from counting number times we are created,
	SPSSIMPORTER_READ_ATTRIB(size_t, dictionary_index)

	/*
	 * Write and Print fields are in fact unions:
	 * Inlines to break up extarct the fields;
	 */
	static unsigned char num_places(int32_t prField) { return ((prField >> 0) & 0xFF); }
	static unsigned char field_width(int32_t prField) { return ((prField >> 8) & 0xFF); }
	static unsigned char format_type(int32_t prField) { return ((prField >> 16) & 0xFF); }
	static unsigned char zero_filler(int32_t prField)  { return ((prField >> 24) & 0xFF); }

	/*
	 * Name of the variable, cleaned-up.
	 */
	const std::string &name() const { return _name; }

	/*
	 * Data checks,
	 */
	bool isNumeric() const { return (_type == 0); }
	bool isString() const { return ((_type != 0) && (_type != -1)); }
	bool isStringContinuation() const { return (_type == -1) && !hasVarLabel() && !hasMissingValues(); }
	bool hasVarLabel() const { return (_has_var_label == 1); }
	bool hasMissingValues() const { return (_n_missing_values != 0); }

	/**
	 * @brief process Manipulates columns by adding the contents of thie record.
	 * @param columns
	 *
	 * Implematations should examine columns to determine the record history.
	 */
	virtual void process(SPSSColumns & columns);

private:
	std::string  _name;
};

}

#endif // VARIBABLERECORD_H
