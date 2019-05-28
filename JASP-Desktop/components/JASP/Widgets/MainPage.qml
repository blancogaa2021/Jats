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
import JASP.Controls	1.0

Item
{
	id: splitViewContainer

	onWidthChanged:
	{
		if(!panelSplit.shouldShowInputOutput)										data.width = splitViewContainer.width
		else if(data.wasMaximized)													return; //wasMaximized binds!
		else if(splitViewContainer.width <= data.width + Theme.splitHandleWidth)	data.maximizeData();
	}

	OLD.SplitView
	{
		id:				panelSplit
		orientation:	Qt.Horizontal
		height:			parent.height
		width:			parent.width + hackySplitHandlerHideWidth

		//hackySplitHandlerHideWidth is there to create some extra space on the right side for the analysisforms I put inside the splithandle. https://github.com/jasp-stats/INTERNAL-jasp/issues/144
		property int  hackySplitHandlerHideWidth:	(panelSplit.shouldShowInputOutput && analysesModel.visible ? Theme.formWidth + 3 + Theme.scrollbarBoxWidth : 0) + ( mainWindow.analysesAvailable ? Theme.splitHandleWidth : 0 )
		property bool shouldShowInputOutput:		(!mainWindow.progressBarVisible && !mainWindow.dataAvailable) || mainWindow.analysesAvailable

		DataPanel
		{
			id:						data
			visible:				mainWindow.progressBarVisible || mainWindow.dataAvailable || fakeEmptyDataForSumStatsEtc //|| analysesModel.count > 0
			z:						1

			property real maxWidth:						fakeEmptyDataForSumStatsEtc ? 0 : splitViewContainer.width - (mainWindow.analysesAvailable ? Theme.splitHandleWidth : 0)
			property bool fakeEmptyDataForSumStatsEtc:	!mainWindow.dataAvailable && mainWindow.analysesAvailable
			property bool wasMaximized:					false

			onWidthChanged:
			{
				if(!mainWindow.progressBarVisible)
				{
					var iAmBig = width > 0;
					 if(iAmBig !== mainWindow.dataPanelVisible)
						mainWindow.dataPanelVisible = iAmBig

					if(fakeEmptyDataForSumStatsEtc)
					{
						mainWindow.dataPanelVisible = false;
						width = 0;
					}
				}

				if(data.width !== data.maxWidth)
					data.wasMaximized = false;
			}

			function maximizeData()	{ data.width = Qt.binding(function() { return data.maxWidth; });	data.wasMaximized = true; }
			function minimizeData()	{ data.width = 0;													data.wasMaximized = false; }

			Connections
			{
				target:						mainWindow
				onDataPanelVisibleChanged:
				{
					if(mainWindow.dataPanelVisible && data.width === 0)		data.maximizeData();
					else if(!mainWindow.dataPanelVisible && data.width > 0)	data.minimizeData();
				}
			}
		}


		handleDelegate: Item
		{
			width:				splitHandle.width + analyses.width
			//onWidthChanged:		parent.width = width

			SplitHandle
			{
				id:				splitHandle
				onArrowClicked:	mainWindow.dataPanelVisible = !mainWindow.dataPanelVisible
				pointingLeft:	mainWindow.dataPanelVisible
				showArrow:		mainWindow.dataAvailable
				toolTipArrow:	mainWindow.dataAvailable	? (mainWindow.dataPanelVisible ? "Resize data"  : "Drag to show data") : ""
				toolTipDrag:	mainWindow.dataPanelVisible ? "Resize data" : "Drag to show data"
				dragEnabled:	mainWindow.dataAvailable && mainWindow.analysesAvailable
			}

			AnalysisForms //This is placed inside the splithandle so that you can drag on both sides of it. Not the most obvious path to take but the only viable one. https://github.com/jasp-stats/INTERNAL-jasp/issues/144
			{
				id:						analyses
				z:						-1
				visible:				panelSplit.shouldShowInputOutput && mainWindow.analysesAvailable
				width:					visible ? implicitWidth : 0
				anchors.top:			parent.top
				anchors.bottom:			parent.bottom
				anchors.left:			splitHandle.right
			}
		}

		Rectangle
		{
			id:						giveResultsSomeSpace
			implicitWidth:			Theme.resultWidth + panelSplit.hackySplitHandlerHideWidth
			Layout.fillWidth:		true
			z:						3
			visible:				panelSplit.shouldShowInputOutput
			onVisibleChanged:		if(visible) width = Theme.resultWidth; else data.maximizeData()
			color:					analysesModel.currentAnalysisIndex !== -1 ? Theme.uiBackground : Theme.white

			Connections
			{
				target:				analysesModel
				onAnalysisAdded:
				{
					//make sure we get to see the results!

					var inputOutputWidth	= splitViewContainer.width - (data.width + Theme.splitHandleWidth)
					var remainingDataWidth	= Math.max(0, data.width - (Theme.splitHandleWidth + Theme.resultWidth));

					if(inputOutputWidth < 100 * preferencesModel.uiScale)
						 mainWindow.dataPanelVisible = false;
					else if(inputOutputWidth < Theme.resultWidth)
					{
						if(remainingDataWidth === 0)	mainWindow.dataPanelVisible = false;
						else							data.width = remainingDataWidth
					}
				}
			}

			WebEngineView
			{
				id:						resultsView

				anchors
				{
					top:				parent.top
					left:				parent.left
					bottom:				parent.bottom
				}

				width: giveResultsSomeSpace.width - panelSplit.hackySplitHandlerHideWidth

				url:					resultsJsInterface.resultsPageUrl
				onLoadingChanged:		resultsJsInterface.resultsPageLoaded(loadRequest.status === WebEngineLoadRequest.LoadSucceededStatus);
				onContextMenuRequested: request.accepted = true
				onNavigationRequested:	request.action = request.navigationType === WebEngineNavigationRequest.ReloadNavigation || request.url == resultsJsInterface.resultsPageUrl ? WebEngineNavigationRequest.AcceptRequest : WebEngineNavigationRequest.IgnoreRequest

				Connections
				{
					target:					resultsJsInterface
					onRunJavaScript:		resultsView.runJavaScript(js)
				}

				webChannel.registeredObjects: [ resultsJsInterfaceInterface ]

				Item
				{
					id:				resultsJsInterfaceInterface
					WebChannel.id:	"jasp"

					// Yeah I know this "resultsJsInterfaceInterface" looks a bit stupid but this honestly seems like the best way to make the current resultsJsInterface functions available to javascript without rewriting (more of) the structure of JASP-Desktop right now.
					// It would be much better to have resultsJsInterface be passed directly though..
					// It also gives you an overview of the functions used in results html

					function openFileTab()								{ resultsJsInterface.openFileTab()                              }
					function saveTextToFile(fileName, html)				{ resultsJsInterface.saveTextToFile(fileName, html)             }
					function analysisUnselected()						{ resultsJsInterface.analysisUnselected()                       }
					function analysisSelected(id)						{ resultsJsInterface.analysisSelected(id)                       }
					function analysisChangedDownstream(id, model)		{ resultsJsInterface.analysisChangedDownstream(id, model)       }
					function analysisTitleChangedFromResults(id, title)	{ resultsJsInterface.analysisTitleChangedFromResults(id, title) }


					function showAnalysesMenu(options)
					{
						// FIXME: This is a mess
						// TODO:  1. remove redundant computations
						//        2. move everything to one place :P

						var optionsJSON  = JSON.parse(options);
						var functionCall = function (index)
						{
							var name		= customMenu.props['model'].getName(index);
							var jsfunction	= customMenu.props['model'].getJSFunction(index);
							
							customMenu.remove()

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

							resultsJsInterface.runJavaScript(jsfunction);

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

						customMenu.showMenu(resultsView, props, (optionsJSON['rXright'] + 10) * preferencesModel.uiScale, optionsJSON['rY'] * preferencesModel.uiScale);
					}

					function updateUserData()						{ resultsJsInterface.updateUserData()						}
					function analysisSaveImage(id, options)			{ resultsJsInterface.analysisSaveImage(id, options)			}
					function analysisEditImage(id, options)			{ resultsJsInterface.analysisEditImage(id, options)			}
					function removeAnalysisRequest(id)				{ resultsJsInterface.removeAnalysisRequest(id)				}
					function pushToClipboard(mime, raw, coded)		{ resultsJsInterface.pushToClipboard(mime, raw, coded)		}
					function pushImageToClipboard(raw, coded)		{ resultsJsInterface.pushImageToClipboard(raw, coded)		}
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
}
