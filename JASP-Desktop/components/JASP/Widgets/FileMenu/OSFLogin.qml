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

import QtQuick 2.12
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.12
import JASP.Controls 1.0
import JASP.Theme 1.0
import JASP.Widgets 1.0

FocusScope
{
	id: osfLogin

	Rectangle
	{
		color:			Theme.grayMuchLighter
		z:				-1
		anchors.fill:	parent
	}

	Component.onCompleted:
	{
		usernameText.focus = true
	}

	Image
	{
		id: osfLogo

		height: 100 * preferencesModel.uiScale
		width : 100 * preferencesModel.uiScale
		source: "qrc:/images/osf-logo.png"
		smooth: true

		sourceSize.width : width  * 2
		sourceSize.height: height * 2

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin    : 20 * preferencesModel.uiScale
	}

	Label
	{
		id: labelOSF

		width : osfLoginBox.width
		height: 40 * preferencesModel.uiScale

		verticalAlignment  : Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: osfLogo.bottom
		anchors.bottomMargin: 20 * preferencesModel.uiScale

		text : qsTr("OSF")
		color: Theme.black
		font : Theme.fontLabel
	}

	Text
	{
		id: labelExplain

		text : qsTr("Sign in with your OSF account to continue")
		color:Theme.black
		font.pointSize: 11 * preferencesModel.uiScale
		font.family:	Theme.font.family

		verticalAlignment  : Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top             : labelOSF.bottom
		anchors.topMargin       : 20 * preferencesModel.uiScale
	}

	Rectangle
	{
		id: osfLoginBox

		// TODO: Should be in Theme? Yes! And probably scaled as well ;)
		height: 240 * preferencesModel.uiScale
		width : 250 * preferencesModel.uiScale
		color : Theme.grayMuchLighter

		border.width: 1
		border.color: Theme.grayDarker

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: labelExplain.bottom
		anchors.topMargin: 10 * preferencesModel.uiScale

		Rectangle
		{
			id: usernameInput

			anchors.left : parent.left
			anchors.right: parent.right
			anchors.top  : parent.top

			anchors.leftMargin : 20 * preferencesModel.uiScale
			anchors.rightMargin: 20 * preferencesModel.uiScale
			anchors.topMargin  : 30 * preferencesModel.uiScale

			height		: 35 * preferencesModel.uiScale
			width		: 100 * preferencesModel.uiScale
			clip		: true
			color		: Theme.white
			border.width: usernameText.activeFocus ? 3 : 1
			border.color: usernameText.activeFocus ? Theme.focusBorderColor : Theme.grayDarker

			TextInput
			{
				id					: usernameText
				text				: fileMenuModel.osf.username

				anchors.fill		: parent
				anchors.leftMargin	: 10 * preferencesModel.uiScale
				selectByMouse		: true
				selectedTextColor	: Theme.white
				selectionColor		: Theme.itemSelectedColor


				verticalAlignment	: Text.AlignVCenter
				font				: Theme.fontRibbon

				onTextChanged		: fileMenuModel.osf.username = text
				onAccepted			: passwordText.focus = true

				KeyNavigation.down	: passwordText
				KeyNavigation.tab	: passwordText
				focus				: true
			}
		}

		Rectangle
		{
			id: passwordInput

			anchors.left  : parent.left
			anchors.right : parent.right
			anchors.top   : usernameInput.bottom

			anchors.leftMargin : 20 * preferencesModel.uiScale
			anchors.rightMargin: 20 * preferencesModel.uiScale
			anchors.topMargin  : 15 * preferencesModel.uiScale

			height: 35 * preferencesModel.uiScale
			width : 100 * preferencesModel.uiScale
			clip  : true
			color : Theme.white

			border.width: passwordText.activeFocus ? 3 : 1
			border.color: passwordText.activeFocus ? Theme.focusBorderColor  : Theme.grayDarker

			TextInput
			{
				id: passwordText

				text:fileMenuModel.osf.password

				anchors.fill		: parent
				anchors.leftMargin	: 10 * preferencesModel.uiScale
				verticalAlignment	: Text.AlignVCenter
				echoMode			: TextInput.Password
				selectByMouse		: true
				selectedTextColor	: Theme.white
				selectionColor		: Theme.itemSelectedColor

				font.pixelSize    : 14 * preferencesModel.uiScale


				onTextChanged:	fileMenuModel.osf.password = text;
				onAccepted:		fileMenuModel.osf.loginRequested(fileMenuModel.osf.username, fileMenuModel.osf.password)

				KeyNavigation.up		: usernameText
				KeyNavigation.backtab	: usernameText
				KeyNavigation.down		: loginButton
				KeyNavigation.tab		: loginButton

			}
		}

		RectangularButton
		{
			id: loginButton

			height   : 35  * preferencesModel.uiScale
			text     : qsTr("Sign in")
			color    : "#5cb85c"  // TODO: Move this to Theme.qml
			border.width: loginButton.activeFocus ? 3 : 1
			border.color: loginButton.activeFocus ? Theme.focusBorderColor : Theme.grayDarker

			textColor: Theme.white

			anchors.top  : passwordInput.bottom
			anchors.right: parent.right
			anchors.left : parent.left

			anchors.topMargin  : 15  * preferencesModel.uiScale
			anchors.rightMargin: 20  * preferencesModel.uiScale
			anchors.leftMargin : 20  * preferencesModel.uiScale

			onClicked:			fileMenuModel.osf.loginRequested(fileMenuModel.osf.username, fileMenuModel.osf.password)

			KeyNavigation.up		: passwordText
			KeyNavigation.backtab	: passwordText
			KeyNavigation.down		: idRememberMe
			KeyNavigation.tab		: idRememberMe
		}

		CheckBox
		{
			id: idRememberMe

			checked: fileMenuModel.osf.rememberme
			label   : qsTr("Remember me")

			anchors.left  : parent.left
			anchors.right : parent.right
			anchors.bottom: parent.bottom

			anchors.bottomMargin: 30 * preferencesModel.uiScale
			anchors.leftMargin  : 20 * preferencesModel.uiScale
			anchors.topMargin   : 10 * preferencesModel.uiScale

			onCheckedChanged:	fileMenuModel.osf.rememberme = checked

			KeyNavigation.up		: loginButton
			KeyNavigation.backtab	: loginButton
		}
	}

	Rectangle
	{
		id: linksBox

		width : osfLoginBox.width
		height: 30 * preferencesModel.uiScale
		color : "transparent"

		anchors.top             : osfLoginBox.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin       : 5 * preferencesModel.uiScale

		Text {
			id: linkOSF

			text          :'<font color="#257bb2"><u>About the OSF</u></font>'
			textFormat    : Text.StyledText
			font.pointSize: 11 * preferencesModel.uiScale
			font.family:	Theme.font.family

			anchors.left      : parent.left
			anchors.bottom    : parent.bottom
			anchors.leftMargin: 5 * preferencesModel.uiScale

			MouseArea
			{
				anchors.fill: parent
				hoverEnabled: true
				cursorShape : Qt.PointingHandCursor
				onClicked   : Qt.openUrlExternally("http://help.osf.io")
			}
		}

		Text
		{
			id: linkRegister

			text          :'<font color="#257bb2"><u>Register</u></font>'
			textFormat    : Text.StyledText
			font.pointSize: 11 * preferencesModel.uiScale
			font.family: Theme.font.family

			anchors.bottom     : parent.bottom
			anchors.right      : parent.right
			anchors.rightMargin: 5 * preferencesModel.uiScale

			MouseArea
			{
				anchors.fill: parent
				hoverEnabled: true
				cursorShape : Qt.PointingHandCursor
				onClicked   : Qt.openUrlExternally("https://osf.io")
			}
		}
	}
}
