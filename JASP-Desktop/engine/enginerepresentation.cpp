#include "enginerepresentation.h"
#include "gui/preferencesmodel.h"
#include "gui/messageforwarder.h"
#include "utilities/qutils.h"
#include "log.h"

EngineRepresentation::EngineRepresentation(IPCChannel * channel, QProcess * slaveProcess, QObject * parent)
	: QObject(parent), _channel(channel)
{
	setSlaveProcess(slaveProcess);
}


void EngineRepresentation::setSlaveProcess(QProcess * slaveProcess)
{
	_slaveCrashed = false;
	_slaveProcess = slaveProcess;
	_slaveProcess->setParent(this);

	connect(_slaveProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),	this, &EngineRepresentation::processFinished);
}

EngineRepresentation::~EngineRepresentation()
{
	Log::log() << "~EngineRepresentation()" << std::endl;

	if(_slaveProcess != nullptr)
	{
		_slaveProcess->terminate();
		_slaveProcess->kill();
	}

	delete _channel;
	_channel = nullptr;
}

void EngineRepresentation::sendString(std::string str)
{
#ifdef PRINT_ENGINE_MESSAGES
	Log::log() << "sending to jaspEngine: " << str << "\n" << std::endl;
#endif
	_channel->send(str);
}

void EngineRepresentation::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Log::log() << "Engine # " << channelNumber() << " finished " << (exitStatus == QProcess::ExitStatus::NormalExit ? "normally" : "crashing") << " and with exitCode " << exitCode << "!" << std::endl;

	_slaveProcess->deleteLater();
	_slaveProcess = nullptr;
	_slaveCrashed = exitStatus == QProcess::ExitStatus::CrashExit;

	if(exitCode != 0 || _slaveCrashed)
		handleEngineCrash();
}

void EngineRepresentation::handleEngineCrash()
{
	Log::log() << "EngineRepresentation::handleEngineCrash():\n" << currentState() << std::endl;

	switch(_engineState)
	{
	case engineState::analysis:
		if(_analysisInProgress)
		{
			_analysisInProgress->setErrorInResults("The engine crashed while trying to run this analysis...");
			clearAnalysisInProgress();
		}
		break;

	case engineState::filter:
		emit processFilterErrorMsg("The engine crashed while trying to run the filter...", _lastRequestId);
		break;

	case engineState::computeColumn:
		emit computeColumnFailed(tq(_lastCompColName), "The engine crashed while trying to compute this column...");
		break;

	case engineState::rCode:
		emit rCodeReturned("The engine crashed while trying to run rscript...", _lastRequestId);
		break;

	case engineState::logCfg:
		//So if the engine crashes on log config change request then we can still continue because it will also get the proper settings on startup.
		//And if it is still broken then we will simply see a crash screen then...
		break;

	case engineState::idle:
	{
		static int idleCrashes = 0;

		if(idleCrashes++ < 3)
			break; //Try it a couple of times because the engine might have crashed right after completing an analysis (because of .crashPlease() for instance) or something. But if it keeps happening something is probably wrong
	}

	default: //If not one of the above then let the engine crash and burn (https://www.youtube.com/watch?v=UtUpXPiSJEg)
		emit engineTerminated();
		return;
	}

	_engineState = engineState::initializing;

	emit requestEngineRestart(channelNumber());
}

void EngineRepresentation::clearAnalysisInProgress()
{
	Log::log() << "Engine " << channelNumber() << " clears current analysis in progress (" << (_analysisInProgress ? _analysisInProgress->name() : "???" ) << ")" << std::endl;

	_analysisInProgress = nullptr;
	_engineState		= engineState::idle;
}

void EngineRepresentation::setAnalysisInProgress(Analysis* analysis)
{
	analysis->incrementRevision(); // Increment revision only when the analysis request is about to be sent
	if(_engineState == engineState::analysis)
	{
		if(_analysisInProgress == analysis)	return; //we are already busy with this analysis so everything is fine
		else								throw std::runtime_error("Engine " + std::to_string(_channel->channelNumber()) + " is running another analysis. Yet you are trying to set an analysis in progress on it..");
	}

	if(_engineState != engineState::idle)	throw std::runtime_error("Engine " + std::to_string(_channel->channelNumber()) + " is not idle! Yet you are trying to set an analysis in progress on it..");

	_analysisInProgress = analysis;
	_engineState		= engineState::analysis;
}

