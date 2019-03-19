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
import QtQuick.Controls 2.4
import QtQml.Models 2.2
import JASP.Controls 1.0
import JASP.Theme 1.0

JASPControl
{
	id:							repeatedMeasuresFactorsList
	controlType:				"RepeatedMeasuresFactorsList"
	width:						parent.width
	implicitWidth:				width
	height:						Theme.defaultListHeight
	implicitHeight:				height
	background:					rectangle

				property var	model
				property string title
				property bool	setWidthInForm: false
				property bool	setHeightInForm: false
	readonly	property string deleteIcon: "dialog-close.png"

	signal itemChanged(int index, var name);
	signal itemRemoved(int index);

	Text
	{
		id:				text
		anchors.top:	parent.top
		anchors.left:	parent.left
		text:			repeatedMeasuresFactorsList.title
		height:			repeatedMeasuresFactorsList.title ? 20 * preferencesModel.uiScale : 0
		font:			Theme.font
		color:			enabled ? Theme.textEnabled : Theme.textDisabled
	}

	Rectangle
	{
		id:				rectangle
		anchors.top:	text.bottom
		anchors.left:	parent.left
		height:			repeatedMeasuresFactorsList.height - text.height
		width:			parent.width
		color:			debug ? Theme.debugBackgroundColor : Theme.controlBackgroundColor
		border.width:	1
		border.color:	Theme.borderColor

		GridView
		{
			id:					listView
			cellHeight:			23 * preferencesModel.uiScale
			cellWidth:			width
			clip:				true

			anchors.fill:		parent
			anchors.margins:	4 * preferencesModel.uiScale
			model:				repeatedMeasuresFactorsList.model
			delegate:			itemComponent
		}
	}

	Component
	{
		id: itemComponent

		FocusScope
		{
			id:		itemWrapper
			height: listView.cellHeight
			width:	listView.cellWidth

			Rectangle
			{
				id:				itemRectangle
				anchors.fill:	parent
				focus:			true
				border.width:	0
				border.color:	Theme.grayLighter

				property bool isDeletable:	model.type.includes("deletable")
				property bool isVirtual:	model.type.includes("virtual")
				property bool isLevel:		model.type.includes("level")

				TextField
				{
					id:								colName
					value:							itemRectangle.isVirtual ? "" : model.name
					placeholderText:				itemRectangle.isVirtual ? model.name : ""
					anchors.verticalCenter:			parent.verticalCenter
					fieldWidth:						parent.width - (6 * preferencesModel.uiScale)
					useExternalBorder:				false
					control.horizontalAlignment:	itemRectangle.isLevel ? TextInput.AlignLeft : TextInput.AlignHCenter
					onEditingFinished:				itemChanged(index, value)
				}

				Image
				{
					source:					iconPath + deleteIcon
					anchors.right:			parent.right
					anchors.verticalCenter:	parent.verticalCenter
					visible:				itemRectangle.isDeletable
					height:					16 * preferencesModel.uiScale
					width:					16 * preferencesModel.uiScale
					z:						2

					MouseArea
					{
						anchors.fill: parent
						onClicked: itemRemoved(index)
					}
				}
			}
		}
	}
}
