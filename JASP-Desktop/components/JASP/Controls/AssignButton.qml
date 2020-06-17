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


import QtQuick		2.11

Button
{
	id:					button
	focusOnTab:			false

				property var	leftSource
				property var	rightSource
				property bool	leftToRight:	true

				property var	source:			leftToRight ? leftSource : rightSource
				property var	target:			leftToRight ? rightSource : leftSource

				property var	interactionControl

	readonly	property string iconToLeft:		jaspTheme.iconPath + "arrow-left.png"
	readonly	property string iconToRight:	jaspTheme.iconPath + "arrow-right.png"
	
	text:			""
	enabled:		false
	iconSource:		leftToRight ? iconToRight : iconToLeft

	control.buttonPadding:	2
	control.implicitWidth:	40 * preferencesModel.uiScale
	control.implicitHeight: 20 * preferencesModel.uiScale

	onClicked:		source.moveSelectedItems(target)

	function setIconToRight()	{ if (leftSource.activeFocus)	leftToRight = true;		}
	function setIconToLeft()	{ if (rightSource.activeFocus)	leftToRight = false;	}
	function setState()
	{
		var result = source.enabled && target.enabled;
		if (result && source.model && source.model.selectedItems().length > 0)
		{
			if (target.allowedColumns.length > 0)
			{
				result = false;
				var sourceSelectedItemsTypes = source.model.selectedItemsTypes()
				for (var i = 0; i < sourceSelectedItemsTypes.length; i++)
				{
					var itemType = sourceSelectedItemsTypes[i];
					if (target.allowedColumns.includes(itemType))
						result = true;
				}
			}
		}

		if (result && interactionControl)
		{
			if (target.addInteractionOptions && source.model)
			{
				var nb = source.model.selectedItems().length
				interactionControl.enabled = result
				var enabledOptions = [ true, nb > 1, nb > 2, nb > 3, nb > 4, true ]
				interactionControl.enabledOptions = enabledOptions
				if (!enabledOptions[interactionControl.currentIndex])
					result = false;
			}
			else
				interactionControl.enabled = false
		}
		
		enabled = result
		
	}

	onSourceChanged:	setState()

}
