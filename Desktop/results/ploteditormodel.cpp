#include "ploteditormodel.h"
#include "analysis/analyses.h"
#include "utilities/qutils.h"
#include "log.h"
#include "tempfiles.h"
#include <QDir>

namespace PlotEditor
{

PlotEditorModel::PlotEditorModel()
	: QObject(Analyses::analyses())
{
	_xAxis = new AxisModel(this, true);
	_yAxis = new AxisModel(this, false);

	connect(_xAxis, &AxisModel::somethingChanged, this, &PlotEditorModel::somethingChanged);
	connect(_yAxis, &AxisModel::somethingChanged, this, &PlotEditorModel::somethingChanged);
}

void PlotEditorModel::showPlotEditor(int id, QString options)
{
	Analysis* analysis = Analyses::analyses()->get(id);
	if (_analysis != analysis)
	{
		if (_analysis)	disconnect(_analysis, &Analysis::destructionSignal, this, &PlotEditorModel::reset);
		if (analysis)	connect(_analysis, &Analysis::destructionSignal, this, &PlotEditorModel::reset);
	}

	_analysisId	= id;
	_analysis = analysis;
	_imgOptions	= Json::objectValue;

	Json::Reader().parse(fq(options), _imgOptions);

	//maybe the following checks are a bit extreme but whatever
	if(!_analysis || !_imgOptions.isMember("type") || _imgOptions["type"].type() != Json::stringValue || _imgOptions["type"] != "interactive")
		return;

	processImgOptions();

	setVisible(true);
}

void PlotEditorModel::reset()
{
	_analysisId		=	-1;

	_analysis		=	nullptr;
	_imgOptions		=	Json::nullValue;
	_editOptions	=	Json::nullValue;
	setName(			"");
	setData(			"");
	setTitle(			"");
	setWidth(			100);
	setHeight(			100);
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

	_coordinates.loadCoordinates(_editOptions.get("coordinates", Json::objectValue)); // To Do Vincent Pedata: is this the right json object?

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

	// To Do Vincent Pedata: Do we need to send the coordinates back? No right?

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

	if(!_visible)
		reset();
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
	if(!_analysis)
		return;

	//Lets make sure the plot gets reloaded by QML
	_goBlank = true;
	emit dataChanged();
	_goBlank = false;
	emit dataChanged();

	_analysis->setEditOptionsOfPlot(_name.toStdString(), generateEditOptions());
}

QUrl PlotEditorModel::imgFile() const
{
	if(!_analysis || _goBlank)
		return QUrl("");

	QString pad(tq(TempFiles::sessionDirName()) + "/" + _data);
		
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

QString PlotEditorModel::clickHitsElement(double x, double y) const
{
	std::string elementName;
	return 	_coordinates.elementHit(x, y, elementName) ? tq(elementName) : "";
}

}
