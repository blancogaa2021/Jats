//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//


#include "dynamicmodules.h"
#include "utilities/extractarchive.h"
#include "utilities/settings.h"
#include "gui/messageforwarder.h"
#include "utilities/appdirs.h"


DynamicModules::DynamicModules(QObject *parent) : QObject(parent)
{
	connect(this, &DynamicModules::stopEngines, this, &DynamicModules::enginesStopped, Qt::QueuedConnection);

	_modulesInstallDirectory = AppDirs::modulesDir().toStdString();

	if(!boost::filesystem::exists(_modulesInstallDirectory))
		boost::filesystem::create_directories(_modulesInstallDirectory);

	initializeInstalledModules();
}

DynamicModules::~DynamicModules()
{
	for(auto dynamic : _modules)
		delete dynamic.second;
	_modules.clear();
}

void DynamicModules::initializeInstalledModules()
{
	boost::system::error_code error;
	for (boost::filesystem::directory_iterator itr(_modulesInstallDirectory, error); !error && itr != boost::filesystem::directory_iterator(); itr++)
	{
		std::string path = itr->path().generic_string();
		std::string name = itr->path().filename().generic_string();

		//Development Module should always be fresh!
		if(name == developmentModuleName())			boost::filesystem::remove_all(itr->path());
		else if(name.size() > 0 && name[0] != '.')	initializeModuleFromDir(path);
	}
}

bool DynamicModules::initializeModuleFromDir(std::string moduleDir)
{
	if(moduleDir.size() == 0)
		throw std::runtime_error("Empty path was supplied to DynamicsModules::loadModule..");

	if(moduleDir[moduleDir.size() - 1] != '/')
		moduleDir += '/';

	Modules::DynamicModule	*newMod		= new Modules::DynamicModule(QString::fromStdString(moduleDir), this);

	return initializeModule(newMod);
}

bool DynamicModules::initializeModule(Modules::DynamicModule * module)
{
	try
	{
		std::string	moduleName				= module->name();
		bool		wasAddedAlready			= true;
					_modules[moduleName]	= module;

		if(std::count(_moduleNames.begin(), _moduleNames.end(), moduleName) == 0)
		{
			_moduleNames.push_back(moduleName);
			wasAddedAlready = false;
		}


		if(!module->initialized())
		{
			connect(module, &Modules::DynamicModule::registerForLoading,	this, &DynamicModules::registerForLoading);
			connect(module, &Modules::DynamicModule::registerForInstalling, this, &DynamicModules::registerForInstalling);
			connect(module, &Modules::DynamicModule::descriptionReloaded,	this, &DynamicModules::descriptionReloaded);

			module->initialize();
		}

		if(!wasAddedAlready)
			emit dynamicModuleAdded(module);

		return true;
	}
	catch(std::runtime_error & e)
	{
		MessageForwarder::showWarning("An error occured trying to initialize a module from dir " + module->moduleRLibrary().toStdString() + ", the error was: " + e.what());
		return false;
	}
}

std::string DynamicModules::loadModule(const std::string & moduleName)
{
	try
	{
		if(_modules.count(moduleName) == 0 && !initializeModuleFromDir(moduleDirectory(moduleName)))
			throw std::runtime_error("Couldn't load (and initialize) module " + moduleName);


		Modules::DynamicModule	*loadMe	= _modules[moduleName];
		loadMe->setLoadingNeeded();

		return moduleName;
	}
	catch(std::runtime_error & e)
	{
		MessageForwarder::showWarning("An error occured trying to load module " + moduleName + ", the error was: '" + e.what() + "'");
		return "";
	}
}

void DynamicModules::registerForInstalling(const std::string & moduleName)
{
	_modulesToBeUnloaded.erase(moduleName);
	_modulesToBeLoaded.erase(moduleName);
	_modulesInstallPackagesNeeded.insert(moduleName);

	if(_modules[moduleName]->loaded())	//If the package is already loaded it might be hard to convince R to reinstall it properly, so let's restart the engines
	{
		stopEngines();
		_modulesToBeUnloaded.clear(); //if we are going to restart the engines we can also forget anything that's loaded and needs to be unloaded
		restartEngines();

		_modules[moduleName]->setUnloaded();
	}
}

void DynamicModules::registerForLoading(const std::string & moduleName)
{
	if(_modulesInstallPackagesNeeded.count(moduleName) > 0)
		return; //When the install is done it will trigger the need for loading anyway.

	_modulesToBeUnloaded.erase(moduleName);
	_modulesToBeLoaded.insert(moduleName);
}

