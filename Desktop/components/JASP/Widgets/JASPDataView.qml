import QtQuick			2.9
import QtQuick.Window	2.3
import QtQuick.Controls	2.2

import JASP.Controls	1.0
import JASP				1.0

FocusScope
{
	id: __JASPDataViewRoot

				property alias model:					theView.model
				property alias toolTip:					datasetMouseArea.toolTipText
				property alias cursorShape:				datasetMouseArea.cursorShape
				property alias mouseArea:				datasetMouseArea
				property bool  doubleClickWorkaround:	true

				property alias itemDelegate:			theView.itemDelegate
				property alias rowNumberDelegate:		theView.rowNumberDelegate
				property alias columnHeaderDelegate:	theView.columnHeaderDelegate
				property alias leftTopCornerItem:		theView.leftTopCornerItem
				property alias extraColumnItem:			theView.extraColumnItem

				property alias itemHorizontalPadding:	theView.itemHorizontalPadding
				property alias itemVerticalPadding:		theView.itemVerticalPadding
	readonly	property alias rowNumberWidth:			theView.rowNumberWidth

	readonly	property alias contentX:				myFlickable.contentX
	readonly	property alias contentY:				myFlickable.contentY
	readonly	property alias contentWidth:			myFlickable.contentWidth
	readonly	property alias contentHeight:			myFlickable.contentHeight

	readonly	property alias verticalScrollWidth:		vertiScroller.width
	readonly	property alias horizontalScrollHeight:	horiScroller.height
	///Aka without a scrollbar
	readonly	property real  flickableWidth:			myFlickable.width
	///Aka without a scrollbar
	readonly	property real  flickableHeight:			myFlickable.height

				property alias flickableInteractive:	myFlickable.interactive

	JASPMouseAreaToolTipped
	{
		id:					datasetMouseArea
		z:					2
		anchors.fill:		myFlickable
		anchors.leftMargin:	theView.rowNumberWidth
		anchors.topMargin:	theView.headerHeight

		toolTipText:		doubleClickWorkaround ? qsTr("Double click to edit data") : ""

		acceptedButtons:	doubleClickWorkaround ? Qt.LeftButton : Qt.NoButton
		dragging:			myFlickable.dragging
		//hoverEnabled:		!flickableInteractive

		property real	lastTimeClicked:	-1
		property real	doubleClickTime:	400

		onPressed:
		{
			if(!doubleClickWorkaround)
			{
				mouse.accepted = false;
				return;
			}

			var curTime = new Date().getTime()

			if(lastTimeClicked === -1 || curTime - lastTimeClicked > doubleClickTime)
			{
				lastTimeClicked = curTime
				mouse.accepted = false
			}
			else
			{
				lastTimeClicked = -1
				__JASPDataViewRoot.doubleClicked()
			}
		}

		onWheel:
		{
			if(wheel.angleDelta.y == 120)
			{
				if(wheel.modifiers & Qt.ShiftModifier)	horiScroller.scrollUp()
				else									vertiScroller.scrollUp()
			}
			else if(wheel.angleDelta.y == -120)
			{
				if(wheel.modifiers & Qt.ShiftModifier)	horiScroller.scrollDown()
				else									vertiScroller.scrollDown()
			}
			/* Might be needed to have scrolling when flickable is !interactive. But something else seems to be stealing the wheel.
			else if(!flickableInteractive)
			{
				horiScroller.scroll( -wheel.pixelDelta.x);
				vertiScroller.scroll(-wheel.pixelDelta.y);
			}*/
			else
				wheel.accepted = false;
		}


	}

	signal doubleClicked()

	Flickable
	{
		id:				myFlickable
		z:				-1
		clip:			true

		anchors.top:	parent.top
		anchors.left:	parent.left
		anchors.right:	vertiScroller.left
		anchors.bottom: horiScroller.top

		contentWidth:	theView.width
		contentHeight:	theView.height

		DataSetView
		{
			z:			-1
			id:			theView
			model:		null

			viewportX:	myFlickable.visibleArea.xPosition   * width
			viewportY:	myFlickable.visibleArea.yPosition   * height
			viewportW:	myFlickable.visibleArea.widthRatio  * width
			viewportH:	myFlickable.visibleArea.heightRatio * height
		}
	}


	JASPScrollBar
	{
		id:				vertiScroller;
		flickable:		myFlickable
		anchors.top:	parent.top
		anchors.right:	parent.right
		anchors.bottom: horiScroller.top
		bigBar:			true
	}

	JASPScrollBar
	{
		id:				horiScroller;
		flickable:		myFlickable
		vertical:		false
		anchors.left:	parent.left
		anchors.right:	vertiScroller.left
		anchors.bottom: parent.bottom
		bigBar:			true
	}
}
