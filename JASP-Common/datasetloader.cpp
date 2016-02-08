//
// Copyright (C) 2013-2016 University of Amsterdam
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

#include <boost/filesystem.hpp>

#include "sharedmemory.h"
#include "dataset.h"

#include "importers/csvimporter.h"
#include "importers/spssimporter.h"
#include "importers/jaspimporter.h"

using namespace boost::interprocess;
using namespace boost;
using namespace std;


void DataSetLoader::loadPackage(DataSetPackage *packageData, const string &locator, boost::function<void(const string &, int)> progress)
{
	filesystem::path path(locator);

	if (path.extension().compare(string(".sav")) == 0)
		SPSSImporter::loadDataSet(packageData, locator, progress);
	else if (path.extension().compare(string(".csv")) == 0 || path.extension().compare(string(".txt")) == 0)
		CSVImporter::loadDataSet(packageData, locator, progress);
	else
		JASPImporter::loadDataSet(packageData, locator, progress);
}

void DataSetLoader::freeDataSet(DataSet *dataSet)
{
	SharedMemory::deleteDataSet(dataSet);
}

DataSet *DataSetLoader::getDataSet()
{
	return SharedMemory::retrieveDataSet();
}