void DynamicModules::unloadModule(const std::string & moduleName)
{
	_modulesInstallPackagesNeeded.erase(moduleName);
	_modulesToBeLoaded.erase(moduleName);

	if(_modules.count(moduleName) > 0)
	{
		Modules::DynamicModule * dynMod		= _modules[moduleName];
		_modulesToBeUnloaded[moduleName]	= dynMod->requestJsonForPackageUnloadingRequest();

		dynMod->setUnloaded();

		emit dynamicModuleUnloadBegin(dynMod);
	}
}

void DynamicModules::uninstallModule(const std::string & moduleName)
{
	if(moduleName == developmentModuleName())
	{
		delete _devModDescriptionWatcher;
		delete _devModRWatcher;

		_devModDescriptionWatcher	= nullptr;
		_devModRWatcher				= nullptr;
		_devModSourceDirectory		= QDir();
	}

	std::string modulePath	= moduleDirectory(moduleName);




	if(_modules.count(moduleName) > 0)
	{
		unloadModule(moduleName);
		_modules[moduleName]->setInstalled(false);

		for(int i=int(_moduleNames.size()) - 1; i>=0; i--)
			if(_moduleNames[size_t(i)] == moduleName)
				_moduleNames.erase(_moduleNames.begin() + i);

		delete _modules[moduleName];
		_modules.erase(moduleName);
	}

	removeUninstalledModuleFolder(moduleName);
}

void DynamicModules::removeUninstalledModuleFolder(const std::string & moduleName, bool enginesStopped)
{
#ifdef JASP_DEBUG
	std::cout << "DynamicModules::removeUninstalledModuleFolder("<< moduleName << ", engines " << (enginesStopped ? "stopped" : "started") << ")" << std::endl;
#endif

	std::string modulePath	= moduleDirectory(moduleName);

	try
	{
		if(boost::filesystem::exists(modulePath))
			boost::filesystem::remove_all(modulePath); //Can fail because R might have a library from this folder still loaded. On Windows (and perhaps other OSs) these openend files can't be removed.

	}
	catch (boost::filesystem::filesystem_error & e)
	{
		if(enginesStopped)
			MessageForwarder::showWarning("Something went wrong removing files for module " + moduleName + " at path '" + modulePath + "' and the error was: " + e.what());
		else
		{
#ifdef JASP_DEBUG
			std::cout << "Probably some library was still loaded in R... Let's stop the engines!" << std::endl;
#endif
			stopEngines();
			_modulesToBeUnloaded.clear(); //if we are going to restart the engines we can also forget anything that's loaded and needs to be unloaded
			removeUninstalledModuleFolder(moduleName, true);
			restartEngines();
		}

		return;
	}

	emit dynamicModuleUninstalled(QString::fromStdString(moduleName));
}

Modules::DynamicModule* DynamicModules::requestModuleForSomethingAndRemoveIt(std::set<std::string> & theSet)
{
	if(theSet.size() == 0)
		return NULL;

	std::string installMe = *theSet.begin();
	theSet.erase(installMe);

	return _modules[installMe];
}

Json::Value DynamicModules::getJsonForPackageUnloadingRequest()
{
	std::string firstModule		= _modulesToBeUnloaded.begin()->first;
	Json::Value	unloadRequest	= _modulesToBeUnloaded[firstModule];

	_modulesToBeUnloaded.erase(firstModule);

	return unloadRequest;
}

void DynamicModules::installationPackagesFailed(const QString & moduleName, const QString & errorMessage)
{
	if(_modules.count(moduleName.toStdString()) > 0)
		_modules[moduleName.toStdString()]->setInstallingSucces(false);

	MessageForwarder::showWarning("Installation of Module " + moduleName + " failed", "The installation of Module "+ moduleName+ " failed with the following errormessage:\n"+errorMessage);

	uninstallModule(moduleName.toStdString());
}

void DynamicModules::installationPackagesSucceeded(const QString & moduleName)
{
	std::cout << "Installing packages for module (" << moduleName.toStdString() << ") succeeded!" << std::endl;
	_modules[moduleName.toStdString()]->setInstallingSucces(true);

	auto *dynMod = _modules[moduleName.toStdString()];

	bool wasInitialized = dynMod->initialized();

	if(!wasInitialized)	initializeModule(dynMod);
	else				registerForLoading(moduleName.toStdString());


	if(dynMod->isDevMod())
	{
		if(wasInitialized)
			emit dynamicModuleChanged(dynMod);
		startWatchingDevelopersModule();
	}
}


