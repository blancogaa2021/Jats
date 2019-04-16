//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

import QtQuick 2.11
import QtQuick.Window 2.11
import JASP.Widgets 1.0
import JASP.Theme 1.0

Window
{
	id:			mainWindowRoot
	title:		mainWindow.windowTitle
	visible:	true
	width:		1024
	height:		768


	minimumWidth:	640
	minimumHeight:	480

	onVisibleChanged: if(!visible) helpModel.visible = false

	property real devicePixelRatio: Screen.devicePixelRatio

	onDevicePixelRatioChanged: if(devicePixelRatio > 0) mainWindow.screenPPI = devicePixelRatio * 96

	onClosing: close.accepted = mainWindow.checkPackageModifiedBeforeClosing();

	Item
	{
		anchors.fill: parent

		focus:	true
		Shortcut
		{
			sequences: [Qt.Key_ZoomIn, "Ctrl+Plus", "Ctrl+\+", "Ctrl+\="]
			onActivated: mainWindow.zoomInKeyPressed()
		}
		Shortcut
		{
			sequences: [Qt.Key_ZoomOut, "Ctrl+Minus", "Ctrl+\-"]
			onActivated: mainWindow.zoomOutKeyPressed();
		}
		Shortcut
		{
			sequences: ["Ctrl+0"]
			onActivated: mainWindow.zoomResetKeyPressed();
		}
		Shortcut
		{
			sequences: ["Ctrl+S"]
			onActivated: mainWindow.saveKeyPressed();
		}
		Shortcut
		{
			sequences: ["Ctrl+O"]
			onActivated: mainWindow.openKeyPressed();
		}
		Shortcut
		{
			sequences: ["Ctrl+Y"]
			onActivated: mainWindow.syncKeyPressed();
		}
		Shortcut
		{
			sequences: ["Ctrl+R"]
			onActivated: mainWindow.refreshKeyPressed();
		}

		RibbonBar
		{
			id	: ribbon
			z	: 4

			anchors
			{
				top:	parent.top
				left:	parent.left
				right:	parent.right
			}
		}

		CustomMenu
		{
			id	: customMenu
			z	: 5

			function showMenu(item, props, x_offset, y_offset)
			{
				customMenu.props	= props;

				var point			= item.mapToItem(null, 0, 0);
				customMenu.x		= point.x + x_offset;
				customMenu.y		= point.y + y_offset;
				var rightX = customMenu.x + customMenu.width + 2;
				var bottomY = customMenu.y + customMenu.height + 2
				if (rightX > mainWindowRoot.width)
					customMenu.x -= (rightX - mainWindowRoot.width);
				if (bottomY > mainWindowRoot.height)
					customMenu.y -= (bottomY - mainWindowRoot.height);
				customMenu.visible	= true;
			}
		}

		FileMenu
		{
			id:			filemenu
			z:			3

			anchors
			{
				top:	ribbon.bottom
				left:	parent.left
				bottom:	parent.bottom
			}
		}

		MainPage
		{
			id: mainpage
			z:	0

			anchors
			{
				top:	ribbon.bottom
				left:	parent.left
				right:	parent.right
				bottom:	parent.bottom
			}
		}

		MouseArea
		{
			visible:		fileMenuModel.visible || modulesMenu.opened || customMenu.visible
			z:				1
			hoverEnabled:	true

			onContainsMouseChanged: if(containsMouse) ribbonModel.highlightedModuleIndex = -1

			anchors.fill:	parent

			//Rectangle { id: purpleDebugRect; color: "purple"; anchors.fill: parent }

			onClicked:
			{
				fileMenuModel.visible	= false
				modulesMenu.opened		= false
				mouse.accepted			= false
				customMenu.visible		= false
			}
		}

		ModulesMenu
		{
			id:			modulesMenu
			z:			2

			anchors
			{
				top:	ribbon.bottom
				right:	parent.right
				bottom:	parent.bottom
			}
		}

		CreateComputeColumnDialog	{ id: createComputeDialog	}
		ModuleInstaller				{ id: moduleInstallerDialog	}

		/*MessageBox
		{
			id:	msgBox
			z:	2

			Connections
			{
				target:			mainWindow
				onShowWarning:	msgBox.showWarning(title, message)
			}

		}*/
	}
}
