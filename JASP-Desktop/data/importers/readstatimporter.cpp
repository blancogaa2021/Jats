#include "readstatimporter.h"
#include <iostream>
#include "readstat/readstatimportdataset.h"
#include "log.h"
#include "readstat/readstat_custom_io.h"

ReadStatImporter::~ReadStatImporter() {}

int handle_metadata(readstat_metadata_t *metadata, void *ctx)
{
	ReadStatImportDataSet * data = static_cast<ReadStatImportDataSet*>(ctx);

	data->setVariableCount(readstat_get_var_count(metadata));
	data->setExpectedRows(readstat_get_row_count(metadata));

	return READSTAT_HANDLER_OK;
}

int handle_variable(int, readstat_variable_t *variable, const char *val_labels, void *ctx)
{
	ReadStatImportDataSet * data			= static_cast<ReadStatImportDataSet*>(ctx);
	int 					var_index		= readstat_variable_get_index(variable);
	std::string				name			= readstat_variable_get_name(variable),
							labelsID		= val_labels != NULL ? val_labels : "";
	readstat_measure_t		colMeasure		= readstat_variable_get_measure(variable);
	columnType		colType;

	switch(colMeasure)
	{
	case READSTAT_MEASURE_UNKNOWN:	colType = columnType::unknown;	break;
	case READSTAT_MEASURE_NOMINAL:	colType = columnType::nominal;	break;
	case READSTAT_MEASURE_ORDINAL:	colType = columnType::ordinal;	break;
	case READSTAT_MEASURE_SCALE:	colType = columnType::scale;		break;
	}

	data->addColumn(var_index, new ReadStatImportColumn(data, name, labelsID, colType));

	return READSTAT_HANDLER_OK;
}

int handle_value(int , readstat_variable_t *variable, readstat_value_t value, void *ctx)
{
	ReadStatImportDataSet *	data			= static_cast<ReadStatImportDataSet*>(ctx);
	int						var_index		= readstat_variable_get_index(variable);
	ReadStatImportColumn  *	col				= data->column(var_index);

	if(var_index == 0) data->incrementRow();

	col->addValue(value);

	return READSTAT_HANDLER_OK;
}

int handle_value_label(const char *val_labels, readstat_value_t value, const char *label, void *ctx)
{
	ReadStatImportDataSet * data = static_cast<ReadStatImportDataSet*>(ctx);

	data->addLabelKeyValue(val_labels, value, label);

	return READSTAT_HANDLER_OK;
}

bool ReadStatImporter::extSupported(const std::string & ext)
{
	static std::set<std::string> supportedExts({"dta", "por", "sav", "sas7bdat", "sas7bcat", "xpt", ".dta", ".por", ".sav", ".sas7bdat", ".sas7bcat", ".xpt"});
	return supportedExts.count(ext) > 0;
}


ImportDataSet* ReadStatImporter::loadFile(const std::string &locator, boost::function<void(const std::string &, int)> progressCallback)
{
	ReadStatImportDataSet	*	data	= new ReadStatImportDataSet(this, progressCallback);
	readstat_error_t			error	= READSTAT_OK;
	readstat_parser_t		*	parser	= readstat_parser_init();

	//typedef int (*readstat_note_handler)(int note_index, const char *note, void *ctx); //Could be nice to have the notes from whatever file in JASP? Although I am not sure where we would show the data.
	//typedef int (*readstat_value_label_handler)(const char *val_labels, readstat_value_t value, const char *label, void *ctx);

#ifdef WIN32
	init_io_handlers(parser);
#endif

	readstat_set_metadata_handler(		parser, &handle_metadata	);
	readstat_set_variable_handler(		parser, &handle_variable	);
	readstat_set_value_handler(			parser, &handle_value		);
	readstat_set_value_label_handler(	parser, &handle_value_label	);

	if		(_ext == "sav")			error = readstat_parse_sav(		parser, locator.c_str(), data);
	else if	(_ext == "dta")			error = readstat_parse_dta(		parser, locator.c_str(), data);
	else if	(_ext == "por")			error = readstat_parse_por(		parser, locator.c_str(), data);
	else if	(_ext == "sas7bdat")	error = readstat_parse_sas7bdat(parser, locator.c_str(), data);
	else if	(_ext == "sas7bcat")	error = readstat_parse_sas7bcat(parser, locator.c_str(), data);
	else if	(_ext == "xpt")			error = readstat_parse_xport(	parser, locator.c_str(), data);
	else							throw std::runtime_error("JASP does not support extension " + _ext);

	readstat_parser_free(parser);

#ifdef WIN32
	io_cleanup();
#endif

	data->setLabelsToColumns();

	if (error != READSTAT_OK)
		throw std::runtime_error("Error processing " + locator + " " + readstat_error_message(error));

	return data;
}

void ReadStatImporter::initColumn(QVariant colId, ImportColumn * importColumn)
{
	ReadStatImportColumn * col = static_cast<ReadStatImportColumn*>(importColumn);

	switch(col->getColumnType())
	{
	case columnType::scale:
		_packageData->initColumnAsScale(colId, col->name(), col->doubles());
		break;

	case columnType::ordinal:
	case columnType::nominal:
		if(col->hasLabels())	_packageData->initColumnAsNominalOrOrdinal(colId, col->name(), col->ints(), col->intLabels(),	col->getColumnType() == columnType::ordinal);
		else					_packageData->initColumnAsNominalOrOrdinal(colId, col->name(), col->ints(), col->uniqueInts(),	col->getColumnType() == columnType::ordinal);
		break;

	case columnType::nominalText:
		_packageData->storeInEmptyValues(col->name(), _packageData->initColumnAsNominalText(colId, col->name(), col->strings(), col->strLabels()));
		break;

	default:
		Log::log() << "Column " << col->name() << " has unknown type after loading so presumably doesn't contain any data whatsoever, this error should probably not occur!\n";
		break;
	}
}