void DynamicModules::loadingFailed(const QString & moduleName, const QString & errorMessage)
{
	std::cout << "Loading packages for module (" << moduleName.toStdString() << ") failed because of: " << errorMessage.toStdString() << std::endl;
	if(moduleName != "*")
	{
		_modules[moduleName.toStdString()]->setLoadingSucces(false);

		MessageForwarder::showWarning("Loading packages for Module " + moduleName + " failed", "Loading the packages of Module "+ moduleName+ " failed with the following errormessage:\n"+errorMessage);
	}
}

void DynamicModules::loadingSucceeded(const QString & moduleName)
{
	std::cout << "Loading packages for module (" << moduleName.toStdString() << ") succeeded!" << std::endl;

	if(moduleName != "*")
		_modules[moduleName.toStdString()]->setLoadingSucces(true);
}

Modules::AnalysisEntry* DynamicModules::retrieveCorrespondingAnalysisEntry(const Json::Value & jsonFromJaspFile)
{
	std::string moduleName		= jsonFromJaspFile.get("moduleName", "Modulename wasn't actually filled!").asString();

	if(_modules.count(moduleName) > 0)
		return _modules[moduleName]->retrieveCorrespondingAnalysisEntry(jsonFromJaspFile);

	throw Modules::ModuleException(moduleName, "Couldn't find Module " + moduleName +", to use this JASP file you will need to install that first.\nTry the module's website: "  + jsonFromJaspFile.get("moduleWebsite", "jasp-stats.org").asString()	 +  " or, if that doesn't help, you could try to contact the module's maintainer: " + jsonFromJaspFile.get("moduleAuthor", "the JASP team").asString());
}

Modules::AnalysisEntry*	DynamicModules::retrieveCorrespondingAnalysisEntry(const std::string & codedReference)
{
    auto parts = stringUtils::splitString(codedReference, '~');

	if(parts.size() != 3)
		throw Modules::ModuleException("No module", "This isnt a coded reference");

	std::string moduleName		= parts[0];

	return dynamicModule(moduleName)->retrieveCorrespondingAnalysisEntry(codedReference);
}

bool DynamicModules::isFileAnArchive(const QString &  filepath)
{
	return ExtractArchive::isFileAnArchive(filepath.toStdString());
}

QString DynamicModules::getDescriptionJsonFromArchive(const QString &  filepath)
{
	try {

		return QString::fromStdString(ExtractArchive::extractSingleTextFileFromArchive(filepath.toStdString(), "description.json"));
	} catch (...) {
		return "";
	}
}

void DynamicModules::uninstallJASPModule(const QString & moduleName)
{
	uninstallModule(moduleName.toStdString());
}

void DynamicModules::installJASPModule(const QString & moduleZipFilename)
{
	if(!QFile(moduleZipFilename).exists())
	{
		MessageForwarder::showWarning("Cannot install module because " + moduleZipFilename + " does not exist.");
		return;
	}

	Modules::DynamicModule * dynMod = new Modules::DynamicModule(moduleZipFilename.toStdString(), this);

	std::string moduleName = dynMod->name();

	if(moduleIsInstalled(moduleName))
		uninstallModule(moduleName);

	auto modNameQ = QString::fromStdString(moduleName);
	if(!QDir(AppDirs::modulesDir() + "/" + modNameQ).exists())
		QDir(AppDirs::modulesDir()).mkdir(modNameQ);

	_modules[moduleName] = dynMod;

	registerForInstalling(moduleName);
}

void DynamicModules::installJASPDeveloperModule()
{
	_devModSourceDirectory = QDir(Settings::value(Settings::DEVELOPER_FOLDER).toString());

	std::string origin	= _devModSourceDirectory.absolutePath().toStdString(),
				name	= developmentModuleName(),
				dest	= moduleDirectory(name);

	if(origin == "")
	{
		MessageForwarder::showWarning("Select a folder", "To install a development module you need to select the folder you want to watch and load, you can do this under the filemenu, Preferences->Advanced.");
		return;
	}

	if(moduleIsInstalled(name))
		uninstallModule(name);

	QDir destQDir(QString::fromStdString(dest));

	if(!destQDir.exists())
		QDir(AppDirs::modulesDir()).mkdir(QString::fromStdString(name));

	Modules::DynamicModule * devMod = new Modules::DynamicModule(this);
	_modules[name] = devMod;

	registerForInstalling(name);
}

