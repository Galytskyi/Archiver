import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls
import QtQuick.Layouts 1.12


Window
{
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    property int side_margin: 20

    function encode(file_name)
    {
        console.log("encode: " + file_name);
        workerThreads.appendEncodeFile(file_name);
    }

    function decode(file_name)
    {
        console.log("decode: " + file_name);
        workerThreads.appendDecodeFile(file_name);
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.leftMargin: side_margin
        anchors.rightMargin: side_margin
        anchors.topMargin: side_margin

        spacing: side_margin / 2

        RowLayout
        {
            anchors.fill: parent

            Label
            {
                text: "Filter for file list: "
            }

            ComboBox
            {
                id: cmbFilter
                Layout.fillWidth:  true
                font.pixelSize: 14

                model: filtersModel
                textRole: "display"

                onActivated:
                {
                    console.log("Set filter: " + cmbFilter.currentText);
                    tableModel.setFiles(cmbFilter.currentText);
                }
            }
        }

        Label
        {
            text: "File list"
            font.pointSize: 14
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView
        {
            id: listView
            //anchors.fill: parent

            Layout.fillWidth:  true
            Layout.fillHeight:  true

            ScrollBar.vertical: ScrollBar {}

            model: tableModel
            clip: true
            spacing: 2

            delegate: DelegateItem
            {
                id: delegateItem

                text: model.FileName;

                onBmpToBarch :
                {
                    bmpToBarch.connect(encode);
                }

                onBarchToBmp :
                {
                    barchToBmp.connect(decode);
                }
            }
        }

    }
}