void EngineRepresentation::process()
{
	if (_engineState == engineState::idle)
	{
		if		(_stopRequested)	sendStopEngine();
		else if	(_pauseRequested)	sendPauseEngine();
		return;
	}

	std::string data;

	if (_channel->receive(data))
	{
#ifdef PRINT_ENGINE_MESSAGES
		Log::log() << "message received" <<std::endl;
#endif

		Json::Value json;
		bool		jsonIsOK = false;

		try { jsonIsOK = Json::Reader().parse(data, json) && (json.get("typeRequest", Json::nullValue).isString() || _engineState == engineState::analysis); }
		catch(...) {}

		if(!jsonIsOK)
		{
			Log::log() << "Malformed reply from engine in state " << _engineState << ": '" << data << "'" << std::endl;
			throw std::runtime_error("Malformed reply from engine!");
		}

		engineState typeRequest = engineStateFromString(json.get("typeRequest", "analysis").asString());

		switch(typeRequest)
		{
		case engineState::filter:			processFilterReply(json);			break;
		case engineState::rCode:			processRCodeReply(json);			break;
		case engineState::analysis:			processAnalysisReply(json);			break;
		case engineState::computeColumn:	processComputeColumnReply(json);	break;
		case engineState::paused:			processEnginePausedReply();			break;
		case engineState::resuming:			processEngineResumedReply();		break;
		case engineState::stopped:			processEngineStoppedReply();		break;
		case engineState::moduleRequest:	processModuleRequestReply(json);	break;
		case engineState::logCfg:			processLogCfgReply();				break;
		case engineState::settings:			processSettingsReply();				break;
		default:							throw std::logic_error("If you define new engineStates you should add them to the switch in EngineRepresentation::process()!");
		}
	}
}

void EngineRepresentation::runScriptOnProcess(RFilterStore * filterStore)
{
	Json::Value json = Json::Value(Json::objectValue);

	_engineState			= engineState::filter;

	json["typeRequest"]		= engineStateToString(_engineState);
	json["generatedFilter"] = filterStore->generatedfilter.toStdString();
	json["requestId"]		= filterStore->requestId;

	_lastRequestId			= filterStore->requestId;

	QString dataFilter = filterStore->script == "" ? "*" : filterStore->script;
	json["filter"] = dataFilter.toStdString();

	Log::log() << "sending filter with requestID " << filterStore->requestId << " to engine" << std::endl;

	sendString(json.toStyledString());
}

void EngineRepresentation::processFilterReply(Json::Value & json)
{
	if(_engineState != engineState::filter)
		throw std::runtime_error("Received an unexpected filter reply!");
	_engineState = engineState::idle;

#ifdef PRINT_ENGINE_MESSAGES
			Log::log() << "msg is filter reply" << std::endl;
#endif

	int requestId = json.get("requestId", -1).asInt();

	emit filterDone(requestId);

	if(json.get("filterResult", Json::Value(Json::intValue)).isArray()) //If the result is an array then it came from the engine.
	{
		std::vector<bool> filterResult;
		for(Json::Value & jsonResult : json.get("filterResult", Json::Value(Json::arrayValue)))
			filterResult.push_back(jsonResult.asBool());

		emit processNewFilterResult(filterResult, requestId);

		if(json.get("filterError", "").asString() != "")
			emit processFilterErrorMsg(QString::fromStdString(json.get("filterError", "there was a warning").asString()), requestId);
	}
	else
		emit processFilterErrorMsg(QString::fromStdString(json.get("filterError", "something went wrong").asString()), requestId);
}

void EngineRepresentation::runScriptOnProcess(RScriptStore * scriptStore)
{
	Json::Value json = Json::Value(Json::objectValue);

	_engineState			= engineState::rCode;
	json["typeRequest"]		= engineStateToString(_engineState);
	json["rCode"]			= scriptStore->script.toStdString();
	json["requestId"]		= scriptStore->requestId;
	json["whiteListed"]		= scriptStore->whiteListedVersion;

	_lastRequestId			= scriptStore->requestId;

	sendString(json.toStyledString());
}