void DynamicModules::startWatchingDevelopersModule()
{
	std::string origin	= _devModSourceDirectory.absolutePath().toStdString(),
				name	= developmentModuleName(),
				dest	= moduleDirectory(name);

	bool	descriptionFound	= false,
			rFound				= false,
			qmlFound			= false,
			iconsFound			= false;

	for(const QFileInfo & entry : _devModSourceDirectory.entryInfoList(QDir::Filter::Dirs | QDir::Filter::Files | QDir::Filter::NoDotAndDotDot))
		if(entry.isDir() && entry.fileName().toLower() == "inst")
		{
			QDir instDir(entry.absoluteFilePath());
			for(const QFileInfo & entryinst : instDir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::Files | QDir::Filter::NoDotAndDotDot))
				if(entryinst.isFile() && entryinst.fileName().toLower() == "description.json")
					descriptionFound = true;
				else if(entryinst.isDir())
				{
					QString dir = entryinst.fileName().toLower();

					if(dir == "qml")	qmlFound	= true;
					if(dir == "icons")	iconsFound	= true;
				}
		}
		else if(entry.isDir() && entry.fileName().toUpper() == "R")
			rFound = true;

	if(!(descriptionFound && rFound && qmlFound && iconsFound))
	{
		MessageForwarder::showWarning("Missing files or folders", "The selected folder cannot be installed as a developer module because it does not contain all the necessary files and folders.\n" +
			std::string(descriptionFound	? "" : "Create a inst/description.json file.\n") +
			std::string(rFound				? "" : "Create a R directory containing your analysis code.\n") +
			std::string(qmlFound			? "" : "Create a inst/qml directory containing your optionsforms.\n") +
			std::string(iconsFound			? "" : "Create a inst/icons directory containing the icons for your ribbonbuttons.\n"));
		return;
	}

	devModCopyDescription();
	devModWatchFolder("R",		_devModRWatcher);
	devModWatchFolder("help",	_devModHelpWatcher);
}

void DynamicModules::devModCopyDescription()
{
	const QString descJson = "inst/description.json";
	QFileInfo src(_devModSourceDirectory.filePath(descJson));
	QFileInfo dst(QString::fromStdString(moduleDirectory(developmentModuleName()) + developmentModuleName() + "/")  + descJson);

	if(!src.exists())
	{
		if(dst.exists())
			MessageForwarder::showWarning("Missing "+descJson.toStdString(), "You seem to have removed "+descJson.toStdString()+" from your development module directory. Without it your module cannot work, make sure to put it back. For now your old "+descJson.toStdString()+" file will be kept.");
		else
		{
			MessageForwarder::showWarning("Missing "+descJson.toStdString(), "You seem to have never had a "+descJson.toStdString()+" in your development module directory. Without it your module cannot work, make sure to create one. How you installed is a bit of a mystery and thus the development module shall be uninstalled now");
			uninstallModule(developmentModuleName());
		}
		return;
	}

	QFile	srcFile(src.absoluteFilePath()),
			dstFile(dst.absoluteFilePath());

	delete _devModDescriptionWatcher;

	_devModDescriptionWatcher	= new QFileSystemWatcher({src.absoluteFilePath()}, this);

	connect(_devModDescriptionWatcher, &QFileSystemWatcher::fileChanged, [=](const QString & path)
	{
		if(path != src.absoluteFilePath())
			throw std::runtime_error("This watcher ought to watch just a single file! ("+src.absoluteFilePath().toStdString()+")");

		if(src.exists()) //file changed because it still exists
		{
#ifdef JASP_DEBUG
		std::cout << "Watched file " << descJson.toStdString() << " was modified." << std::endl;
#endif
			QFile	srcFileChanged(src.absoluteFilePath()),
					dstFileChanged(dst.absoluteFilePath());

			this->regenerateDeveloperModuleRPackage();
		}
		else
		{
			MessageForwarder::showWarning(descJson.toStdString() + " was removed!", "You seem to have removed "+descJson.toStdString()+" but this file is required for your module to work. The development module is going to be uninstalled now.");
			uninstallModule(developmentModuleName());
		}
	});
}

