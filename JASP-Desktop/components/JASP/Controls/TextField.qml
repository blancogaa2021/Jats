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
import QtQuick.Layouts	1.3
import JASP				1.0

JASPControl
{
	id:					textField
	controlType:		JASPControlBase.TextField
	
	implicitHeight:		control.height
	implicitWidth:		afterLabel.text ? (afterLabelRect.x + afterLabelRect.width) : (control.x + control.width)
	background:			useExternalBorder ? externalControlBackground : control.background
	cursorShape:		Qt.IBeamCursor
	
	property alias	control:			control	
	property alias	label:				beforeLabel.text
	property alias	text:				beforeLabel.text
	property alias	value:				control.text
	property string lastValidValue:		""
	property int	fieldWidth:			jaspTheme.textFieldWidth
	property int	fieldHeight:		0
	property bool	useExternalBorder:	true
	property bool	showBorder:			useExternalBorder
	property alias	placeholderText:	control.placeholderText
	property bool	selectValueOnFocus:	false
	
	property alias	validator:			control.validator
	property alias	controlLabel:		beforeLabel
	property alias	afterLabel:			afterLabel.text
	property string	inputType:			"string"
	property bool	useLastValidValue:	true

	property double controlXOffset:		0

	signal editingFinished()
	signal textEdited()
	signal pressed()
	signal released()

	function doEditingFinished()
	{
		lastValidValue = control.text
		editingFinished();
	}
	
	Component.onCompleted:
	{
		if (!beforeLabel.text && textField.text)
			beforeLabel.text = textField.text;
		
		control.editingFinished.connect(doEditingFinished);
		control.textEdited.connect(textEdited);
		control.pressed.connect(pressed);
		control.released.connect(released);        
	}	
		
	Rectangle
	{
		id:					beforeLabelRect
		width:				beforeLabel.implicitWidth
		height:				control.height
		color:				debug ? jaspTheme.debugBackgroundColor : "transparent"
		visible:			beforeLabel.text && textField.visible
		Label
		{
			id:						beforeLabel
			font:					jaspTheme.font
			anchors.verticalCenter: parent.verticalCenter
			color:					enabled ? jaspTheme.textEnabled : jaspTheme.textDisabled
		}
	}

	TextField
	{
		id:						control
		anchors.left:			beforeLabelRect.visible ? beforeLabelRect.right : parent.left
		anchors.leftMargin:		controlXOffset + (beforeLabelRect.visible ? jaspTheme.labelSpacing : 0)
		width:					textField.fieldWidth //+ (textField.useExternalBorder ? 2 * jaspTheme.jaspControlHighlightWidth : 0)
		font:					jaspTheme.font
		activeFocusOnPress:		true
		color:					enabled ? jaspTheme.textEnabled : jaspTheme.textDisabled

		padding:				jaspTheme.jaspControlPadding
		leftPadding:			jaspTheme.labelSpacing
		selectByMouse:			true
		selectedTextColor:		jaspTheme.white
		selectionColor:			jaspTheme.itemSelectedColor

		background: Rectangle
		{
			id:				controlBackground
			color:			jaspTheme.controlBackgroundColor
			border.width:	textField.showBorder && !control.activeFocus	? 1					: 0
			border.color:	textField.showBorder							? jaspTheme.borderColor : "transparent"
		}

		Rectangle
		{
			id:					externalControlBackground
			height:				parent.height + jaspTheme.jaspControlHighlightWidth
			width:				parent.width + jaspTheme.jaspControlHighlightWidth
			color:				"transparent"
			border.width:		3
			border.color:		control.acceptableInput ? "transparent" : jaspTheme.red // Needed when the QML file has wrong default value
			anchors.centerIn:	parent
			opacity:			debug ? .3 : 1
			visible:			textField.useExternalBorder
			radius:				jaspTheme.jaspControlHighlightWidth
		}

		onActiveFocusChanged:
		{
			if (activeFocus)
			{
				if (textField.selectValueOnFocus)
					control.selectAll()
			}

			if (!control.acceptableInput)
			{
				var msg
				if (control.validator && (typeof control.validator.validationMessage === "function"))
					msg = control.validator.validationMessage(beforeLabel.text) + "<br><br>";

				if (textField.useLastValidValue)
					text = textField.lastValidValue
				msg += qsTr("Restoring last correct value: %1").arg(text);
				showControlErrorTemporary(msg)
				textField.background.border.color = jaspTheme.red;
				redToNormal.start()
			}
			else
				clearControlError();
		}

		PropertyAnimation
		{
			id:			redToNormal
			target:		textField.background
			property:	"border.color"
			to:			jaspTheme.focusBorderColor
			duration:	1000
			onStopped:	textField.background.border.color = control.activeFocus ? jaspTheme.focusBorderColor : "transparent"
		}

		Keys.onReturnPressed:
		{
			if (!control.acceptableInput)
			{
				if (control.validator && (typeof control.validator.validationMessage === "function"))
					showControlError(control.validator.validationMessage(beforeLabel.text));
				textField.background.border.color = jaspTheme.red;
				redToNormal.start()
			}
			else
			{
				clearControlError();
				event.accepted = false;
			}
		}
	}

	Binding
	{
		// This is a way to set the property height only if fieldHeight is set
		// If not, height should keep its implicit binding.
		target:		control
		property:	"height"
		value:		textField.fieldHeight
		when:		textField.fieldHeight != 0
	}

	Rectangle
	{
		id:				afterLabelRect
		width:			afterLabel.implicitWidth
		height:			control.height
		color:			debug ? jaspTheme.debugBackgroundColor : "transparent"
		visible:		afterLabel.text && textField.visible
		anchors.left:	control.right
		anchors.leftMargin: jaspTheme.labelSpacing
		Label
		{
			id:			afterLabel
			font:		jaspTheme.font
			anchors.verticalCenter: parent.verticalCenter
			color:		enabled ? jaspTheme.textEnabled : jaspTheme.textDisabled
		}
	}
}
