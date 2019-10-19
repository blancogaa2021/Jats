#include "ploteditormodel.h"
#include "analysis/analyses.h"
#include "utilities/qutils.h"
#include "log.h"
#include "tempfiles.h"
#include <QDir>

PlotEditorModel::PlotEditorModel(Analyses * analyses)
	: QObject(analyses), _analyses(analyses)
{
	_xAxis = new PlotEditorAxisModel(this, true);
	_yAxis = new PlotEditorAxisModel(this, false);

	connect(_xAxis, &PlotEditorAxisModel::somethingChanged, this, &PlotEditorModel::somethingChanged);
	connect(_yAxis, &PlotEditorAxisModel::somethingChanged, this, &PlotEditorModel::somethingChanged);
}

void PlotEditorModel::showPlotEditor(int id, QString options)
{
	setVisible(false);

	_analysisId	= id;
	_analysis	= _analyses->get(id);
	_imgOptions	= Json::objectValue;

	Json::Reader().parse(fq(options), _imgOptions);

	//maybe the following checks are a bit extreme but whatever
	if(!_analysis || !_imgOptions.isMember("type") || _imgOptions["type"].type() != Json::stringValue || _imgOptions["type"] != "interactive")
		return;

	processImgOptions();

	setVisible(true);
}

void PlotEditorModel::processImgOptions()
{
	setName(	tq(			_imgOptions.get(	"name",			"").asString()));
	setData(	tq(			_imgOptions.get(	"data",			"").asString()));
	setTitle(	tq(			_imgOptions.get(	"title",		"").asString()));
	setWidth(				_imgOptions.get(	"width",		100).asInt());
	setHeight(				_imgOptions.get(	"height",		100).asInt());

	//_editOptions		=	_imgOptions.get(	"editOptions",	Json::objectValue);
	_editOptions		=	_name == "" || !_analysis ? Json::objectValue : _analysis->editOptionsOfPlot(_name.toStdString());
	Json::Value	xAxis	=	_editOptions.get(	"xAxis",		Json::objectValue),
				yAxis	=	_editOptions.get(	"yAxis",		Json::objectValue);

	_xAxis->setAxisData(xAxis);
	_yAxis->setAxisData(yAxis);

}

Json::Value PlotEditorModel::generateImgOptions() const
{
	Json::Value		imgOptions	= _imgOptions;

	imgOptions["editOptions"]	= generateEditOptions();

	imgOptions["name"]			= name().toStdString();
	imgOptions["data"]			= data().toStdString();
	imgOptions["title"]			= title().toStdString();
	imgOptions["width"]			= width();
	imgOptions["height"]		= height();

	return imgOptions;
}

Json::Value PlotEditorModel::generateEditOptions() const
{
	Json::Value		editOptions = _editOptions;

	editOptions["xAxis"]		= _xAxis->getAxisData();
	editOptions["yAxis"]		= _yAxis->getAxisData();

	return editOptions;
}

void PlotEditorModel::somethingChanged() const
{
	if(!_visible) return; // We're still loading!

	_analysis->editImage(generateImgOptions());

}

void PlotEditorModel::setVisible(bool visible)
{
	if (_visible == visible)
		return;

	_visible = visible;
	emit visibleChanged(_visible);
}

void PlotEditorModel::setName(QString name)
{
	if (_name == name)
		return;

	_name = name;
	emit nameChanged(_name);
	somethingChanged();
}

void PlotEditorModel::refresh()
{
	//Lets make sure the plot gets reloaded by QML
	_goBlank = true;
	emit dataChanged();
	_goBlank = false;
	emit dataChanged();

	_analysis->setEditOptionsOfPlot(_name.toStdString(), generateEditOptions());
}

QUrl PlotEditorModel::imgFile() const
{
	if(_goBlank) return QUrl("");

	QString pad(tq(TempFiles::sessionDirName()) + "/" + _data);
	
	std::cout << "Pad='" << pad.toStdString() << "'" << std::endl;
	
	return QUrl::fromLocalFile(pad);
}

void PlotEditorModel::setData(QString data)
{
	if (_data == data)
		return;

	_data = data;
	emit dataChanged();
	somethingChanged();
}

void PlotEditorModel::setTitle(QString title)
{
	if (_title == title)
		return;

	_title = title;
	emit titleChanged(_title);
	somethingChanged();
}

void PlotEditorModel::setWidth(int width)
{
	if (_width == width)
		return;

	_width = width;
	emit widthChanged(_width);
}

void PlotEditorModel::setHeight(int height)
{
	if (_height == height)
		return;

	_height = height;
	emit heightChanged(_height);
}