void EngineRepresentation::processRCodeReply(Json::Value & json)
{
	if(_engineState != engineState::rCode)
		throw std::runtime_error("Received an unexpected rCode reply!");
	_engineState = engineState::idle;

	std::string rCodeResult = json.get("rCodeResult", "").asString();
	int requestId			= json.get("requestId", -1).asInt();

	emit rCodeReturned(QString::fromStdString(rCodeResult), requestId);

	Log::log() << "rCode reply for request (" << requestId << ") returned: " << rCodeResult << " with error: '" << json.get("rCodeError", "no error") << "'\n" <<  std::flush;
}


void EngineRepresentation::runScriptOnProcess(RComputeColumnStore * computeColumnStore)
{
	Json::Value json = Json::Value(Json::objectValue);

	_engineState			= engineState::computeColumn;

	json["typeRequest"]		= engineStateToString(_engineState);
	json["columnName"]		= computeColumnStore->_columnName.toStdString();
	json["computeCode"]		= computeColumnStore->script.toStdString();
	json["columnType"]		= columnTypeToString(computeColumnStore->_columnType);

	_lastCompColName		= json["columnName"].asString();

	sendString(json.toStyledString());
}


void EngineRepresentation::processComputeColumnReply(Json::Value & json)
{
	if(_engineState != engineState::computeColumn)
		throw std::runtime_error("Received an unexpected computeColumn reply!");
	_engineState = engineState::idle;


	std::string result		= json.get("result", "some string that is not 'TRUE' or 'FALSE'").asString();
	std::string error		= json.get("error", "").asString();
	std::string columnName	= json.get("columnName", "").asString();

	if(result == "TRUE")		emit computeColumnSucceeded(QString::fromStdString(columnName), QString::fromStdString(error), true);
	else if(result == "FALSE")	emit computeColumnSucceeded(QString::fromStdString(columnName), QString::fromStdString(error), false);
	else						emit computeColumnFailed(	QString::fromStdString(columnName), QString::fromStdString(error == "" ? "Unknown Error" : error));
}

void EngineRepresentation::runAnalysisOnProcess(Analysis *analysis)
{
#ifdef PRINT_ENGINE_MESSAGES
	Log::log() << "send request for analysis-id #" << analysis->id() << " to jaspEngine on channel #" << channelNumber() << std::endl;
#endif

	setAnalysisInProgress(analysis);

	Json::Value json(analysis->createAnalysisRequestJson());

#ifdef PRINT_ENGINE_MESSAGES
	Log::log() << "sending: " << json.toStyledString() << std::endl;
#endif

	_channel->send(json.toStyledString());

}

void EngineRepresentation::analysisRemoved(Analysis * analysis)
{
	if(_engineState != engineState::analysis || _analysisInProgress != analysis)
		return;

	_idRemovedAnalysis = analysis->id();
	abortAnalysisInProgress();
	_analysisInProgress = nullptr; //Because it will be deleted!
	//But we keep the engineState at analysis to make sure another analysis won't try to run until the aborted one gets the message!
}

