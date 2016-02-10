#include "fsbmosf.h"

#include <QGridLayout>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QFile>

#include "fsentry.h"
#include "onlinedatamanager.h"

FSBMOSF::FSBMOSF()
{
	_dataManager = NULL;
	_manager = NULL;
	_isAuthenticated = false;

	_rootPath = _path = "Projects";
}

FSBMOSF::~FSBMOSF()
{

}

void FSBMOSF::setOnlineDataManager(OnlineDataManager *odm)
{
	_dataManager = odm;

	_manager = odm->getNetworkAccessManager(OnlineDataManager::OSF);

	if (_isAuthenticated == false && _dataManager != NULL && _dataManager->authenticationSuccessful(OnlineDataManager::OSF))
		setAuthenticated(true);
}

bool FSBMOSF::requiresAuthentication() const
{
	return true;
}

void FSBMOSF::authenticate(const QString &username, const QString &password)
{
	_dataManager->setAuthentication(OnlineDataManager::OSF, username, password);

	bool success = _dataManager->authenticationSuccessful(OnlineDataManager::OSF);

	_settings.sync();

	if (success)
	{
		_settings.setValue("OSFUsername", username);
		_settings.setValue("OSFPassword", password);
	}
	else
	{
		_settings.remove("OSFUsername");
		_settings.remove("OSFPassword");
	}

	_settings.sync();

	setAuthenticated(success);
}

void FSBMOSF::setAuthenticated(bool value)
{
	if (value)
	{
		_isAuthenticated = true;
		emit authenticationSuccess();
		refresh();
	}
	else
	{
		_isAuthenticated = false;
		emit authenticationFail("Username and password are not correct. Please try again.");
	}
}

bool FSBMOSF::isAuthenticated() const
{
	return _isAuthenticated;
}

void FSBMOSF::clearAuthentication()
{
	_isAuthenticated = false;

	_dataManager->clearAuthentication(OnlineDataManager::OSF);
	_entries.clear();
	_pathUrls.clear();
	setPath(_rootPath);
	emit entriesChanged();

	_settings.sync();
	_settings.remove("OSFUsername");
	_settings.remove("OSFPassword");
	_settings.sync();

	emit authenticationClear();
}

void FSBMOSF::refresh()
{
	if (_manager == NULL || _isAuthenticated == false)
		return;

	emit processingEntries();

	_entries.clear();

	if (_path == "Projects")
		loadProjects();
	else {
		OnlineNodeData nodeData = _pathUrls[_path];
		if (nodeData.isFolder)
			loadFilesAndFolders(QUrl(nodeData.contentsPath), nodeData.level + 1);
		if (nodeData.isComponent)
			loadFilesAndFolders(QUrl(nodeData.childrenPath), nodeData.level + 1);
	}
}

FSBMOSF::OnlineNodeData FSBMOSF::currentNodeData()
{
	return _pathUrls[_path];
}

void FSBMOSF::loadProjects() {

	QUrl url("https://api.osf.io/v2/users/me/nodes/");
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.api+json");
	request.setRawHeader("Accept", "application/vnd.api+json");

	QNetworkReply* reply = _manager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(gotProjects()));
}

void FSBMOSF::gotProjects()
{
	QNetworkReply *reply = (QNetworkReply*)this->sender();

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray data = reply->readAll();
		QString dataString = (QString) data;

		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(dataString.toUtf8(), &error);

		QJsonObject json = doc.object();
		QJsonArray dataArray = json.value("data").toArray();

		if ( dataArray.size() > 0 )
			_entries.clear();

		foreach (const QJsonValue & value, dataArray) {
			QJsonObject nodeObject = value.toObject();

			QJsonObject attrObj = nodeObject.value("attributes").toObject();

			QString category = attrObj.value("category").toString();
			if (category != "project")
				continue;

			OnlineNodeData nodeData;

			nodeData.name = attrObj.value("title").toString();
			nodeData.isFolder = true;
			nodeData.isComponent = true;

			nodeData.contentsPath = getRelationshipUrl(nodeObject, "files");
			nodeData.childrenPath = getRelationshipUrl(nodeObject, "children");

			QJsonObject topLinksObj = nodeObject.value("links").toObject();

			nodeData.nodePath = topLinksObj.value("self").toString();
			nodeData.level = 1;
			nodeData.canCreateFolders = false;
			nodeData.canCreateFiles = false;

			QString path = _path + "/" + nodeData.name;
			_entries.append(createEntry(path, FSEntry::Folder));

			_pathUrls[path] = nodeData;

		}
	}

	emit entriesChanged();
	reply->deleteLater();
}

