import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle
{
    color: systemPalette.midlight
    visible: false
    anchors.left: rootDataset.left
    anchors.right: rootDataset.right
    anchors.top: rootDataset.top

    Text
    {
        id: loadingText
        horizontalAlignment: Text.AlignHCenter
        text: "Load some data or a .jasp file"
		font.pixelSize: baseFontSize * ppiScale

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
    }


    ProgressBar
    {
        id: loadingBar
        minimumValue: 0
        maximumValue: 100
        value: 0


        anchors.left: progressBarHolder.left
        anchors.right: progressBarHolder.right
        anchors.top: loadingText.top
        anchors.topMargin: 24
    }

    function show()
    {
        setStatus("", 0)
        visible = true
        splitViewData.visible = false
    }

    function hide()
    {
        visible = false
        splitViewData.visible = true
    }

    function setStatus(statusString, progress)
    {
        loadingText.text = statusString
        loadingBar.value = progress

    }

}
