import QtQuick			2.11
import QtQuick.Controls 2.4
import JASP.Widgets		1.0
import JASP.Theme		1.0
import JASP.Controls	1.0


Item
{
	focus:					true
	onActiveFocusChanged:	if(activeFocus) synchronizeDataSave.forceActiveFocus();

	MenuHeader
	{
		id:			menuHeader
		headertext:	"Data Preferences"
		helpfile:	"preferences/prefsdata"
	}

	ScrollView
	{
		id:					scrollPrefs
		anchors.top:		menuHeader.bottom
		anchors.left:		menuHeader.left
		anchors.right:		menuHeader.right
		anchors.bottom:		menuHeader.bottom
		anchors.topMargin:	Theme.generalMenuMargin
		focus:				true
		Keys.onLeftPressed: resourceMenu.forceActiveFocus();
		focusPolicy:		Qt.WheelFocus

		PrefsGroupRect
		{
			spacing:		Theme.rowSpacing
			implicitWidth:	scrollPrefs.width - (Theme.generalAnchorMargin * 2)

			CheckBox  //Synchronize automatically
			{
				id:					synchronizeDataSave
				label:				qsTr("Synchronize automatically on data file save")
				checked:			preferencesModel.dataAutoSynchronization
				onCheckedChanged:	preferencesModel.dataAutoSynchronization = checked
				KeyNavigation.down:	useDefaultEditor
				KeyNavigation.tab:	useDefaultEditor
			}

			Item //Use default spreadsheet editor
			{
				height:	useDefaultEditor.height + (editCustomEditor.visible ? editCustomEditor.height : 0)
				width:	parent.width - Theme.generalAnchorMargin

				CheckBox
				{
					id:					useDefaultEditor
					label:				"Use default spreadsheet editor"
					checked:			preferencesModel.useDefaultEditor
					onCheckedChanged:	preferencesModel.useDefaultEditor = checked
					KeyNavigation.down:	browseEditorButton
					KeyNavigation.tab:	browseEditorButton
				}

				Item
				{
					id:					editCustomEditor
					visible:			!preferencesModel.useDefaultEditor
					width:				parent.width
					height:				browseEditorButton.height
					anchors.top:		useDefaultEditor.bottom


					RectangularButton
					{
						id:					browseEditorButton
						text:				"Select custom editor"
						onClicked:			preferencesModel.browseSpreadsheetEditor()
						anchors.left:		parent.left
						anchors.leftMargin: Theme.subOptionOffset
						KeyNavigation.down:	customEditorText
						KeyNavigation.tab:	customEditorText
					}

					Rectangle
					{
						anchors
						{
							left:			browseEditorButton.right
							right:			parent.right
							top:			parent.top
							bottom:			parent.bottom
						}

						height:				browseEditorButton.height
						color:				Theme.white
						border.color:		Theme.buttonBorderColor
						border.width:		1

						TextInput
						{
							id:					customEditorText
							text:				preferencesModel.customEditor
							clip:				true
							font:				Theme.font
							onTextChanged:		preferencesModel.customEditor = text
							color:				Theme.textEnabled
							KeyNavigation.down:	customThreshold
							KeyNavigation.tab:	customThreshold

							anchors
							{
								left:			parent.left
								right:			parent.right
								verticalCenter:	parent.verticalCenter
								margins:		Theme.generalAnchorMargin
							}

							Connections
							{
								target:					preferencesModel
								onCustomEditorChanged:	customEditorText = preferencesModel.customEditor
							}

						}
					}
				}
			}


			Item  //Scale threshold
			{
				height:		customThreshold.height
				width:		customThreshold.width + thresholdScale.width

				CheckBox
				{
					id:					customThreshold
					label:				qsTr("Import threshold between Nominal or Scale")
					checked:			preferencesModel.customThresholdScale
					onCheckedChanged:	preferencesModel.customThresholdScale = checked
					ToolTip.delay:		500
					ToolTip.timeout:	6000 //Some longer to read carefully
					toolTip:			qsTr("Threshold number of unique integers before classifying a variable as 'scale'.\nYou need to reload your data to take effect! Check help for more info.")
					KeyNavigation.down:	thresholdScale
					KeyNavigation.tab:	thresholdScale

				}

				SpinBox
				{
					id:					thresholdScale
					value:				preferencesModel.thresholdScale
					onValueChanged:		preferencesModel.thresholdScale = value
					visible:			preferencesModel.customThresholdScale

					KeyNavigation.down:	missingFileList.firstComponent
					KeyNavigation.tab:	missingFileList.firstComponent
					anchors
					{
						left:			customThreshold.right
						leftMargin:		Theme.generalAnchorMargin
						verticalCenter:	parent.verticalCenter
					}
				}
			}

			PrefsMissingValues { id: missingFileList }
		}
	}
}
