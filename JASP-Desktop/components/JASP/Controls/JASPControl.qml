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

import QtQuick				2.11
import JASP					1.0
import QtQuick.Controls		2.4
import QtQuick.Layouts		1.3 as L


JASPControlBase
{
	id: jaspControl

	property bool	debug:					false
	property var	background:				null
	property var	focusIndicator:			background
	property bool   indent:                 false
	property alias	cursorShape:			controlMouseArea.cursorShape
	property alias	hovered:				controlMouseArea.containsMouse
	property bool	useControlMouseArea:	true
	property bool	childControlHasFocus:	false
	property bool	isDependency:			false
	property var	dependencyMustContain:	[] //Will be filled with QStringList when necessary
	property bool	shouldShowFocus:		activeFocus && focusOnTab && !childControlHasFocus

	L.Layout.leftMargin: indent ? aspTheme.indentationLength : 0

	function showControlError(message)
	{
		form.showControlError(jaspControl, message, false)
	}

	function showControlErrorTemporary(message)
	{
		form.showControlError(jaspControl, message, true)
	}

	function clearControlError()
	{
		form.clearControlError()
	}

	function setDebugState()
	{
		debug = true
		if (typeof(DEBUG_MODE) === "undefined" || !DEBUG_MODE)
			visible = false;
		else if (background)
			background.color = jaspTheme.debugBackgroundColor
		
		if (form && childControlsArea)
		{
			var	childControls = []

			form.getJASPControls(childControls, childControlsArea, false)
			for (var i = 0; i < childControls.length; i++)
				childControls[i].setDebugState();
		}
	}
	
	Component.onCompleted:
	{
		if (typeof(control) !== "undefined")
		{
			if (!background)
				background = control.background
		}
		
		if (debug)
			setDebugState();

		if (typeof(form) !== "undefined" && form && childControlsArea && childControlsArea.children.length > 0)
		{
			var childControls = [];
			form.getJASPControls(childControls, childControlsArea, false)
			if (controlType !== JASPControlBase.Expander)
				childControlHasFocus = Qt.binding(function()
				{
					for (var i = 0; i < childControls.length; i++)
						if (childControls[i].activeFocus)
							return true;
					return false;
				});
		}
	}
	
	states: [
		State
		{
			name: "hasFocus"
			when: focusIndicator && jaspControl.shouldShowFocus
			PropertyChanges
			{
				target:			focusIndicator
				border.color:	jaspTheme.focusBorderColor
				border.width:	jaspTheme.jaspControlHighlightWidth
			}
		},

		State
		{
			name: "isDependency"
			when: focusIndicator && jaspControl.isDependency && !jaspControl.shouldShowFocus
			PropertyChanges
			{
				target:			focusIndicator
				border.color:	jaspTheme.dependencyBorderColor
				border.width:	jaspTheme.jaspControlHighlightWidth
			}
		}
	]

	transitions: [
		Transition
		{
			to: "hasFocus"
			ParallelAnimation
			{
				NumberAnimation
				{
					target:				focusIndicator
					properties:			"border.width";
					duration:			800;
					easing.type:		Easing.OutElastic;
					easing.amplitude:	1.5;
				}
				ColorAnimation
				{
					target:		focusIndicator
					duration:	100
				}
			}
		}
	]

	property string	toolTip:				""

	ToolTip.text:				toolTip
	ToolTip.timeout:			jaspTheme.toolTipTimeout
	ToolTip.delay:				jaspTheme.toolTipDelay
	ToolTip.toolTip.font:		jaspTheme.font
	ToolTip.visible:			toolTip !== "" && controlMouseArea.containsMouse

	MouseArea
	{
		z:					5
		anchors.fill:		useControlMouseArea ? parent : undefined
		id:					controlMouseArea
		hoverEnabled:		true
		acceptedButtons:	Qt.NoButton
		cursorShape:		Qt.PointingHandCursor
	}
}
