#pragma once
#include "jaspContainer.h"

#ifdef JASP_R_INTERFACE_LIBRARY
#include "jasprcpp_interface.h"
#else
typedef void (*sendFuncDef)(const char *);
typedef bool (*pollMessagesFuncDef)();
//If you edit any function(signatures) or objects for JASP* Rcpp modules and you want to run it as an R-Package you should run Rcpp::compileAttributes from an R instance started in $PWD/JASP-R-Interface/jaspResults
#endif

class jaspResults : public jaspContainer
{
public:
	jaspResults(std::string title, Rcpp::RObject oldState);

	//static functions to allow the values to be set before the constructor is called from R. Would be nicer to just run the constructor in C++ maybe?
	static void setSendFunc(sendFuncDef sendFunc);
	static void setPollMessagesFunc(pollMessagesFuncDef pollFunc);
	static void setResponseData(int analysisID, int revision);
	static void setSaveLocation(const char * newSaveLocation);
	static void setBaseCitation(std::string baseCitation);
	static void setInsideJASP();
	static bool isInsideJASP() { return _insideJASP; }

	void send(std::string otherMsg = "");
	void checkForAnalysisChanged();
	void setStatus(std::string status);
	std::string getStatus();

	const char *	constructResultJson();
	Json::Value		metaEntry() override;
	Json::Value		dataEntry() override;

	void childrenUpdatedCallbackHandler() override;

	void finalizedHandler() override { complete(); }
	void complete();
	void saveResults();

	void loadResults();
	void setErrorMessage(std::string msg);
	void changeOptions(std::string opts);
	void setOptions(std::string opts);
	void pruneInvalidatedData();

	Rcpp::List	getOtherObjectsForState();
	Rcpp::List	getPlotObjectsForState();
	Rcpp::List	getPlotPathsForKeep();
	Rcpp::List	getKeepList();
	std::string	getResults() { return constructResultJson(); }

	std::string _relativePathKeep;

	Json::Value convertToJSON() override;
	void		convertFromJSON_SetFields(Json::Value in) override;

	void startProgressbar(int expectedTicks, int timeBetweenUpdatesInMs = 500);
	void progressbarTick();

	static Rcpp::RObject	getObjectFromEnv(std::string envName);
	static void				setObjectInEnv(std::string envName, Rcpp::RObject obj);
	static bool				objectExistsInEnv(std::string envName);

private:
	static Json::Value				_response;
	static sendFuncDef				_ipccSendFunc;
	static pollMessagesFuncDef		_ipccPollFunc;
	static std::string				_saveResultsHere;
	static std::string				_baseCitation;
	static bool						_insideJASP;
	static const std::string		analysisChangedErrorMessage;

	static Rcpp::Environment		*	_RStorageEnv; //we need this environment to store R objects in a "named" fashion, because then the garbage collector doesn't throw away everything...

	std::string	errorMessage = "";
	Json::Value	_currentOptions		= Json::nullValue,
				_previousOptions	= Json::nullValue;

	void addSerializedPlotObjsForStateFromJaspObject(jaspObject * obj, Rcpp::List & pngImgObj);
	void addPlotPathsForKeepFromJaspObject(jaspObject * obj, Rcpp::List & pngPathImgObj);
	void addSerializedOtherObjsForStateFromJaspObject(jaspObject * obj, Rcpp::List & cumulativeList);
	void fillEnvironmentWithStateObjects(Rcpp::List state);


	int		_progressbarExpectedTicks		= 100,
			_progressbarLastUpdateTime		= -1,
			_progressbarTicks				= 0,
			_progressbarBetweenUpdatesTime	= 500,
			_sendingFeedbackLastTime		= -1,
			_sendingFeedbackInterval		= 500;
};

void JASPresultFinalizer(jaspResults * obj);

Rcpp::RObject givejaspResultsModule();


class  jaspResults_Interface : public jaspContainer_Interface
{
public:
	jaspResults_Interface(jaspObject * dataObj) : jaspContainer_Interface(dataObj) {}

	void		send()								{ ((jaspResults*)myJaspObject)->send();								}
	void		complete()							{ ((jaspResults*)myJaspObject)->complete();							}
	void		setErrorMessage(std::string msg)	{ ((jaspResults*)myJaspObject)->setErrorMessage(msg);				}
	Rcpp::List	getOtherObjectsForState()			{ return ((jaspResults*)myJaspObject)->getOtherObjectsForState();	}
	Rcpp::List	getPlotObjectsForState()			{ return ((jaspResults*)myJaspObject)->getPlotObjectsForState();	}
	Rcpp::List	getKeepList()						{ return ((jaspResults*)myJaspObject)->getKeepList();				}
	void		progressbarTick()					{ ((jaspResults*)myJaspObject)->progressbarTick();					}
	std::string getResults()						{ return ((jaspResults*)myJaspObject)->getResults();				}

	void		startProgressbar(int expectedTicks)									{ ((jaspResults*)myJaspObject)->startProgressbar(expectedTicks); }
	void		startProgressbarMs(int expectedTicks, int timeBetweenUpdatesInMs)	{ ((jaspResults*)myJaspObject)->startProgressbar(expectedTicks, timeBetweenUpdatesInMs); }

	void		setOptions(std::string opts)		{ ((jaspResults*)myJaspObject)->setOptions(opts); }
	void		changeOptions(std::string opts)		{ ((jaspResults*)myJaspObject)->changeOptions(opts); }

	JASPOBJECT_INTERFACE_PROPERTY_FUNCTIONS_GENERATOR(jaspResults, std::string,	_relativePathKeep, RelativePathKeep)
};


RCPP_EXPOSED_CLASS_NODECL(jaspResults_Interface)
