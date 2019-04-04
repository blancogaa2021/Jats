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

			Item
			{
				id:					fileBrowseArea
				anchors.top:		parent.top
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
					id:					folderRect
					anchors.top:		parent.top
					anchors.left:		parent.left
					anchors.right:		parent.horizontalCenter
					anchors.bottom:		parent.bottom
					anchors.margins:	Theme.generalAnchorMargin



					color:			Theme.whiteBroken
					border.color:	Theme.grayLighter
					border.width:	1

					ListView
					{
						id:					folderList
						anchors.fill:		parent
						anchors.margins:	Theme.generalAnchorMargin
						clip:				true
						spacing:			Theme.rowSpacing

						property string currentlySelectedFilePath: ""

						FolderListModel
						{
							id: folderModel

							nameFilters:		["*.tar.gz"]
							showDotAndDotDot:	true
							showDirs:			true
						}

						Component
						{
							id: fileDelegate

							RectangularButton
							{
								id:					fileDelegateRect
								height:				32
								width:				folderList.width - (Theme.generalAnchorMargin * 2)
								x:					Theme.generalAnchorMargin
								showIconAndText:	true
								centerText:			false

								border.color:		Theme.grayDarker
								border.width:		1
								selected:			folderList.currentlySelectedFilePath !== "" && folderList.currentlySelectedFilePath === filePath
								enabled:			!selected

								property int margins: 4


								iconSource: fileIsDir			? "qrc:///icons/folder.svg"			: "qrc:///icons/JASP_logo_green.svg"
								toolTip:	selected			? "Currently selected JASP Module"	: fileIsDir	? "a folder, click to enter" : "a JASP Module, click it to see more information"
								text:		fileName === ".."	? fileName + " (directory up)"		: fileName


								onClicked:
								{
									folderList.currentlySelectedFilePath = ""

									if(fileIsDir)
										folderModel.folder = "file:" + filePath
									else
									{
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
											{
												descriptionViewer.text = "<h3>" + title + "</h3><i>Version " + version + "</i><br><p>" + description + "</p><br><br><i>Created by " + author + " and maintained by " + maintainer + ".<br>See website for further details: <a href=\"http://" + website + "\">" + website + "</a></i>"
												folderList.currentlySelectedFilePath = filePath
											}

										}
										else
											descriptionViewer.text = filePath + " is not a JASP Module!"
									}

								}

							}

						}

						model:		folderModel
						delegate:	fileDelegate
					}
				}

				Rectangle
				{
					anchors.top:		parent.top
					anchors.left:		folderRect.right
					anchors.right:		parent.right
					anchors.bottom:		parent.bottom
					anchors.margins:	Theme.generalAnchorMargin

					color:				Theme.whiteBroken
					border.color:		Theme.gray
					border.width:		1


					Text
					{
						id: descriptionViewer
						anchors.fill:		parent
						anchors.margins:	Theme.generalAnchorMargin
						text:				"<i>Click a JASP Module to see more information here</i>"
						textFormat:			Text.StyledText
						wrapMode:			Text.WrapAtWordBoundaryOrAnywhere
						onLinkActivated:	Qt.openUrlExternally(link)
						clip:				true

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
					right:		closeButtonCross.left
					bottom:		installProgressItem.top
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

						dynamicModules.installJASPModule(folderList.currentlySelectedFilePath)

						moduleInstallerPopup.close() //There is no information being shown anyway so lets just close the window now
					}
				}
			}

			RectangularButton
			{
				id:				closeButtonCross
				iconSource:		"qrc:/images/cross.png"
				width:			height
				height:			installButton.height
				anchors
				{
					right:		parent.right
					bottom:		installProgressItem.top
					margins:	Theme.generalAnchorMargin
				}
				onClicked:	moduleInstallerPopup.close()
			}

			Item
			{
				id:				installProgressItem

				property string moduleBeingInstalled: ""
				anchors.left:	parent.left
				anchors.right:	parent.right
				anchors.bottom: parent.bottom

				Behavior on height { PropertyAnimation {} }

				height: 0
				visible: height > 0

				Rectangle
				{
					anchors.fill:		parent
					anchors.margins:	Theme.generalAnchorMargin

					color:				Theme.whiteBroken
					border.color:		Theme.grayLighter
					border.width:		1

					Text
					{
						anchors.fill:		parent
						anchors.margins:	Theme.generalAnchorMargin
						wrapMode:			Text.WrapAtWordBoundaryOrAnywhere
						text:				installProgressItem.moduleBeingInstalled === "" ? "" : dynamicModules.dynamicModule(installProgressItem.moduleBeingInstalled).installLog
					}
				}
			}
		}
	}
}
