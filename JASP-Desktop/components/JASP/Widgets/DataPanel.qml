import QtQuick			2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts	1.0
import JASP.Theme		1.0

Rectangle
{
	id:				rootDataset
	color:			Theme.uiBackground

	SplitView
    {
		id:				splitViewData
		anchors.fill:	parent
		orientation:	Qt.Vertical
		handle:			Rectangle
		{
			implicitHeight: Theme.splitHandleWidth * 0.8;
			color:			Theme.uiBackground;

			Item
			{
				id:							threeDots
				width:						height * 4
				height:						Theme.splitHandleWidth * 0.3
				anchors.centerIn:			parent
				property color	kleur:		Theme.grayDarker

				Rectangle
				{
					color:		threeDots.kleur
					width:		height
					radius:		width

					anchors
					{
						top:	parent.top
						left:	parent.left
						bottom:	parent.bottom
					}
				}

				Rectangle
				{
					color:		threeDots.kleur
					width:		height
					radius:		width
					anchors
					{
						top:				parent.top
						bottom:				parent.bottom
						horizontalCenter:	parent.horizontalCenter
					}
				}

				Rectangle
				{
					color:		threeDots.kleur
					width:		height
					radius:		width

					anchors
					{
						top:	parent.top
						right:	parent.right
						bottom:	parent.bottom
					}
				}
			}
		}

		FilterWindow
		{
			id:							filterWindow
			objectName:					"filterWindow"
			SplitView.minimumHeight:	desiredMinimumHeight
			SplitView.preferredHeight:	rootDataset.height * 0.25
			SplitView.maximumHeight:	rootDataset.height * 0.8

		}

		ComputeColumnWindow
		{
			id:							computeColumnWindow
			objectName:					"computeColumnWindow"
			SplitView.minimumHeight:	desiredMinimumHeight
			SplitView.preferredHeight:	rootDataset.height * 0.25
			SplitView.maximumHeight:	rootDataset.height * 0.8
		}

        VariablesWindow
        {
			id:							variablesWindow
			SplitView.minimumHeight:	calculatedMinimumHeight
			SplitView.preferredHeight:	rootDataset.height * 0.25
			SplitView.maximumHeight:	rootDataset.height * 0.8
        }

		DataTableView
		{
			objectName:				"dataSetTableView"
			SplitView.fillHeight:	true
			onDoubleClicked:		mainWindow.startDataEditorHandler()
        }
	}
}