void DynamicModules::devModWatchFolder(QString folder, QFileSystemWatcher * & watcher)
{
	QString infix = folder.toUpper() != "R" ? "inst/" : "",
			dstPath = QString::fromStdString(developmentModuleName()) + "/" + infix + folder;
	QDir	src(_devModSourceDirectory.absoluteFilePath(infix + folder)),
			modDir(QString::fromStdString(moduleDirectory(developmentModuleName()))),
			dst(modDir.absoluteFilePath(dstPath));


	if(!src.exists())
	{
		if(folder != "help") //help is not really necessary
		{
			if(dst.exists())
				MessageForwarder::showWarning("Missing folder " + folder.toStdString(), "You seem to have removed the folder " + folder.toStdString() + " from your development module directory. Without it your module cannot work, make sure to put it back. For now your old folder will be kept.");
			else
			{
				MessageForwarder::showWarning("Missing folder " + folder.toStdString(), "You seem to have never had the folder " + folder.toStdString() + " in your development module directory. Without it your module cannot work, make sure to create one. How you installed is a bit of a mystery and thus the development module shall be uninstalled now");
				uninstallModule(developmentModuleName());
			}
		}
		return;
	}

	delete watcher;
	watcher = new QFileSystemWatcher({_devModSourceDirectory.absoluteFilePath(folder)}, this);

	QStringList extensionFilter;
	for(const std::string & extension :  _acceptedFilesInFolders.at(folder.toLower().toStdString()))
		extensionFilter << "*." + QString::fromStdString(extension) << "*." + QString::fromStdString(extension).toUpper();

	std::set<QString> filesInSource;

	for(QFileInfo entry : src.entryInfoList(extensionFilter, QDir::Filter::Files))
	{
		QFile	srcFile(entry.absoluteFilePath()),
				dstFile(dst.filePath(folder != "help" ? entry.fileName() : entry.fileName().toLower()));

		filesInSource.insert(dstFile.fileName());

		watcher->addPath(entry.absoluteFilePath());
	}

	if(folder.toUpper() != "R")
		for(QFileInfo entry : dst.entryInfoList(QDir::Filter::Files))
		{
			QFile dstFile(entry.absoluteFilePath());

			if(filesInSource.count(dstFile.fileName()) == 0)
				dstFile.remove();
		}

	connect(watcher, &QFileSystemWatcher::fileChanged, [=](const QString & path)
	{
		//If only a file changes then update this single file
		QFileInfo srcFileChanged(path);

		QFile	srcFile(path),
				dstFile(dst.filePath(folder != "help" ? srcFileChanged.fileName() : srcFileChanged.fileName().toLower()));

		if(folder.toUpper() == "R")
			this->regenerateDeveloperModuleRPackage();

		if(folder == "help")		emit	this->reloadHelpPage();
	});

	connect(watcher, &QFileSystemWatcher::directoryChanged, [=, &watcher](QString path)
	{
#ifdef JASP_DEBUG
		std::cout << "Watched folder " << folder.toStdString() << " had a changed directory (file added or removed) on path: " << path.toStdString() << std::endl;
#endif

		if(folder.toUpper() == "R")
			this->regenerateDeveloperModuleRPackage();

		if(folder == "help")			emit	this->reloadHelpPage();
	});
}

void DynamicModules::regenerateDeveloperModuleRPackage()
{
	if(_modules.count(developmentModuleName()) == 0)
		throw std::runtime_error("void DynamicModules::regenerateDeveloperModuleRPackage() called but the development module is not initialized...");

	auto * devMod = _modules[developmentModuleName()];
	devMod->setStatus(moduleStatus::installNeeded);
}

Json::Value	DynamicModules::getJsonForReloadingActiveModules()
{
	Json::Value requestJson(Json::objectValue);

	std::stringstream loadingCode;
	for(int i=0; i<_moduleNames.size(); i++)
	{
		const std::string & modName = _moduleNames[i];

		if(_modules[modName]->readyForUse()) //should be loaded again
			loadingCode << _modules[modName]->generateModuleLoadingR(i == _moduleNames.size() - 1) << "\n";
	}

	if(loadingCode.str() == "")
		loadingCode << "return('" << Modules::DynamicModule::succesResultString() << "')"; //We could also avoid calling getJsonForReloadingActiveModules altogether but whatever

	requestJson["moduleCode"]		= loadingCode.str();
	requestJson["moduleRequest"]	= moduleStatusToString(moduleStatus::loadingNeeded);
	requestJson["moduleName"]		= "*";

	return requestJson;
}

void DynamicModules::enginesStopped()
{
	for(auto & nameMod : _modules)
		nameMod.second->setLoaded(false); //Cause we restarted the engines
}

std::string DynamicModules::moduleDirectory(const std::string & moduleName)	const
{
	return AppDirs::modulesDir().toStdString() + moduleName + '/';
}
