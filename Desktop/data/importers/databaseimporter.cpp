﻿#include "databaseimporter.h"
#include <QSqlRecord>
#include <QSqlField>
#include "database/databaseimportcolumn.h"
#include "utils.h"

ImportDataSet * DatabaseImporter::loadFile(const std::string &locator, std::function<void(int)> progressCallback)
{
	// locator is the result of DatabaseConnectionInfo::toJson, so:
	Json::Value json;
	if(!Json::Reader().parse(locator, json))
		throw std::runtime_error("DatabaseImporter::loadFile received illegal locator!"); //shouldnt occur normally
	
	_info = DatabaseConnectionInfo(json);

	if(!_info.connect())
		throw std::runtime_error(fq(tr("Failed to connect to database %1 at %2 with user %3, last error was: '%4'")
										.arg(_info._database)
										.arg(_info._hostname + ":" + tq(std::to_string(_info._port)))
										.arg(_info._username)
										.arg(_info.lastError())));
	
	
	
	QSqlQuery	query	= _info.runQuery();
	float		progDiv	= 100.0f / float(query.size());
	QSqlRecord  record	= query.record();
	
	ImportDataSet * data = new ImportDataSet(this);

	for(int i=0; i<record.count(); i++)
		data->addColumn(new DatabaseImportColumn(data, fq(record.fieldName(i)), record.field(i).metaType()));
												
	long lastProgress = Utils::currentMillis();	
		
	do
	{
		if(lastProgress + 1000 < Utils::currentMillis())
		{
			progressCallback(int(progDiv * query.at()));
			lastProgress = Utils::currentMillis();	
		}

		if(query.isValid())
			for(int i=0; i<record.count(); i++)
				static_cast<DatabaseImportColumn*>(data->getColumn(i))->addValue(query.value(i));
	}
	while(query.next());

	_info.close();
	
	data->buildDictionary(); //Not necessary for reading from database but synching will break otherwise...
	
	return data;
}

void DatabaseImporter::initColumn(QVariant colId, ImportColumn *importColumn)
{
	JASPTIMER_SCOPE(DatabaseImporter::initColumn);
	
	typedef QMetaType::Type MT;
	
	DatabaseImportColumn * col = static_cast<DatabaseImportColumn*>(importColumn);
	
	initColumnWithStrings(colId, col->name(), col->allValuesAsStrings());
	
}
