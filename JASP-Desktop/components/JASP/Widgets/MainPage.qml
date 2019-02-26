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
import QtWebEngine		1.7
import QtWebChannel		1.0
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4 as OLD
import QtQuick.Layouts	1.3
import JASP.Theme		1.0
import JASP.Widgets		1.0

OLD.SplitView
{
	id:				panelSplit
	orientation:	Qt.Horizontal

	property bool shouldShowInputOutput: !mainWindow.dataAvailable || mainWindow.analysesAvailable

	onWidthChanged:
	{
		if(!panelSplit.shouldShowInputOutput)							data.width = panelSplit.width
		else if(panelSplit.width < data.width + Theme.splitHandleWidth)	data.width = panelSplit.width - Theme.splitHandleWidth
	}

	DataPanel
	{
		id:						data
		visible:				mainWindow.dataAvailable //|| analysesModel.count > 0
		z:						1
		property real maxWidth: panelSplit.width - (mainWindow.analysesAvailable ? Theme.splitHandleWidth : 0)

		onWidthChanged:
		{
			var iAmBig = width > 0;
			if(iAmBig !== mainWindow.dataPanelVisible)
				mainWindow.dataPanelVisible = iAmBig
		}

		function maximizeData()
		{
			data.width = data.maxWidth
		}

		Connections
		{
			target:						mainWindow
			onDataPanelVisibleChanged:	if(mainWindow.dataPanelVisible)		data.maximizeData(); else data.width = 0
		}
	}


	handleDelegate: SplitHandle
	{
		onArrowClicked:	mainWindow.dataPanelVisible = !mainWindow.dataPanelVisible
		pointingLeft:	mainWindow.dataPanelVisible
		showArrow:		true
		toolTipArrow:	mainWindow.dataPanelVisible ? "Hide data"   : "Maximize data"
		toolTipDrag:	mainWindow.dataPanelVisible ? "Resize data" : "Drag to show data"
	}


	Item
	{
		id:						inputOutput
		implicitWidth:			Theme.resultWidth
		//Layout.minimumWidth:	Math.max(Theme.minPanelWidth, analyses.width)
		Layout.fillWidth:		true
		z:						3
		visible:				panelSplit.shouldShowInputOutput

		onVisibleChanged:		if(visible) width = Theme.resultWidth; else data.maximizeData()

		property int minimumFullWidth: Theme.resultWidth + Theme.formWidth

		Connections
		{
			target:				analysesModel
			onAnalysisAdded:	if(inputOutput.width <= 10)									mainWindow.dataPanelVisible = false;
								else if(inputOutput.width < inputOutput.minimumFullWidth)	inputOutput.width			= inputOutput.minimumFullWidth
		}

		AnalysisForms
		{
			id:				analyses
			z:				-1
			visible:		mainWindow.analysesAvailable

			anchors
			{
				top:		parent.top
				left:		parent.left
				bottom:		parent.bottom
			}
		}

		WebEngineView
		{
			id:						resultsView
			anchors.fill:			parent
			anchors.leftMargin:		analyses.width
			url:					resultsJsInterface.resultsPageUrl
			onLoadingChanged:		resultsJsInterface.resultsPageLoaded(loadRequest.status === WebEngineLoadRequest.LoadSucceededStatus)
			onContextMenuRequested: request.accepted = true

			Connections
			{
				target:				resultsJsInterface
				onRunJavaScript:	resultsView.runJavaScript(js)
			}

			webChannel.registeredObjects: [ resultsJsInterfaceInterface ]

			Item
			{
				id:				resultsJsInterfaceInterface
				WebChannel.id:	"jasp"

				// Yeah I know this "resultsJsInterfaceInterface" looks a bit stupid but this honestly seems like the best way to make the current resultsJsInterface functions available to javascript without rewriting (more of) the structure of JASP-Desktop right now.
				// It would be much better to have resultsJsInterface be passed irectly though..
				// It also gives you an overview of the functions used in results html

				function openFileTab()							{ resultsJsInterface.openFileTab()							}
				function saveTextToFile(fileName, html)			{ resultsJsInterface.saveTextToFile(fileName, html)			}
				function analysisUnselected()					{ resultsJsInterface.analysisUnselected()					}
				function analysisSelected(id)					{ resultsJsInterface.analysisSelected(id)					}
				function analysisChangedDownstream(id, model)	{ resultsJsInterface.analysisChangedDownstream(id, model)	}

				function showAnalysesMenu(options)
				{
					// FIXME: This is a mess
					// TODO:  1. remove redundant computations
					//        2. move everything to one place :P

					var optionsJSON  = JSON.parse(options);
					var functionCall = function (index)
					{
						customMenu.visible = false;
						var name = customMenu.props['model'].getName(index);

						if (name === 'hasRefreshAllAnalyses') {
							resultsJsInterface.refreshAllAnalyses();
							return;
						}

						if (name === 'hasRemoveAllAnalyses') {
							resultsJsInterface.removeAllAnalyses();
							return;
						}

						if (name === 'hasCopy' || name === 'hasCite') {
							resultsJsInterface.purgeClipboard();
						}

						resultsJsInterface.runJavaScript(customMenu.props['model'].getJSFunction(index));

						if (name === 'hasEditTitle' || name === 'hasNotes') {
							resultsJsInterface.packageModified();
						}
					}

					var selectedOptions = []
					for (var key in optionsJSON) {
						if (optionsJSON.hasOwnProperty(key))
							if (optionsJSON[key] === true)
								selectedOptions.push(key)
					}
					resultMenuModel.setOptions(options, selectedOptions);

					var props = {
						"model"			: resultMenuModel,
						"functionCall"	: functionCall
					};

					customMenu.showMenu(resultsView, props, optionsJSON['rXright'] + 10, optionsJSON['rY']);
				}

				function updateUserData(id, key)				{ resultsJsInterface.updateUserData(id, key)				}
				function analysisSaveImage(id, options)			{ resultsJsInterface.analysisSaveImage(id, options)			}
				function analysisEditImage(id, options)			{ resultsJsInterface.analysisEditImage(id, options)			}
				function removeAnalysisRequest(id)				{ resultsJsInterface.removeAnalysisRequest(id)				}
				function pushToClipboard(mime, raw, coded)		{ resultsJsInterface.pushToClipboard(mime, raw, coded)		}
				function pushImageToClipboard(raw, coded)		{ resultsJsInterface.pushImageToClipboard(raw, coded)		}
				function simulatedMouseClick(x, y, count)		{ resultsJsInterface.simulatedMouseClick(x, y, count)		}
				function saveTempImage(index, path, base64)		{ resultsJsInterface.saveTempImage(index, path, base64)		}
				function getImageInBase64(index, path)			{ resultsJsInterface.getImageInBase64(index, path)			}
				function resultsDocumentChanged()				{ resultsJsInterface.resultsDocumentChanged()				}
				function displayMessageFromResults(msg)			{ resultsJsInterface.displayMessageFromResults(msg)			}
				function setAllUserDataFromJavascript(json)		{ resultsJsInterface.setAllUserDataFromJavascript(json)		}
				function setResultsMetaFromJavascript(json)		{ resultsJsInterface.setResultsMetaFromJavascript(json)		}
			}
		}
	}

}