void EngineRepresentation::processAnalysisReply(Json::Value & json)
{
#ifdef PRINT_ENGINE_MESSAGES
	Log::log() << "Analysis reply: " << json.toStyledString() << std::endl;
#endif

	if(_engineState == engineState::paused || _engineState == engineState::resuming || _engineState == engineState::idle)
	{
		Log::log() << "Do not process analysis reply because engineState is paused, resuming or idle" << std::endl;
		return;
	}

	if(_engineState != engineState::analysis)
	{
		Log::log() << "The engineState is not analysis!!!" << std::endl;
		return;
	}

	int id						= json.get("id",		-1).asInt();
	int revision				= json.get("revision",	-1).asInt();
	
	Json::Value progress		= json.get("progress",	Json::nullValue);
	Json::Value results			= json.get("results",	Json::nullValue);

	analysisResultStatus status	= analysisResultStatusFromString(json.get("status", "error").asString());

	if(_analysisInProgress == nullptr && id == _idRemovedAnalysis)
	{
		Log::log() << "Reply was for an analysis that was removed, we now check if it was done or not, the resultstatus was: " << analysisResultStatusToString(status) << std::endl;
		switch(status)
		{
		case analysisResultStatus::changed:
		case analysisResultStatus::complete:
		case analysisResultStatus::fatalError:
		case analysisResultStatus::validationError:
			_engineState		= engineState::idle;
			_idRemovedAnalysis	= -1;

			Log::log() << "Analysis got the message and we now reset the engineStatus to idle!" << std::endl;
			break;

		default:
			Log::log() << "Analysis ignores the abort it got and keeps going..." << std::endl;
			break;
		}
		return;
	}

	Analysis *analysis			= _analysisInProgress;

	if (analysis->id() != id || analysis->revision() < revision)
		throw std::runtime_error("Received results for wrong analysis!");

	if(analysis->revision() > revision) //I guess we changed some option or something?
	{
		Log::log() << "Analysis reply was for an older revision (" << revision << ") than the one currently requested (" << analysis->revision() << "), so it can be ignored." << std::endl;

		if(_pauseRequested || _analysisAborted == analysis)
		{
			Log::log() << "A pause was requested or some setting changed, so the analysis was aborted." << std::endl;
			clearAnalysisInProgress();
		}

		return;
	}



	Log::log() << "Resultstatus of analysis was " << analysisResultStatusToString(status) << " and it will now be processed." << std::endl;

	switch(status)
	{
	case analysisResultStatus::imageSaved:
		if (results.get("error", false).asBool())
		{
			MessageForwarder::showWarning(tr("Error saving plot"), tr("Unfortunately the plot could not be saved.\n\nError message:\n%1\n\nIf the problem persists, please report the message above at: https://jasp-stats.org/bug-reports").arg(tq(results.get("errorMessage", "").asString())));
			analysis->setStatus(Analysis::Complete);
		}
		else
			analysis->imageSaved(results);
		clearAnalysisInProgress();
		break;


	case analysisResultStatus::imageEdited:
		if (results.get("error", false).asBool())
			MessageForwarder::showWarning(tr("Error resizing plot"), tr("Unfortunately the plot could not be saved.\n\nError message:\n%1\n\nIf the problem persists, please report the message above at: https://jasp-stats.org/bug-reports").arg(tq(results.get("errorMessage", "").asString())));

		analysis->imageEdited(results); // if an error occurs js needs to resize the plot back to the old size
		emit plotEditorRefresh();

		clearAnalysisInProgress();
		break;

	case analysisResultStatus::imagesRewritten:
		analysis->imagesRewritten();
		clearAnalysisInProgress();
		break;

	case analysisResultStatus::validationError:
	case analysisResultStatus::fatalError:
	case analysisResultStatus::complete:
	case analysisResultStatus::inited:
		analysis->setResults(results, status);
		clearAnalysisInProgress();

		if(analysis->columnsCreated().size() > 0)
			checkForComputedColumns(results);
		break;

	case analysisResultStatus::running:
	default:
		analysis->setResults(results, status, progress);
		break;
	}

}

void EngineRepresentation::checkForComputedColumns(const Json::Value & results)
{
	if(results.isArray())
	{
		for(const Json::Value & row : results)
			checkForComputedColumns(row);
	}
	else if(results.isObject())
	{
		auto members = results.getMemberNames();
		std::set<std::string> memberset(members.begin(), members.end());

		if(memberset.count("columnName") > 0 && memberset.count("columnType") > 0 && memberset.count("dataChanged") > 0)
		{
			Log::log() << "The analysis reply contained information on changed computed columns: " << results.toStyledString() << std::endl;

			//jaspColumnType	columnType	= jaspColumnTypeFromString(results["columnType"].asString()); This would work if jaspColumn wasn't defined in jaspColumn.h and Windows would not need to have that separately in a DLL... But it isn't really needed here anyway.
			std::string		columnName	= results["columnName"].asString();
			bool			dataChanged	= results["dataChanged"].asBool();
			bool			typeChanged	= results["typeChanged"].asBool();

			emit computeColumnSucceeded(tq(columnName), "", dataChanged);

			if(typeChanged)
				emit columnDataTypeChanged(tq(columnName));
		}
		else
			for(std::string member : members)
				checkForComputedColumns(results[member]);
	}
}

