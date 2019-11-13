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
import QtQml.Models 2.2
import JASP.Controls 1.0

JASPListControl
{
	id:							inputListView
	controlType:				"InputListView"
	itemComponent:				itemInputComponent

				property var	control			: textField
				property bool	enableExtraColumns	: true
				property string	optionKey		: "value"
				property var	defaultValues	: []
				property int	minimumItems	: 0
				property bool	addVirtual		: true
				property string placeHolder		: qsTr("New Value")
	readonly	property string deleteIcon		: "cross.png"

	signal itemChanged(int index, var name);
	signal itemRemoved(int index);

	Component
	{
		id: itemInputComponent

		FocusScope
		{
			id:		itemWrapper
			height: listGridView.cellHeight
			width:	scrollBar.visible ?  listGridView.cellWidth - scrollBar.width : listGridView.cellWidth

			Rectangle
			{
				id:				itemRectangle
				anchors.fill:	parent
				focus:			true
				border.width:	0
				border.color:	jaspTheme.grayLighter

				property bool	isDeletable:		model.type.includes("deletable")
				property bool	isVirtual:			model.type.includes("virtual")
				property var	extraColumnsModel:	model.extraColumns

				Loader
				{
					sourceComponent: inputListView.control
					asynchronous:	false

					property var modelValue: model
					property int modelIndex: index
					property bool isVirtual: itemRectangle.isVirtual

					onLoaded:
					{
						if (item.hasOwnProperty("fieldWidth"))
							item.fieldWidth = Qt.binding( function() { return itemRectangle.width - extraControls.width - deleteIconID.width; })
					}
				}

				ExtraControls
				{
					id:					extraControls
					anchors.rightMargin: deleteIconID.width
					model:				itemRectangle.extraColumnsModel
					controlComponents:  inputListView.extraControlComponents
					enabled:			!itemRectangle.isVirtual && inputListView.enableExtraColumns
				}

				Image
				{
					id:						deleteIconID
					source:					jaspTheme.iconPath + deleteIcon
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

	Component
	{
		id: textField
		TextField
		{
			value:							isVirtual ? "" : modelValue.name
			placeholderText:				isVirtual ? modelValue.name : ""
			useExternalBorder:				false
			selectValueOnFocus:				true
			control.horizontalAlignment:	TextInput.AlignLeft
			onEditingFinished:				inputListView.itemChanged(modelIndex, value)
		}
	}

}
