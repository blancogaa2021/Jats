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
import QtQuick.Layouts 1.3 as L
import JASP.Theme 1.0

Rectangle {
	id:					control
	
	implicitWidth:		Math.max(label.realWidth, Theme.groupContentPadding + contentArea.implicitWidth)
	implicitHeight:		label.realHeight + Theme.titleBottomMargin + contentArea.implicitHeight	
	color:				Theme.analysisBackgroundColor // transparent generates sometimes temporary black blocks
	L.Layout.leftMargin:	indent ? Theme.indentationLength : 0
	visible:			!debug || DEBUG_MODE
    
	default property alias	content:			contentArea.children
			property alias	contentArea:		contentArea
			property int	rowSpacing:			Theme.rowGroupSpacing
			property int	columnSpacing:		Theme.columnGroupSpacing
			property int	columns:			1
			property string title:				""
			property bool	debug:				false
			property bool	indent:				false
			property bool	alignTextFields:	true
			property var	childControls:		[]
			property alias	alignChildrenTopLeft: contentArea.alignChildrenTopLeft
			property alias	label:				label

	Label
	{
		id:				label
		anchors.top:	control.top
		anchors.left:	control.left
		text:			control.title
		color:			enabled ? Theme.textEnabled : Theme.textDisabled
		font:			Theme.font
		visible:		control.title ? true : false
		
		property int	realHeight: visible ? implicitHeight : 0
		property int	realWidth: visible ? implicitWidth : 0
		
    }
    
	GridLayout
	{
		id:					contentArea
		columns:			control.columns
		anchors.top:		control.title ? label.bottom : control.top
		anchors.topMargin:	control.title ? Theme.titleBottomMargin : 0
		anchors.left:		control.left
        anchors.leftMargin: control.title ? Theme.groupContentPadding : 0
		rowSpacing:			control.rowSpacing
		columnSpacing:		control.columnSpacing
    }
    
	Component.onCompleted:
	{
		var i;
        form.getJASPControls(childControls, contentArea, false)
		for (i = 0; i < childControls.length; i++)
		{
            if (control.debug)
				childControls[i].setDebugState();
		}
		
		if (alignTextFields)
		{
            var textFieldsControls = [];
            var maxX = 0;
			for (i = 0; i < contentArea.children.length; i++)
			{
                var child = contentArea.children[i];
				if (child.hasOwnProperty('controlType') && child.controlType === 'TextField')
				{
                    textFieldsControls.push(child);
                    if (maxX < child.control.x) maxX = child.control.x;
                }
            }
            
			for (i = 0; i < textFieldsControls.length; i++)
                textFieldsControls[i].control.L.Layout.leftMargin = maxX - textFieldsControls[i].control.x;
        }
    }
}
