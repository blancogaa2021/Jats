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
import QtQuick.Controls 2.12
import JASP				1.0


JASPControl
{
	id:					button
	controlType:		JASPControlBase.Button
	implicitHeight:		control.implicitHeight
	implicitWidth:		control.implicitWidth
	isBound:			false
	opacity:			enabled ? 1 : .7
	
	property alias control: control
    property alias text:	control.text
	property alias label:	control.text
    property alias image:	image
	property alias down:	control.down
	property alias pressed:	control.pressed
	property alias background: control.background
    
    signal clicked()
    
	Component.onCompleted: control.clicked.connect(clicked);

	Button
	{
		id:	control

		font: jaspTheme.font
		Image
		{
			id:					image
			fillMode:			Image.PreserveAspectFit
			anchors.centerIn:	parent
			sourceSize.height:	background.height - (6 * preferencesModel.uiScale)
			height:				sourceSize.height
			visible:			source ? true : false
        }
    
		background: Rectangle
		{
			id:				rectangle
			border.width:	1
			border.color:	jaspTheme.borderColor
			radius:			jaspTheme.borderRadius
			color:			enabled ? (control.down ? jaspTheme.gray : jaspTheme.controlBackgroundColor) : jaspTheme.disableControlBackgroundColor
            
			Behavior on color {	enabled: !preferencesModel.safeGraphics;	ColorAnimation { duration: 100 } }
        }
        
    }
}