void FSBMOSF::loadFilesAndFolders(QUrl url, int level)
{
	parseFilesAndFolders(url, level);
}

void FSBMOSF::parseFilesAndFolders(QUrl url, int level)
{
	_level = level;

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.api+json");
	request.setRawHeader("Accept", "application/vnd.api+json");

	QNetworkReply* reply = _manager->get(request);

	connect(reply, SIGNAL(finished()), this, SLOT(gotFilesAndFolders()));
}

void FSBMOSF::gotFilesAndFolders()
{
	QNetworkReply *reply = (QNetworkReply*)this->sender();

	bool finished = false;

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray data = reply->readAll();
		QString dataString = (QString) data;

		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(dataString.toUtf8(), &error);

		QJsonObject json = doc.object();
		QJsonArray dataArray = json.value("data").toArray();

		if ( dataArray.size() > 0 )
			_entries.clear();

		foreach (const QJsonValue & value, dataArray) {
			QJsonObject nodeObject = value.toObject();

			OnlineNodeData nodeData;

			nodeData.isComponent = nodeObject.value("type").toString() == "nodes";

			FSEntry::EntryType entryType = FSEntry::Other;
			QJsonObject attrObj = nodeObject.value("attributes").toObject();

			if (nodeData.isComponent == false)
			{
				QString kind = attrObj.value("kind").toString();
				if (kind != "folder" && kind != "file")
					continue;

				nodeData.name = attrObj.value("name").toString();

				if (kind == "folder")
					entryType = FSEntry::Folder;
				else if (nodeData.name.endsWith(".jasp", Qt::CaseInsensitive))
					entryType = FSEntry::JASP;
				else if (nodeData.name.endsWith(".csv", Qt::CaseInsensitive) || nodeData.name.endsWith(".txt", Qt::CaseInsensitive))
					entryType = FSEntry::CSV;
		#ifdef QT_DEBUG
				else if (nodeData.name.endsWith(".spss", Qt::CaseInsensitive))
					entryType = FSEntry::SPSS;
		#endif
				else
					continue;
			}
			else
			{
				entryType = FSEntry::Folder;
				nodeData.name = attrObj.value("title").toString();
			}

			if (entryType == FSEntry::Folder) {

				nodeData.contentsPath = getRelationshipUrl(nodeObject, "files");
				nodeData.childrenPath = getRelationshipUrl(nodeObject, "children");

				QJsonObject topLinksObj = nodeObject.value("links").toObject();

				if (nodeData.isComponent)
					nodeData.nodePath = topLinksObj.value("self").toString();
				else
					nodeData.nodePath = topLinksObj.value("info").toString();

				nodeData.uploadPath = topLinksObj.value("upload").toString();
				nodeData.isFolder = true;

				nodeData.canCreateFolders = topLinksObj.contains("new_folder");
				nodeData.canCreateFiles = topLinksObj.contains("upload");

				if (nodeData.nodePath == "")
					nodeData.nodePath = reply->url().toString() + "#folder://" + nodeData.name;
			}
			else
			{
				QJsonObject linksObj = nodeObject.value("links").toObject();
				nodeData.isFolder = false;

				nodeData.uploadPath = linksObj.value("upload").toString();
				nodeData.downloadPath = linksObj.value("download").toString();
				nodeData.nodePath = linksObj.value("info").toString();

				nodeData.canCreateFolders = false;
				nodeData.canCreateFiles = false;
			}


			QString path = _path + "/" + nodeData.name;

			nodeData.level = _level;

			_entries.append(createEntry(path, entryType));
			_pathUrls[path] = nodeData;

		}

		QJsonObject contentLevelLinks = json.value("links").toObject();

		QJsonValue nextContentList = contentLevelLinks.value("next");
		if (nextContentList.isNull() == false)
			parseFilesAndFolders(QUrl(nextContentList.toString()), _level + 1);
		else
			finished = true;

	}

	if (finished)
		emit entriesChanged();

	reply->deleteLater();
}

QString FSBMOSF::getRelationshipUrl(QJsonObject nodeObject, QString name)
{
	QJsonObject relationshipsObj = nodeObject.value("relationships").toObject();

	if (relationshipsObj.contains(name) == false)
		return "";

	QJsonObject filesObj = relationshipsObj.value(name).toObject();
	QJsonObject linksObj = filesObj.value("links").toObject();
	QJsonObject relatedObj = linksObj.value("related").toObject();

	return relatedObj.value("href").toString();
}

FSBMOSF::OnlineNodeData FSBMOSF::getNodeData(QString key)
{
	return _pathUrls[key];
}
