#include "reporter.h"
#include "results/resultsjsinterface.h"
#include "data/datasetpackage.h"
#include "gui/messageforwarder.h"
#include "gui/preferencesmodel.h"
#include "analysis/analyses.h"
#include <iostream>
#include <QDateTime>
#include <QFile>
#include "tempfiles.h"

Reporter::Reporter(QObject *parent, QDir reportingDir) 
	: _reportingDir(reportingDir), 
	  _pdfPath(_reportingDir.absoluteFilePath("report.pdf"))
{
	assert(_reporter == nullptr);
	_reporter = this;

	QObject::connect(ResultsJsInterface::singleton(), &ResultsJsInterface::pdfPrintingFinished, this, &Reporter::onPdfPrintingFinishedHandler, Qt::UniqueConnection);
	//because of the connection exporting to pdf from the filemenu/results won't work anymore... 
	//but this is only used when JASP is running in reporting mode so that doesnt matter
}

Reporter * Reporter::_reporter = nullptr;

Reporter * Reporter::reporter() { return _reporter; }

bool Reporter::isJaspFileNotDabaseOrSynching() const
{
	//We report through cerr because otherwise it might get messy if JASP is started hidden from a service.
	//The service will just have to catch the output from std::cerr
	if(!DataSetPackage::pkg()->isLoaded() || !DataSetPackage::pkg()->hasDataSet())
	{
		std::cerr << "There is no file loaded or it has no data..." << std::endl;
		return false;
	}
	
	if(DataSetPackage::pkg()->isDatabase())
	{
		if(!DataSetPackage::pkg()->isDatabaseSynching())
		{
			std::cerr << "A non synching database file was loaded, which means there will be no updates..." << std::endl;
			return false;		
		}
	}
	else
	{
		if(DataSetPackage::pkg()->dataFilePath() == "")
		{
			std::cerr << "A jasp file without a datafile was loaded, which means there can be no updates..." << std::endl;

			return false;
		}	
		
		//Lets make sure it actually checks whether the datafile is  being synched or not.
		PreferencesModel::prefs()->setDataAutoSynchronization(true);
	}
			
	return true;
}

void Reporter::analysesFinished()
{
	//The order in which the following are executed is important.
	//The report service will use the "modified" dates of the files created by each of these to keep an eye on JASP.
	//so, when "checkReports() write "sometingToReport.txt" the service knows a refresh has just completed
	//and when "report.complete" is written it knows everything is ready to be sent/read/whatever

	if(PreferencesModel::prefs()->reportingMode())
	{
		checkReports();
		writeResultsJson();
		writeReportLog();
		writeReport();
	}
}

