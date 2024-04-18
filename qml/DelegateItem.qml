import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls
import QtQuick.Layouts 1.12

Page
{
    id: root
    width: listView.width
    height: lblFile.height * 4

    property alias text: lblFile.text

    // signals
    //
    signal bmpToBarch(string file_name);
    signal barchToBmp(string file_name);

    // functions
    //
    function getExt(file_name)
    {
        return (file_name.slice(file_name.lastIndexOf(".")+1))
    }

    function enableVisible(file_name, ext)
    {
        if (ext === getExt(file_name.toString()))
            return true;

        return false;
    }

    // controls
    //
    background: Rectangle
    {
        id: bgRect
        color: "white"
        border.color: "grey"
        anchors.fill: root
        radius: 10
    }

    RowLayout
    {
        id: layout
        anchors.fill: parent
        anchors.margins: 20

        Text
        {
            id: lblFile

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        Button
        {
            id: btnBmpToBarch
            Layout.alignment: Qt.AlignVCenter
            visible: enableVisible(lblFile.text, "bmp")

            text: " Bmp to Barch"
            icon.source: "icons/Encode.png"

            onClicked:
            {
                 bmpToBarch(lblFile.text);
            }
        }

        Button
        {
            id: btnBarchToBmp
            Layout.alignment: Qt.AlignVCenter
            visible: enableVisible(lblFile.text, "barch")

            text: " Barch to Bmp"
            icon.source: "icons/Decode.png"

            onClicked:
            {
                barchToBmp(lblFile.text);
            }
        }
    }
}
