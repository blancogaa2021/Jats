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

import QtQuick			2.11
import QtQuick.Controls	2.4
import JASP.Theme		1.0

JASPControl
{
	id:							radioButton
	controlType:				"RadioButton"
	isBound:					false
	useDefaultBackground:		true
	implicitHeight:				control.height
	implicitWidth:				control.width

	property alias	text:		control.text
	property alias	checked:	control.checked
	property alias	value:		radioButton.name
	property var	buttonGroup

	RadioButton
	{
		id:					control
		ButtonGroup.group:	buttonGroup
		padding:			Theme.jaspControlPadding

		indicator: Rectangle
		{
			id:				radioIndicator
			width:			height
			height:			label.height
			x:				control.padding
			y:				control.padding

			radius:			width
			color:			control.checked ? (control.enabled ? Theme.buttonBackgroundColor : Theme.disableControlBackgroundColor) : Theme.controlBackgroundColor
			border.color:	control.enabled ? (control.checked ? Theme.buttonBackgroundColor : Theme.borderColor)					: Theme.disableControlBackgroundColor
			border.width:	1

			Rectangle
			{
				anchors.centerIn:	parent
				width:				parent.width / 2
				height:				parent.height / 2
				radius:				width
				visible:			control.checked
				color:				Theme.controlBackgroundColor
            }
        }

		contentItem: Label
		{
			id:				label
			text:			control.text
			leftPadding:	radioIndicator.width + control.padding
			font:			Theme.font
			color:			enabled ? Theme.textEnabled : Theme.textDisabled
        }
    }
}