/// Goes through all analyses' results and extracts those parts generated by jaspReport and assings as an array to _reports
bool Reporter::checkReports()
{
	Json::Value reports		= Json::arrayValue;
	_reportsNeeded  = 0;
	_reportsNeutral = 0;

	Analyses::analyses()->applyToAll([&](Analysis * a)
	{
		int reportsNeeded, reportsNeutral;
		Json::Value analysisReports = reportsFromAnalysis(a, reportsNeeded, reportsNeutral);
		_reportsNeeded  += reportsNeeded;
		_reportsNeutral += reportsNeutral;
		
		if(analysisReports.size() || a->isErrorState())
		{
			Json::Value analysisReport = Json::objectValue;
			
			analysisReport["id"]		= int(a->id());
			analysisReport["name"]		= a->name();
			analysisReport["title"]		= a->title();
			analysisReport["module"]	= a->module();
			analysisReport["reports"]	= analysisReports;
			
			if(a->isErrorState())
			{
				analysisReport["error"]			= a->results().get("error", false).asBool();
				analysisReport["errorMessage"]	= a->results().get("errorMessage", "???").asString();
				//lets keep it concise analysisReport["results"]		= a->results();
			}
			
			reports.append(analysisReport);
		}
	});
	
	_reports = reports;
	
	QFile somethingToReportFile(_reportingDir.absoluteFilePath("reportSomething.txt"));
	
	if(somethingToReportFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		somethingToReportFile.write(("Needed reports: " + std::to_string(_reportsNeeded)).c_str()); //Just something simple to check, could be anything really.

	return _reportsNeeded;
}

Json::Value Reporter::reportsFromAnalysis(Analysis * a, int & reportsNeeded, int & reportsNeutral)
{
	reportsNeeded  = 0;
	reportsNeutral = 0;

	std::function<void(stringvec & names, const Json::Value & meta)> reportNameExtractor =
		[&reportNameExtractor](stringvec & names, const Json::Value & meta) -> void
		{
			switch(meta.type())
			{
			case Json::arrayValue:
				for(const Json::Value & subEntry : meta)
					reportNameExtractor(names, subEntry);
				break;

			case Json::objectValue:
			{
				if(meta.get("type", "").asString() == "reportNode")
					names.push_back(meta["name"].asString());

				const Json::Value & collection = meta.get("collection", Json::arrayValue);

				for(const Json::Value & subEntry : collection)
					reportNameExtractor(names, subEntry);

				break;
			}

			default:
				//Why would we even be here? It is just to shut up the warning ;)
				break;
			}
		};

	std::function<void(const stringset & names, const Json::Value & results, Json::Value & reports)> reportsExtractor =
		[&reportsExtractor, &reportsNeeded, &reportsNeutral](const stringset & names, const Json::Value & results, Json::Value & reports) -> void
		{
			switch(results.type())
			{
			case Json::arrayValue:
				for(const Json::Value & subEntry : results)
					reportsExtractor(names, subEntry, reports);
				break;

			case Json::objectValue:
			{
				for(const std::string & name : results.getMemberNames())
					if(names.count(name))
					{
						const Json::Value & report = results[name];
						if(report["report"].asBool())	reportsNeeded++;
						else							reportsNeutral++;

						reports.append(report);
					}
					else if(name != ".meta" && results[name].isObject())
					{
						//std::cerr << "looking at name " << name << std::endl;
						const Json::Value & collection = results[name].get("collection", Json::arrayValue);

						for(const Json::Value & subEntry : collection)
							reportsExtractor(names, subEntry, reports);
					}

				break;
			}

			default:
				//Why would we even be here? It is just to shut up the warning ;)
				break;
			}
		};


	Json::Value analysisReports = Json::arrayValue;

	stringvec reportNames;
	reportNameExtractor(reportNames, a->resultsMeta());

	reportsExtractor(stringset(reportNames.begin(), reportNames.end()), a->results(), analysisReports);

	return analysisReports;
}

bool Reporter::analysisHasReportNeeded(Analysis *a)
{
	int needed, neutral;
	reportsFromAnalysis(a, needed, neutral);
	return needed;
}

void Reporter::writeResultsJson()
{
	QDir tempResultsDir(_reportingDir.absoluteFilePath("resultsTemp"));

	if(tempResultsDir.exists())
		tempResultsDir.removeRecursively();

	tempResultsDir.mkpath(".");

	stringvec tempfiles = TempFiles::retrieveList();
	for(const std::string & tempfile : tempfiles)
		QFile(TempFiles::sessionDirName()) + tempfile).copy(
	QFile resultsFile(_reportingDir.absoluteFilePath("results.json"));
	
	if(resultsFile.open(QIODevice::WriteOnly | QIODevice::Truncate  | QIODevice::Text))
		resultsFile.write(Analyses::analyses()->asJson() .toStyledString().c_str());
}

void Reporter::writeReport()
{
	ResultsJsInterface::singleton()->exportToPDF(_pdfPath);
}

void Reporter::writeReportLog()
{
	QFile reportLogFile(_reportingDir.absoluteFilePath("reportLog.csv"));

	if(!reportLogFile.exists())
	{
		reportLogFile.open(QIODevice::WriteOnly | QIODevice::Truncate  | QIODevice::Text);
		reportLogFile.write("DateTime,NeutralReports,ImportantReports\n");
		reportLogFile.close();
	}

	reportLogFile.open(QIODevice::WriteOnly | QIODevice::Append  | QIODevice::Text);
	QStringList writeThis = {
		QDateTime::currentDateTimeUtc().toString(),
		tq(std::to_string(_reportsNeutral)),
		tq(std::to_string(_reportsNeeded))
	};
	reportLogFile.write((writeThis.join(',').toStdString()+"\n").c_str());
	reportLogFile.close();
}


///Called from ResultsJSInterface and before that WebEngine and makes sure "report.complete" is written/updated to signal to the service some new data is available
void Reporter::onPdfPrintingFinishedHandler(QString pdfPath)
{
	if(_pdfPath != pdfPath) 
	{
		std::cerr << "Got unexpected Reporter::onPdfPrintingFinishedHandler! Expected path: \"" << _pdfPath << "\" but got: \"" << pdfPath << "\"...\nIgnoring it!" << std::endl;
		return;
	}
	
	QFile reportComplete(_reportingDir.absoluteFilePath("report.complete"));
	
	if(reportComplete.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		reportComplete.write(QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString().c_str());
}
