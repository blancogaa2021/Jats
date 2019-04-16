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
import QtGraphicalEffects 1.12
import JASP.Theme 1.0

Item
{
	id					: menu
	width				: menuRectangle.width
	height				: menuRectangle.height
	property var props	: undefined
	property bool hasIcons: true

	onPropsChanged:
	{
		hasIcons = (menu.props === undefined || "undefined" === typeof(menu.props["hasIcons"])) ? true : menu.props["hasIcons"]
	}

	function resizeElements(newWidth)
	{
		for (var i = 0; i < repeater.count; ++i) {
			repeater.itemAt(i).width = newWidth;
		}
	}

	Rectangle
	{
		id		: menuRectangle
		z		: menuShadow.z + 1
		color	: Theme.white
	}

	Column
	{
		id		: column
		x		: menuRectangle.x
		y		: menuRectangle.y + (Theme.menuPadding / 2)
		z		: menuRectangle.z + 1
		spacing	: Theme.menuSpacing

		Repeater
		{
			id		: repeater
            model	: menu.props === undefined ? undefined : menu.props["model"]

			onItemAdded:
			{
				if (index === 0) {
					menuRectangle.width = 0;
					menuRectangle.height = 0;
				}

				menuRectangle.width = Math.max(item.width, menuRectangle.width);
				menuRectangle.height += (item.height + Theme.menuSpacing)

				if (index === count - 1) {
					menuRectangle.height += (Theme.menuPadding - Theme.menuSpacing)
					menu.resizeElements(menuRectangle.width);
				}
			}

			delegate: Loader
			{
				sourceComponent :
				{
					if (model.isSeparator)
						return menuSeparator;
					else if (model.isGroupTitle)
						return menuGroupTitle;
					return menuDelegate
				}

				Component
				{
					id: menuDelegate

					Rectangle
					{
						id		: menuItem
						width	: initWidth
						height	: Theme.menuItemHeight
						color	: mouseArea.pressed ? Theme.blueMuchLighter : mouseArea.containsMouse ? Theme.grayLighter : Theme.white

						property double initWidth: (menu.hasIcons ? menuItemImage.width : 0) + menuItemText.implicitWidth + (menu.hasIcons ? 15 : 10) * Theme.uiScale
						// 15 = menuItemImage.leftMargin + menuItemText.leftMargin + menuItemText.rightMargin + menuItemImage.smallerBy

						Rectangle
						{
							id		: menuItemImage
							height	: menuItem.height - 5 * Theme.uiScale // 5 = smallerBy
							width	: menuItem.height - 5 * Theme.uiScale
							color	: menuItem.color

							anchors.left			: parent.left
							anchors.leftMargin		: 5 * Theme.uiScale
							anchors.verticalCenter	: parent.verticalCenter

							Image
							{
								visible		: menu.hasIcons
								height		: parent.height
								width		: parent.width
								source		: menuImageSource
								fillMode	: Image.PreserveAspectFit
							}
						}

						Text
						{
							id					: menuItemText
							text				: displayText
							height				: menuItem.height
							font				: Theme.font
							verticalAlignment	: Text.AlignVCenter

							anchors.left		: menu.hasIcons ? menuItemImage.right : parent.left
							anchors.leftMargin	: 5 * Theme.uiScale
						}

						MouseArea
						{
							id				: mouseArea
							hoverEnabled	: true
							anchors.fill	: parent
							onClicked		:
							{
								menu.props['functionCall'](index)
							}
						}
					}
				}
				Component
				{
					id: menuGroupTitle

					Rectangle
					{
						id		: menuItem
						width	: initWidth
						height	: Theme.menuItemHeight
						color	: Theme.white

						property double initWidth: menuItemImage.width + menuItemText.implicitWidth + 15 * Theme.uiScale


						Rectangle
						{
							id		: menuItemImage
							height	: menuItem.height - 5 * Theme.uiScale // 5 = smallerBy
							width	: menuItem.height - 5 * Theme.uiScale
							color	: Theme.white

							anchors.left			: parent.left
							anchors.leftMargin		: 5 * Theme.uiScale
							anchors.verticalCenter	: parent.verticalCenter

							Image
							{
								height		: parent.height
								width		: parent.width
								source		: menuImageSource
								fillMode	: Image.PreserveAspectFit
							}
						}

						Text
						{
							id					: menuItemText
							text				: displayText
							font				: Theme.fontLabel
							anchors.left		: menuImageSource ? menuItemImage.right : menuItem.left
							anchors.leftMargin	: 5 * Theme.uiScale
							anchors.verticalCenter	: parent.verticalCenter
						}
					}
				}

				Component
				{
					id	: menuSeparator

					ToolSeparator
					{
						orientation	: Qt.Horizontal
					}
				}
			}
		}
	}

	RectangularGlow
	{
		id				: menuShadow
		anchors.fill	: menuRectangle

		color			: Theme.grayDarker
		spread			: 0.2
		cornerRadius	: menuRectangle.radius + glowRadius
		glowRadius		: 5
	}
}
