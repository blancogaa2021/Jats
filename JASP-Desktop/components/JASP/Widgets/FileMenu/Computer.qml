import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import JASP.Theme 1.0
import JASP.Widgets 1.0

Item
{
	id:						rect
	focus:					true
	onActiveFocusChanged:	if(activeFocus) computerList.forceActiveFocus()

	MenuHeader
	{
		id:					menuHeader
		headertext:			qsTr("Recent Folders")
		toolseparator:		false
	}

	RectangularButton
	{
		id:					browseButton
		text:				qsTr("Browse")
		anchors.left:		menuHeader.left
		anchors.top:		menuHeader.bottom
		onClicked:			fileMenuModel.computer.browseMostRecent();
		KeyNavigation.down:	computerList
		KeyNavigation.tab:	computerList
	}

	ToolSeparator
	{
		id:					firstSeparator

		anchors.top:		browseButton.bottom
		anchors.left:		menuHeader.left
		anchors.right:		menuHeader.right
		anchors.topMargin:	8 * preferencesModel.uiScale
		width:				rect.width
		orientation:		Qt.Horizontal
	}

	FileList
	{
		id:					computerList
		cppModel:			fileMenuModel.computer.listModel


		anchors
		{
			top:			firstSeparator.bottom
			left:			menuHeader.left
			right:			menuHeader.right
			bottom:			parent.bottom
			topMargin:		Theme.generalMenuMargin
			bottomMargin:	Theme.generalMenuMargin
		}

	}

}
