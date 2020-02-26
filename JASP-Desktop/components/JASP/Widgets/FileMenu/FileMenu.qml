import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import JASP.Widgets 1.0
import FileOperation 1.0

FocusScope
{
	id: fileMenu

	Keys.onEscapePressed:	fileMenuModel.visible = false;
	Keys.onDownPressed:		fileMenuModel.actionButtons.selectButtonDown();
	Keys.onUpPressed:		fileMenuModel.actionButtons.selectButtonUp();


	function showToolSeperator(typeRole)
	{
		return typeRole === FileOperation.Close || typeRole === FileOperation.Preferences || typeRole === FileOperation.About
	}

	width:		slidePart.width
	height:		600
	z:			1
	visible:	fileMenuAnimation.running ? actionMenu.x + actionMenu.width > 0 : fileMenuModel.visible

	property int  actionButtionHeight:		35 * preferencesModel.uiScale
	property int  resourceButtonHeight:		1.5 * actionButtionHeight
	property int  nbColumns:				1 + (resourceRepeaterId.count > 0 ? 1 : 0 )
	property int  colWidths:				190


	Connections
	{
		target:				fileMenuModel
		onVisibleChanged:	if(fileMenuModel.visible) actionMenu.forceActiveFocus(); else fileMenu.focus = false;
	}

	Item
	{
		id:		slidePart

		property real desiredX: !fileMenuModel.visible ? -(resourceScreen.otherColumnsWidth + resourceScreen.width) : 0

		x:		desiredX
		width:	(fileMenu.nbColumns * fileMenu.colWidths * preferencesModel.uiScale) + (resourceScreen.aButtonVisible ? resourceScreen.width : 0)
		height: fileMenu.height

		Behavior on x
		{
			enabled:		!preferencesModel.safeGraphics

			PropertyAnimation
			{
				id:				fileMenuAnimation
				duration:		jaspTheme.fileMenuSlideDuration
				easing.type:	Easing.OutCubic
			}
		}

		FocusScope
		{
			id:				actionMenu
			anchors.left:	parent.left
			width:			fileMenu.colWidths * preferencesModel.uiScale
			height:			parent.height
			z:				2

			Rectangle
			{
				z:				-1
				color:			jaspTheme.fileMenuColorBackground
				border.width:	1
				border.color:	jaspTheme.uiBorder
				anchors.fill:	parent
			}

			Column
			{
				id: fileAction
				spacing: 4
				width: parent.width - jaspTheme.generalAnchorMargin

				anchors
				{
					top: parent.top
					topMargin: 5
					horizontalCenter: parent.horizontalCenter
				}

				Repeater
				{
					id:		actionRepeaterId
					model:	fileMenuModel.actionButtons

					Item
					{
						id:				itemActionMenu
						width:			parent.width - (6 * preferencesModel.uiScale)
						anchors.left:	parent.left
						height:			actionButtionHeight + actionToolSeperator.height
						enabled:		enabledRole

						MenuButton
						{
							id:					actionMenuButton
							hasSubMenu:			hasSubMenuRole
							width:				itemActionMenu.width
							height:				actionButtionHeight
							text:				nameRole
							selected:			selectedRole
							focus:				selectedRole

							anchors
							{
								leftMargin: 3  * preferencesModel.uiScale
								left:		itemActionMenu.left
							}

							Keys.onLeftPressed:		fileMenuModel.visible = false;
							Keys.onRightPressed:	if(hasSubMenuRole) resourceMenu.forceActiveFocus()
							Keys.onDownPressed:		fileMenuModel.actionButtons.selectButtonDown()
							Keys.onUpPressed:		fileMenuModel.actionButtons.selectButtonUp()
							onHoverClicked:
							{
								actionMenuButton.forceActiveFocus();
								fileMenuModel.actionButtons.buttonClicked(typeRole);
							}

							onClicked:
							{
								hoverClicked();
								if(hasSubMenuRole)
									resourceMenu.forceActiveFocus()
							}
						}

						ToolSeparator
						{
							id:					actionToolSeperator
							anchors.top:		actionMenuButton.bottom
							width:				actionMenuButton.width
							anchors.topMargin:	(showToolSeperator(typeRole) ? 3 : 0)  * preferencesModel.uiScale
							anchors.left:		actionMenuButton.left

							orientation:		Qt.Horizontal
							visible:			showToolSeperator(typeRole)
						}
					}
				}
			}
		}

		FocusScope
		{
			id:					resourceMenu

			width:				fileMenu.colWidths * preferencesModel.uiScale
			height:				parent.height
			anchors.left:		actionMenu.right
			anchors.leftMargin: hasButtons ? 0 : - fileMenu.colWidths * preferencesModel.uiScale
			z:					1

			onFocusChanged:		if(focus) fileMenuModel.resourceButtons.selectFirstButtonIfNoneSelected();

			Keys.onLeftPressed: actionMenu.forceActiveFocus();

			Behavior on anchors.leftMargin
			{
				enabled:		!preferencesModel.safeGraphics

				PropertyAnimation
				{
					id:				resourceMenuAnimation
					duration:		jaspTheme.fileMenuSlideDuration
					easing.type:	Easing.OutCubic
				}
			}

			Rectangle
			{
				color:			jaspTheme.fileMenuColorBackground
				border.width:	1
				border.color:	jaspTheme.uiBorder
				z:				-1
				anchors.fill:	parent
			}

			property bool hasButtons: resourceRepeaterId.count > 0

			visible: hasButtons

			Column
			{
				id:							resourceLocation

				anchors.top:				parent.top
				anchors.topMargin:			5 * preferencesModel.uiScale
				anchors.horizontalCenter:	parent.horizontalCenter
				width:						parent.width - jaspTheme.generalAnchorMargin

				spacing:					6 * preferencesModel.uiScale

				Repeater
				{
					id:		resourceRepeaterId
					model:	fileMenuModel.resourceButtonsVisible

					Item
					{

						id:					itemResourceMenu
						width:				parent.width - (6 * preferencesModel.uiScale)
						height:				resourceButtonHeight
						anchors.leftMargin: 3 * preferencesModel.uiScale
						anchors.left:		parent.left
						enabled:			enabledRole

						MenuButton
						{
							id:						resourceButton
							hasSubMenu:				true
							width:					parent.width
							height:					parent.height
							anchors.left:			parent.left
							enabled:				!resourceMenuAnimation.running

							text:					nameRole
							selected:				selectedRole
							focus:					selectedRole

							Keys.onLeftPressed:		actionMenu.forceActiveFocus();
							Keys.onRightPressed:	showSelectedSubScreen.forceActiveFocus()
							Keys.onDownPressed:		fileMenuModel.resourceButtons.selectButtonDown()
							Keys.onUpPressed:		fileMenuModel.resourceButtons.selectButtonUp()
							onHoverClicked:			fileMenuModel.resourceButtons.selectedButton = typeRole
							onClicked:
							{
								hoverClicked();
								showSelectedSubScreen.forceActiveFocus();
							}
						}

					}
				}
			}
		}

		Item
		{
			id:			dropShadow

			y:			0
			x:			resourceScreen.x + resourceScreen.width
			height:		resourceScreen.height
			width:		jaspTheme.shadowRadius

			visible:	resourceScreen.visible
			z:			-3

			Rectangle
			{
				anchors.centerIn:	parent
				height:				dropShadow.width
				width:				dropShadow.height

				rotation: -90
				gradient: Gradient
				{
					GradientStop	{ position: 0.0; color: jaspTheme.shadow; }
					GradientStop	{ position: 1.0; color: "transparent" }
				}
			}
		}

		Rectangle
		{
			property real otherColumnsWidth:	fileMenu.colWidths * fileMenu.nbColumns * preferencesModel.uiScale
			property bool aButtonVisible:		resourceRepeaterId.count > 0 && fileMenuModel.resourceButtons.currentQML !== ''

			property real desiredWidth:			Math.min(mainWindowRoot.width - otherColumnsWidth, 600 * preferencesModel.uiScale)
			property real desiredX:				otherColumnsWidth - (aButtonVisible ? 0 : desiredWidth)

			property string previousQML: ""
			property string currentQML: ""

			id:				resourceScreen

			x:				desiredX
			width:			desiredWidth
			height:			parent.height

			border.width:	1
			border.color:	jaspTheme.grayDarker
			color:			jaspTheme.fileMenuColorBackground
			z:				-2

			Behavior on x
			{
				enabled:		!preferencesModel.safeGraphics

				PropertyAnimation
				{
					id:				fileMenuResourceLoaderAnimation
					duration:		jaspTheme.fileMenuSlideDuration
					easing.type:	Easing.OutCubic
				}
			}

			onXChanged:
				if(resourceScreen.x === resourceScreen.desiredX && resourceScreen.currentQML === "" && resourceScreen.previousQML !== "")
					resourceScreen.previousQML = "";

			Connections
			{
				target:	fileMenuModel.resourceButtons
				onCurrentQMLChanged:
				{
					resourceScreen.previousQML = resourceScreen.currentQML
					resourceScreen.currentQML  = fileMenuModel.resourceButtons.currentQML
				}
			}

			Loader
			{
				id:						showSelectedSubScreen
				anchors.fill:			parent
				source:					resourceScreen.currentQML === "" && resourceScreen.x > resourceScreen.desiredX ? resourceScreen.previousQML : resourceScreen.currentQML
				Keys.onLeftPressed:		resourceMenu.forceActiveFocus()
				Keys.onEscapePressed:	resourceMenu.forceActiveFocus()
			}
		}

		MouseArea
		{
			id:					gottaCatchEmAll //Clicks that is
			anchors.fill:		parent
			z:					-6
			onWheel:			wheel.accepted = true
		}
	}
}
