import QtQuick			2.11
import QtQuick.Controls	2.4
import JASP.Theme		1.0

Rectangle
{
	color:			Theme.uiBackground
	border.color:	Theme.uiBorder
	border.width:	1
	height:			120 * preferencesModel.uiScale
	radius:			height
	width:			parent.width * 0.5

    Text
    {
		id:						loadingText
		horizontalAlignment:	Text.AlignHCenter
		text:					mainWindow.progressBarStatus
		font:					Theme.fontLabel

		anchors
		{
			top:		parent.top
			left:		parent.left
			right:		parent.right
			topMargin:	16 * preferencesModel.uiScale
		}
    }

    ProgressBar
    {
		id:				loadingBar
		from:			0
		to:				100
		value:			mainWindow.progressBarProgress

		anchors
		{
			verticalCenter:	parent.verticalCenter
			left:			progressBarHolder.left
			right:			progressBarHolder.right
			margins:		30 * preferencesModel.uiScale
		}

		background:
			Rectangle
			{
				//y:				3 * preferencesModel.uiScale
				implicitWidth:	200 * preferencesModel.uiScale
				height:			18 * preferencesModel.uiScale
				radius:			height
				color:			Theme.grayLighter
			}


		contentItem: Item
		{
			implicitWidth:	200 * preferencesModel.uiScale
			implicitHeight: 14  * preferencesModel.uiScale

			Rectangle
			{
				y:		2 * preferencesModel.uiScale
				x:		2 * preferencesModel.uiScale
				width:	loadingBar.visualPosition * (parent.width - (x * 2))
				height: parent.height
				radius: height
				color:	Theme.blue
			}
		}
    }
}
