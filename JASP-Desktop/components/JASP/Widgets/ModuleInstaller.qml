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

import QtQuick					2.11
import QtQuick.Controls			2.4
import Qt.labs.folderlistmodel	2.11
//import QtQuick.Window			2.3
import JASP.Theme				1.0
import JASP.Widgets				1.0

Popup
{
	id:			moduleInstallerPopup

	width:		900
	height:		500

	modal: true; focus: true;

	background: Rectangle { color: Theme.uiBackground }



	y: (parent.height / 2) - (height / 2)
	x: (parent.width / 2) - (width / 2)

	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

	Loader
	{
		visible:			moduleInstallerPopup.opened
		sourceComponent:	visible ? moduleInstallerComponent : null
		anchors.fill:		parent
	}

	Component
	{
		id:		moduleInstallerComponent

		Item
		{
			id:			moduleInstallerRect


			property var currentJSON: null

			RectangularButton
			{
				id:					browseButton
				iconSource:			"qrc:///icons/folder.svg"
				showIconAndText:	true
				width:				height
				height:				installButton.height
				text:				qsTr("Browse for JASP Module");
				anchors
				{
					top:		parent.top
					left:		parent.left
					right:		closeButtonCross.left
					margins:	Theme.generalAnchorMargin
				}

				function browse() { return mainWindow.browseOpenFileDocuments(qsTr("Select a JASP Module"), "*.tar.gz"); }


				onClicked:		descriptionViewer.currentlySelectedFilePath = browse();
			}

			RectangularButton
			{
				id:				closeButtonCross
				iconSource:		"qrc:/images/cross.png"
				width:			height
				height:			installButton.height
				anchors
				{
					top:		parent.top
					right:		parent.right
					margins:	Theme.generalAnchorMargin
				}
				onClicked:	moduleInstallerPopup.close()
			}

			Item
			{
				id:					fileBrowseArea
				anchors.top:		browseButton.bottom
				anchors.left:		parent.left
				anchors.right:		parent.right
				anchors.bottom:		installButton.top

				onHeightChanged:
				{
					if(height <= Theme.generalAnchorMargin * 2)
						visible = false
				}

				Rectangle
				{
					anchors.fill:		parent
					anchors.margins:	Theme.generalAnchorMargin

					color:				Theme.whiteBroken
					border.color:		Theme.gray
					border.width:		1


					Text
					{
						id:					descriptionViewer
						anchors.fill:		parent
						anchors.margins:	Theme.generalAnchorMargin
						text:				defaultText
						textFormat:			Text.StyledText
						wrapMode:			Text.WrapAtWordBoundaryOrAnywhere
						onLinkActivated:	Qt.openUrlExternally(link)
						clip:				true

						property string currentlySelectedFilePath: browseButton.browse()
						property string defaultText: "<i>Browse for a JASP Module to see more information here</i>"

						onCurrentlySelectedFilePathChanged: if(currentlySelectedFilePath !== "") showDescription();

						function showDescription()
						{
							var filePath = currentlySelectedFilePath;

							if(filePath === "")
							{
								descriptionViewer.text = descriptionViewer.defaultText;
								return;
							}

							if(dynamicModules.isFileAnArchive(filePath))
							{
								var textJson			= dynamicModules.getDescriptionJsonFromArchive(filePath)
								descriptionViewer.text	= "<i>File is not a JASP module or something is wrong with it.</i>"

								moduleInstallerRect.currentJSON = JSON.parse(textJson)

								var moduleDescription	= moduleInstallerRect.currentJSON.moduleDescription

								var title				= moduleDescription.title
								var description			= moduleDescription.description
								var version				= moduleDescription.version
								var author				= moduleDescription.author
								var maintainer			= moduleDescription.maintainer
								var website				= moduleDescription.website

								if(textJson !== undefined && textJson !== "")
									descriptionViewer.text = "<h3>" + title + "</h3><i>Version " + version + "</i><br><p>" + description + "</p><br><br><i>Created by " + author + " and maintained by " + maintainer + ".<br>See website for further details: <a href=\"http://" + website + "\">" + website + "</a></i>"
								else
									descriptionViewer.currentlySelectedFilePath = "";
							}
							else
							{
								descriptionViewer.text = filePath + " is not a JASP Module!"
								descriptionViewer.currentlySelectedFilePath = "";
							}
						}

						MouseArea
						{
							anchors.fill:		parent
							acceptedButtons:	Qt.NoButton
							cursorShape:		descriptionViewer.hoveredLink !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
						}
					}
				}
			}

			RectangularButton
			{
				id:					installButton
				anchors
				{
					left:		parent.left
					right:		parent.right
					bottom:		parent.bottom
					margins:	Theme.generalAnchorMargin
				}
				enabled:			moduleInstallerRect.currentJSON !== null

				text:				"Install"
				toolTip:			enabled ? "Press this to install your selected Module" : "Select a JASP Module to install"

				onClicked:
				{
					if(moduleInstallerRect.currentJSON !== null)
					{
						enabled							= false
						selected						= true

						dynamicModules.installJASPModule(descriptionViewer.currentlySelectedFilePath)

						moduleInstallerPopup.close() //There is no information being shown anyway so lets just close the window now
					}
				}
			}
		}
	}
}
