import QtQuick			2.13
import QtQuick.Controls 2.13
import JASP.Theme		1.0
import "FilterConstructor"

FocusScope
{
	id:							filterContainer
	visible:					opened

	property bool	opened:					false
	property int	minimumHeightTextBoxes: 50 * preferencesModel.uiScale
	property bool	showEasyFilter:			true
	property int	desiredMinimumHeight:	showEasyFilter ? easyFilterConstructor.desiredMinimumHeight : rFilterFields.desiredMinimumHeight

	onShowEasyFilterChanged:	if(!showEasyFilter) absorbModelRFilter()
	onVisibleChanged:			if(!visible) filterWindow.close()

    function toggle()
    {
        opened = !opened
		absorbModelRFilter()

		if(opened)
			height = parent.height / 2
    }

	function open()
	{
		if(!opened)
			toggle();
	}

	function close()
	{
		if(opened)
			toggle();
	}


    function applyAndSendFilter(newFilter)
    {
		filterModel.rFilter = newFilter //Triggers send in FilterModel
		absorbModelRFilter()
    }

	function resetFilter()
	{
		filterModel.resetRFilter()
		absorbModelRFilter()
	}

	function absorbModelRFilter()
	{
		filterEdit.text = filterModel.rFilter
	}

	signal rCodeChanged(string rScript)

	Rectangle
	{
		anchors.fill:	parent
		color:			Theme.uiBackground
		border.color:	Theme.uiBorder
		border.width:	1
	}

	Item
	{
		anchors.fill:		parent
		anchors.margins:	1
		visible:			parent.showEasyFilter


		FilterConstructor
		{
			id:				easyFilterConstructor
			onRCodeChanged: filterContainer.rCodeChanged(rScript)
			clip:			true

			anchors
			{
				bottom:	closeEasyRectangularButton.top
				right:	parent.right
				left:	parent.left
				top:	parent.top
			}


			functionModel: ListModel
			{

				ListElement	{ type: "function";	functionName: "abs";	functionParameters: "values";			functionParamTypes: "number";						toolTip: "absolute value" }
				ListElement	{ type: "function";	functionName: "sd";		functionParameters: "values";			functionParamTypes: "number";						toolTip: "standard deviation" }
				ListElement	{ type: "function";	functionName: "var";	functionParameters: "values";			functionParamTypes: "number";						toolTip: "variance" }
				ListElement	{ type: "function";	functionName: "sum";	functionParameters: "values";			functionParamTypes: "number";						toolTip: "summation" }
				ListElement	{ type: "function";	functionName: "prod";	functionParameters: "values";			functionParamTypes: "number";						toolTip: "product of values" }

				ListElement	{ type: "function";	functionName: "min";	functionParameters: "values";			functionParamTypes: "number";							toolTip: "returns minimum of values" }
				ListElement	{ type: "function";	functionName: "max";	functionParameters: "values";			functionParamTypes: "number";								toolTip: "returns maximum of values" }
				ListElement	{ type: "function";	functionName: "mean";	functionParameters: "values";			functionParamTypes: "number";									toolTip: "mean" }
				ListElement	{ type: "function";	functionName: "round";	functionParameters: "y,n";				functionParamTypes: "number,number";								toolTip: "rounds y to n decimals" }
				ListElement	{ type: "function";	functionName: "length";	functionParameters: "y";				functionParamTypes: "string:number";									toolTip: "returns number of elements in y" }
				ListElement	{ type: "function";	functionName: "median";	functionParameters: "values";			functionParamTypes: "number";												toolTip: "median" }
				ListElement	{ type: "function";	functionName: "ifelse";	functionParameters: "test,then,else";	functionParamTypes: "boolean,boolean:string:number,boolean:string:number";		toolTip: "if-else statement" }
			}

			function askIfChanged(closeFunc)
			{
				if(jsonChanged() || !lastCheckPassed)
				{
					easySaveDialog.closeFunc = closeFunc
					easySaveDialog.open()
				}
				else
					closeFunc()
			}

			SaveDiscardCancelDialog
			{
				id:		easySaveDialog

				title:	"Filter Changed"
				text:	qsTr("There are unapplied changes to your filter; what would you like to do?")

                property var closeFunc: undefined

				onSave:		if(easyFilterConstructor.checkAndApplyFilter()) closeFunc();
				onDiscard:	closeFunc();
			}
		}

		RectangularButton
		{
			id:			rRectangularButton
			iconSource: "qrc:/icons/R.png"
			onClicked:	easyFilterConstructor.askIfChanged(function() { filterContainer.showEasyFilter = false } )
			width:		height
			toolTip:	qsTr("Switch to the R filter")
			anchors
			{
				left:	parent.left
				bottom:	parent.bottom
				top:	closeEasyRectangularButton.top
			}
		}

		RectangularButton
		{
			id:			showInactiveFilteredButtonEasy
			iconSource: dataSetModel.showInactive ? "qrc:/icons/eyeOpen.png" : "qrc:/icons/eyeClosed.png"
			onClicked:	dataSetModel.showInactive = !dataSetModel.showInactive
			width:		height
			toolTip:	(dataSetModel.showInactive ? qsTr("Hide rows that were filtered out.") : qsTr("Show rows that were filtered out."))
			anchors
			{
				left:	rRectangularButton.right
				bottom:	parent.bottom
				top:	closeEasyRectangularButton.top
			}
		}

		RectangularButton
		{
			property bool showApplyNotApplied: easyFilterConstructor.somethingChanged || easyFilterConstructor.showStartupMsg

			id:				applyEasyFilter
			text:			showApplyNotApplied ? qsTr("Apply pass-through filter") : qsTr("Filter applied")
			enabled:		easyFilterConstructor.somethingChanged
			onClicked:		easyFilterConstructor.checkAndApplyFilter()
			toolTip:		showApplyNotApplied ? qsTr("Click to apply filter") : qsTr("Filter is already applied")
			anchors
			{
				left:	showInactiveFilteredButtonEasy.right
				right:	helpEasyRectangularButton.left
				bottom: parent.bottom
				top:	closeEasyRectangularButton.top
			}

		}

		RectangularButton
		{
			id:				helpEasyRectangularButton
			iconSource:		"qrc:/images/info-button.png"

			onClicked:		helpModel.showOrTogglePage("other/EasyFilterConstructor");
			toolTip:		qsTr("Open Documentation")
			anchors
			{
				right:	closeEasyRectangularButton.left
				bottom:	parent.bottom
				top:	closeEasyRectangularButton.top
			}
		}

		RectangularButton
		{
			id:				closeEasyRectangularButton
			iconSource:		"qrc:/images/cross.png"
			anchors.right:	parent.right
			anchors.bottom: parent.bottom

			onClicked:		easyFilterConstructor.askIfChanged(function() { filterWindow.toggle() } )
			toolTip:		qsTr("Hide filter")
		}
	}

	Item
    {
						id:						rFilterFields
						visible:				!parent.showEasyFilter
						anchors.fill:			parent
						anchors.margins:		1
		property real	desiredMinimumHeight:	filterButtons.height + (filterErrorScroll.visible ? filterErrorScroll.height : 0 ) + filterEditRectangle.desiredMinimumHeight

		Rectangle
		{
							id:						filterEditRectangle
							color:					Theme.white
							border.width:			1
							border.color:			"lightGrey"
			property real	desiredMinimumHeight:	applyFilter.height + filterWindow.minimumHeightTextBoxes + filterGeneratedBox.contentHeight

			anchors
			{
				top:	parent.top
				bottom:	filterErrorScroll.top
				left:	parent.left
				right:	parent.right
			}

			Image
			{
				id:							backgroundImage

				source:						"qrc:/icons/filterConstructorBackground.png"
				anchors.centerIn:			parent

				property real widthScale:	parent.width  / implicitWidth
				property real heightScale:	parent.height / implicitHeight
				property real ratio:		Math.min(Math.min(widthScale, heightScale), 1.0) * 0.5

				width:						implicitWidth * ratio
				height:						implicitHeight * ratio
			}

			ScrollView
			{
				id:				filterScroller
				anchors.fill:	parent
				clip:			true
				contentWidth:	width
				contentHeight:	filterGeneratedBox.height + filterEditBox.height

				Rectangle
				{
					id:				filterGeneratedBox
					height:			filterGeneratedEdit.contentHeight
					color:			"transparent"
					border.color:	"lightGray"
					border.width:	1

					anchors
					{
						top:	parent.top
						left:	parent.left
						right:	parent.right
					}


					TextArea
					{
						id:						filterGeneratedEdit
						anchors.top:			filterGeneratedBox.top
						anchors.left:			resetAllGeneratedFilters.right
						anchors.right:			filterGeneratedBox.right
						text:					filterModel.generatedFilter +"\n"
						height:					contentHeight
						readOnly:				true
						color:					"gray"
						selectByMouse:			true
						onActiveFocusChanged:	if(!activeFocus) deselect()

						font.family:			"Courier"
						font.pixelSize:			baseFontSize * preferencesModel.uiScale
						wrapMode:				TextArea.WrapAtWordBoundaryOrAnywhere

					}

					RectangularButton
					{
						id:						resetAllGeneratedFilters
						width:					dataSetModel.columnsFilteredCount > 0 ? height : 0
						height:					filterGeneratedBox.height
						iconSource:				"qrc:/images/eraser_all.png"
						visible:				dataSetModel.columnsFilteredCount > 0
						toolTip:				qsTr("Reset all checkmarks on all labels")
						onClicked:				dataSetModel.resetAllFilters()

						anchors.left:			parent.left
						anchors.verticalCenter:	parent.verticalCenter
						anchors.margins:		1

					}
				}

				Item
				{
					//Must be here because otherwise filterEdit turns its clipping on, because it is in a scrollview...
					id:				filterEditBox
					height:			filterEdit.height
					anchors.top:	filterGeneratedBox.bottom
					anchors.left:	parent.left
					anchors.right:	parent.right

					TextArea
					{

						id:						filterEdit
						height:					contentHeight + 30
						selectByMouse:			true
						onActiveFocusChanged:	if(!activeFocus) deselect()
						font.family:			"Courier"
						font.pixelSize:			baseFontSize * preferencesModel.uiScale
						wrapMode:				TextArea.WrapAtWordBoundaryOrAnywhere

						property bool changedSinceLastApply: text !== filterModel.rFilter


						anchors
						{
							top:	parent.top
							left:	parent.left
							right:	parent.right
						}
					}
				}
			}

			function askIfChanged(closeFunc)
			{
				if(filterEdit.changedSinceLastApply)
				{
					saveDialog.closeFunc = closeFunc
					saveDialog.open()
				}
				else
					closeFunc()
			}

			SaveDiscardCancelDialog
			{
				id:		saveDialog

				title:	"Filter Changed"
				text:	qsTr("There are unapplied changes to your filter; what would you like to do?")

				property var closeFunc: undefined

				onSave:		{ filterWindow.applyAndSendFilter(filterEdit.text); closeFunc(); }
				onDiscard:	closeFunc();
			}
        }

		ScrollView
		{
			id:				filterErrorScroll
			height:			filterWindow.minimumHeightTextBoxes

			anchors
			{
				left:	parent.left
				right:	parent.right
				bottom: filterButtons.top
			}

			TextArea
			{
				id:						filterError
				color:					"red"
				readOnly:				true
				text:					filterModel.filterErrorMsg + "\n"
				selectByMouse:			true
				onActiveFocusChanged:	if(!activeFocus) deselect()
				font.family:			"Courier"
				font.pixelSize:			baseFontSize * preferencesModel.uiScale

				states:
				[
					State
					{
						name: "closed"
						when: filterModel.filterErrorMsg.length === 0
						PropertyChanges { target: filterErrorScroll; visible: false; height: 0 }
					},
					State
					{
						name: "opened"
						when: filterModel.filterErrorMsg.length > 0
						PropertyChanges { target: filterErrorScroll; visible: true; height: filterError.contentHeight} //Math.min( , filterWindow.minimumHeightTextBoxes)
					}
				]
			}
		}

		Item
		{
			id:				filterButtons
			height:			closeRectangularButton.height
			anchors
			{
				left:		parent.left
				right:		parent.right
				bottom:		parent.bottom
			}

			RectangularButton
			{
				id:				easyRectangularButton
				iconSource:		"qrc:/icons/NotR.png"
				onClicked:		filterEditRectangle.askIfChanged(function (){ filterContainer.showEasyFilter = true })
				width:			visible ? height : 0
				toolTip:		qsTr("Switch to the drag and drop filter")
				anchors
				{
					left:		parent.left
					bottom:		parent.bottom
					top:		closeRectangularButton.top
				}
			}

			RectangularButton
			{
				id:			showInactiveFilteredButtonR
				iconSource: dataSetModel.showInactive ? "qrc:/icons/eyeOpen.png" : "qrc:/icons/eyeClosed.png"
				onClicked:	dataSetModel.showInactive = !dataSetModel.showInactive
				width:		height
				toolTip:	(dataSetModel.showInactive ? qsTr("Hide rows that were filtered out.") : qsTr("Show rows that were filtered out."))
				anchors
				{
					left:	easyRectangularButton.right
					bottom:	parent.bottom
					top:	easyRectangularButton.top
				}
			}

			RectangularButton
			{
				id:			clearRectangularButton
				iconSource: "qrc:/images/eraser.png"
				onClicked:	filterWindow.resetFilter()
				width:		visible ? implicitWidth : 0
				height:		filterContainer.buttonsHeight
				visible:	filterEdit.text !== filterModel.defaultRFilter
				toolTip:	qsTr("Reset to default filter")

				anchors
				{
					left:	showInactiveFilteredButtonR.right
					bottom: parent.bottom
					top:	closeRectangularButton.top
				}
			}

			RectangularButton
			{
				id: applyFilter

				property bool filterIsDefault: filterEdit.text === filterModel.defaultRFilter

				text:			filterEdit.changedSinceLastApply ? qsTr("Apply pass-through filter") : filterIsDefault ? qsTr("Default filter applied") : qsTr("Filter applied")
				enabled:		filterEdit.changedSinceLastApply
				anchors.left:	clearRectangularButton.right
				anchors.right:	helpButton.left
				anchors.bottom:	parent.bottom
				anchors.top:	closeRectangularButton.top

				onClicked:		filterWindow.applyAndSendFilter(filterEdit.text)

				toolTip:		filterEdit.changedSinceLastApply ? qsTr("Click to apply filter") : filterIsDefault ? qsTr("Filter is unchanged from default") : qsTr("Filter is already applied")
			}

			RectangularButton
			{
				id:				helpButton
				iconSource:		"qrc:/images/info-button.png"
				anchors.right:	closeRectangularButton.left
				anchors.bottom: parent.bottom
				anchors.top:	closeRectangularButton.top

				onClicked:		helpModel.showOrTogglePage("other/RFilterConstructor");
				toolTip:		qsTr("Open Documentation")
			}


			RectangularButton
			{
				id:				closeRectangularButton
				iconSource:		"qrc:/images/cross.png"
				anchors.right:	parent.right
				anchors.bottom: parent.bottom

				onClicked:		filterEditRectangle.askIfChanged(function (){ filterWindow.toggle() })
				toolTip:		qsTr("Hide filter")
			}
		}
    }

}
