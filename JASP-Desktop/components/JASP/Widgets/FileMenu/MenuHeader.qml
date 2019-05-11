import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import JASP.Theme 1.0
import JASP.Widgets 1.0

Item
{

	property bool	toolseparator:	true
	property string headertext:		""
	property string helpfile:		""

	function showHelp()
	{
		if(helpfile !== "")
			helpModel.showOrTogglePage(helpfile);
	}

	height:				Theme.menuHeaderHeight
	anchors
	{
		top:			parent.top
		left:			parent.left
		right:			parent.right
		leftMargin:		Theme.generalMenuMargin
		rightMargin:	Theme.generalMenuMargin
		topMargin:		Theme.generalMenuMargin
	}

	Label
	{
		id:				headLabel
		text:			headertext

		anchors.fill:	parent
		font:			Theme.fontGroupTitle
		color:			Theme.black
	}

	MenuButton
	{
		id:						idHelpButton
		visible:				helpfile !== ""
		height:					headLabel.height
		width:					height
		iconSource:				"qrc:/images/info-button.png"  // {info-button, info-button-grey}.png Icons made by Freepik from https://www.flaticon.com/
		onClicked:				helpModel.showOrTogglePage(helpfile)
		toolTip:				"Show info about these preferences"
		radius:					height / 2
		anchors.verticalCenter: headLabel.verticalCenter
		anchors.right:			parent.right
	}

	ToolSeparator
	{
		id:				firstSeparator

		anchors.top:	headLabel.bottom
		width:			parent.width

		orientation:	Qt.Horizontal
		visible:		toolseparator
	}

}