void EngineRepresentation::handleRunningAnalysisStatusChanges()
{
	if (_engineState != engineState::analysis || _idRemovedAnalysis >= 0)
		return;

	if((_analysisInProgress->isEmpty() || _analysisInProgress->isAborted()) && _analysisAborted != _analysisInProgress)
		runAnalysisOnProcess(_analysisInProgress);
}

void EngineRepresentation::killEngine()
{
	_slaveProcess->kill();
}

void EngineRepresentation::stopEngine()
{
	_stopRequested = true;
}

void EngineRepresentation::sendStopEngine()
{
	Json::Value json		= Json::Value(Json::objectValue);
	_engineState			= engineState::stopRequested;
	json["typeRequest"]		= engineStateToString(_engineState);

	Log::log() << "informing engine that it ought to stop" << std::endl;

	sendString(json.toStyledString());
}

void EngineRepresentation::restartEngine(QProcess * jaspEngineProcess)
{
	Log::log() << "informing engine that it ought to restart" << std::endl;

	if(_slaveProcess != nullptr && _slaveProcess != jaspEngineProcess)
	{
		_slaveProcess->kill();
		_slaveProcess->deleteLater();
		Log::log() << "EngineRepresentation::restartEngine says: Engine already has jaspEngine process!" << std::endl;
	}

	sendString("");
	setSlaveProcess(jaspEngineProcess);
	_stopRequested	= false;
	resumeEngine();
}

void EngineRepresentation::pauseEngine()
{
	_pauseRequested = true;
	if(_analysisInProgress)
		_analysisInProgress->abort();
}

void EngineRepresentation::sendPauseEngine()
{
	Json::Value json		= Json::Value(Json::objectValue);
	_engineState			= engineState::pauseRequested;
	json["typeRequest"]		= engineStateToString(_engineState);

	Log::log() << "informing engine that it ought to pause for a bit" << std::endl;

	sendString(json.toStyledString());
}

void EngineRepresentation::resumeEngine()
{
	if(_engineState != engineState::paused && _engineState != engineState::stopped && _engineState != engineState::initializing)
		throw std::runtime_error("Attempt to resume engine made but it isn't paused or stopped");

	_pauseRequested			= false;
	_engineState			= engineState::resuming;
	Json::Value json		= Json::Value(Json::objectValue);
	json["typeRequest"]		= engineStateToString(_engineState);

	Log::log() << "informing engine that it may resume" << std::endl;

	sendString(json.toStyledString());
}

void EngineRepresentation::processEnginePausedReply()
{
	if(_engineState != engineState::pauseRequested)
		throw std::runtime_error("Received an unexpected engine paused reply!");

	_engineState = engineState::paused;
}

void EngineRepresentation::processEngineResumedReply()
{
	if(_engineState != engineState::resuming && _engineState != engineState::initializing)
		throw std::runtime_error("Received an unexpected engine resumed reply!");

	if(_engineState == engineState::initializing)
		_settingsChanged = true; //Make sure we send the settings at least once

	_engineState = engineState::idle;
}

void EngineRepresentation::processEngineStoppedReply()
{
	if(_engineState != engineState::stopRequested)
		throw std::runtime_error("Received an unexpected engine stopped reply!");

	_engineState = engineState::stopped;
}

void EngineRepresentation::runModuleRequestOnProcess(Json::Value request)
{
	_engineState			= engineState::moduleRequest;
	request["typeRequest"]	= engineStateToString(_engineState);

	sendString(request.toStyledString());
}

