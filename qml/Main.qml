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

    Page
    {
        id: page
        anchors.fill: parent

        ListView
        {
            id: listView
            anchors.fill: parent
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
