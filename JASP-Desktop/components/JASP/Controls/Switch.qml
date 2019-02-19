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
import QtQuick.Controls 2.4
import JASP.Theme		1.0

JASPControl
{
	controlType:				"Switch"
	implicitWidth:				control.width
	implicitHeight:				control.height
	
	property alias control:		control
	property alias label:		control.text
	property alias checked:		control.checked
    signal clicked();
    
	Component.onCompleted: control.clicked.connect(clicked);
    
	Switch
	{
		id:			control
		height:		control.indicator.height + (4 * preferencesModel.uiScale)
		width:		control.indicator.width + controlLabel.implicitWidth + control.spacing + (6 * preferencesModel.uiScale)

		indicator:	Rectangle
		{
			id:				switchHandle
			width:			Theme.switchHeight * 2.2
			height:			Theme.switchHeight
			radius:			Theme.switchHeight / 2
			color:			Theme.light
			border.color:	Theme.borderColor
			anchors
			{
				left:		control.left
				leftMargin: 2 * preferencesModel.uiScale
				top:		control.top
				topMargin:	2 * preferencesModel.uiScale
			}
    
			Rectangle
			{
				id:				rectangle
				width:			Theme.switchHeight
				height:			Theme.switchHeight
				radius:			Theme.switchHeight / 2
				color:			Theme.light
				border.color:	Theme.borderColor
            }
        }
        
		contentItem: Label
		{
			id:				controlLabel
			text:			control.text
			font:			Theme.font
			color:			enabled ? Theme.textEnabled : Theme.textDisabled
			anchors
			{
				left:		control.indicator.right
				leftMargin: control.spacing
				top:		control.top
				topMargin:	2 * preferencesModel.uiScale
			}
        }
    }
}