void EngineRepresentation::processModuleRequestReply(Json::Value & json)
{
	if(_engineState != engineState::moduleRequest)
		throw std::runtime_error("Received an unexpected moduleRequest reply!");
	_engineState = engineState::idle;

	moduleStatus moduleRequest	= moduleStatusFromString(json["moduleRequest"].asString());
	bool succes					= json["succes"].asBool();
	QString moduleName			= QString::fromStdString(json["moduleName"].asString());
	auto getError				= [&](){ return QString::fromStdString(json.get("error", "Unknown error").asString()); };

	switch(moduleRequest)
	{
	case moduleStatus::installNeeded:
		if(succes)	emit moduleInstallationSucceeded(moduleName);
		else		emit moduleInstallationFailed(moduleName, getError());
		break;

	case moduleStatus::loadingNeeded:
		if(succes)	emit moduleLoadingSucceeded(moduleName, channelNumber());
		else		emit moduleLoadingFailed(moduleName, getError(), channelNumber());
		break;

	case moduleStatus::unloadingNeeded:
		emit moduleUnloadingFinished(moduleName, channelNumber());
		break;

	default:
		throw std::runtime_error("Unsupported module request reply to EngineRepresentation::processModuleRequestReply!");
	}
}

void EngineRepresentation::sendLogCfg()
{
	Log::log() << "EngineRepresentation::sendLogCfg()" << std::endl;

	if(_engineState != engineState::idle)
		throw std::runtime_error("EngineRepresentation::sendLogCfg() expects to be run from an idle engine.");

	_engineState		= engineState::logCfg;
	Json::Value msg		= Log::createLogCfgMsg();
	msg["typeRequest"]	= engineStateToString(_engineState);

	sendString(msg.toStyledString());
}

void EngineRepresentation::processLogCfgReply()
{
	_engineState = engineState::idle;

	emit logCfgReplyReceived(channelNumber());
}

std::string EngineRepresentation::currentState() const
{
	try {
		std::stringstream out;

		out << "**Engine # " << channelNumber() << "** process ";

		if(_slaveCrashed)		out << "*crashed*";
		else if(!_slaveProcess)	out << "*does not exist (anymore)*";
		else
			switch(_slaveProcess->state())
			{
			case QProcess::ProcessState::NotRunning:	out << " is *not running*";		break;
			case QProcess::ProcessState::Running:		out << " is *running*";			break;
			case QProcess::ProcessState::Starting:		out << " is *starting*";		break;
			}

		out << " and it's state is *" << engineStateToString(_engineState) << "*.";

		if(_engineState == engineState::analysis)
			try			{	out << " Analysis is " << (_analysisInProgress ? ("**" + _analysisInProgress->name() + "** with status *" + Analysis::statusToString(_analysisInProgress->status()) + "*") : "*???*") << ""; }
			catch(...)	{	out << " Something is wrong with the analysis..."; }

		return out.str();
	}
	catch (...)
	{
		return "EngineRepresentation::currentState() didn't work...";
	}
}

//This function is now only used by settingsChanged
void EngineRepresentation::abortAnalysisInProgress()
{
	if(_engineState == engineState::analysis && _analysisInProgress != nullptr)
	{
		if(_analysisInProgress->status() != Analysis::Status::Aborting && _analysisInProgress->status() != Analysis::Status::Aborted)
			_analysisInProgress->setStatus(Analysis::Status::Aborting);

		runAnalysisOnProcess(_analysisInProgress);

		_analysisAborted = _analysisInProgress;
	}
}

void EngineRepresentation::settingsChanged()
{
	Log::log() << "void EngineRepresentation::settingsChanged()" << std::endl;
	_settingsChanged = true;
	abortAnalysisInProgress();
}

void EngineRepresentation::sendSettings()
{
	Log::log() << "EngineRepresentation::sendSettings()" << std::endl;

	if(_engineState != engineState::idle)
		throw std::runtime_error("EngineRepresentation::sendSettings() expects to be run from an idle engine.");

	_engineState			= engineState::settings;
	Json::Value msg			= Json::objectValue;
	msg["typeRequest"]		= engineStateToString(_engineState);
	msg["ppi"]				= PreferencesModel::prefs()->plotPPI();
	msg["developerMode"]	= PreferencesModel::prefs()->developerMode();
	msg["imageBackground"]	= fq(PreferencesModel::prefs()->plotBackground());
	msg["languageCode"]		= fq(PreferencesModel::prefs()->languageCode());

	sendString(msg.toStyledString());

	_settingsChanged = false;
}

void EngineRepresentation::processSettingsReply()
{
	_engineState = engineState::idle;

	if(_analysisAborted && _analysisAborted->isAborted())
		_analysisAborted->refresh();

	_analysisAborted = nullptr;
}
