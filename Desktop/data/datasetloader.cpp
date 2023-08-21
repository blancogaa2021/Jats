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

#include "datasetloader.h"

#include <boost/algorithm/string.hpp>

#include "importers/databaseimporter.h"
#include "importers/csvimporter.h"
#include "importers/jaspimporter.h"
#include "importers/jaspimporterold.h"
#include "importers/odsimporter.h"
#include "importers/readstatimporter.h"

#include <QFileInfo>

#include "timers.h"
#include "utils.h"
#include "log.h"

using namespace std;
using namespace ods;

string DataSetLoader::getExtension(const string &locator, const string &extension)
{
	std::filesystem::path path(locator);
	string ext = path.extension().generic_string();

	if (!ext.length()) ext=extension;
	return ext;
}

Importer* DataSetLoader::getImporter(const string & locator, const string &ext)
{
	if(	ext == "DATABASE")										return new DatabaseImporter();
	if(	boost::iequals(ext,".csv") || 
		boost::iequals(ext,".txt") ||
		boost::iequals(ext,".tsv"))								return new CSVImporter();
	if(	boost::iequals(ext,".ods"))								return new ODSImporter();
	if(	ReadStatImporter::extSupported(ext))					return new ReadStatImporter(ext);

	return nullptr; //If NULL then JASP will try to load it as a .jasp file (if the extension matches)
}

void DataSetLoader::loadPackage(const string &locator, const string &extension, std::function<void(int)> progress)
{
	JASPTIMER_RESUME(DataSetLoader::loadPackage);

	Importer* importer = getImporter(locator, extension);

	if (importer)
	{
		importer->loadDataSet(locator, progress);
		delete importer;
	}
	else if(extension == ".jasp" || extension == "jasp")
    {
        bool useOldImporter = JASPImporterOld::isCompatible(locator) != JASPImporterOld::Compatibility::NotCompatible;

        if(useOldImporter)  JASPImporterOld::loadDataSet(locator, progress);
        else                JASPImporter::loadDataSet(locator, progress);
	}
	else
		throw std::runtime_error("JASP does not support loading the file-type \"" + extension + '"');

	JASPTIMER_STOP(DataSetLoader::loadPackage);
}

void DataSetLoader::syncPackage(const string &locator, const string &extension, std::function<void(int)> progress)
{
	Utils::sleep(100);

	Importer* importer = getImporter(locator, extension);

	if (importer)
	{
		importer->syncDataSet(locator, progress);
		delete importer;
	}
}
