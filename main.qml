import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Gauss - Jordan elimination method")

    Canvas {
        id: mycanvas
        z: 2
        width: mainWindow.width
        height: mainWindow.height

        property real height_pct: 0.35
        property real width_pct: 0.7

        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = "#03A9F4";
            ctx.beginPath();
            ctx.moveTo(mainWindow.width * width_pct, 0);
            ctx.lineTo(mainWindow.width, 0);
            ctx.lineTo(mainWindow.width,mainWindow.height * height_pct);
            ctx.closePath();
            ctx.shadowColor = "#000000";
            ctx.shadowOffsetX = -2;
            ctx.shadowOffsetY = 2;
            ctx.shadowBlur = 5.0;
            ctx.fill();
        }
    }

    Column {
        z: 1
        width: mainWindow.width
        height: mainWindow.height


        Item {
            id: mainTitle
            width: parent.width * 0.7
            height: 100

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "Gauss - Jordan elimination"
                color: "#03A9F4"
                font.pixelSize: 32
                font.bold: true
            }
        }



        Item {
            width: parent.width * 0.85
            height: parent.height * 0.6 - mainTitle.height - arithmetic_wrapper.height

            Subtitle {
                text: "Results:"
            }

            Flickable {
                id: flickable_results
                anchors.fill: parent
                anchors.margins: 20
                anchors.topMargin: 30
                anchors.bottomMargin: 20

                TextArea.flickable: TextArea {
                    id: results
                    wrapMode: TextArea.Wrap
                    textMargin: 10
                    selectByMouse: true
                    readOnly: true
                    text: arithmetic.checked ? solver.results_interval() : solver.results_standard()

                    background: Rectangle {
                        anchors.fill: parent
                        color: results.focus ? "#03A9F4" : "#616161"

                        Rectangle {
                            anchors.fill: parent
                            anchors.bottomMargin: 2
                            color: "#212121"
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar { }
            }
        }

        Row {
            id: arithmetic_wrapper
            anchors.margins: 40
            height: 80
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            Button {
                id: sMatrix
                text: "Standard Matrix"
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    results.text = solver.print_sMatrix()
                }
            }

            Button {
                id: iMatrix
                text: "Interval Matrix"
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    results.text = solver.print_iMatrix()
                }
            }

            Row {
                anchors.verticalCenter: parent.verticalCenter

                Label {
                    text: "Standard"
                    color: arithmetic.checked ? "#9E9E9E" : "#FAFAFA"
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: !arithmetic.checked
                }

                Switch {
                    id: arithmetic
                    checked: false

                    onClicked: {
                        results.text = arithmetic.checked ? solver.results_interval() : solver.results_standard()
                    }
                }

                Label {
                    text: "Interval"
                    color: arithmetic.checked ? "#FAFAFA" : "#9E9E9E"
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: arithmetic.checked
                }
            }
        }

        Item {
            width: parent.width
            height: parent.height * 0.4 - compute.height - 20

            Subtitle {
                text: "Data:"
            }

            Flickable {
                id: flickable_factors
                anchors.fill: parent
                anchors.margins: 20
                anchors.topMargin: 30
                anchors.bottomMargin: 10

                TextArea.flickable: TextArea {
                    id: factors
                    wrapMode: TextArea.Wrap
                    textMargin: 10
                    selectByMouse: true

                    background: Rectangle {
                        anchors.fill: parent
                        color: factors.focus ? "#03A9F4" : "#616161"

                        Rectangle {
                            anchors.fill: parent
                            anchors.bottomMargin: 2
                            color: "#212121"
                        }

                        Text {
                            anchors.fill: parent
                            anchors.margins: 10
                            anchors.topMargin: 20
                            text: "Enter data here... \nPattern: <number | interval>_<number | interval>=<number | interval>;\n\nExample: \n4_8_-5=4; \n[6.66, 12.34]_+0.22_-5.2=2; \n2.12_3.0_5.1=[1, 2];"
                            color: "#616161"
                            visible: !factors.text && !factors.focus
                            font.pixelSize: 14
                            lineHeight: 1.25
                        }
                    }

                }

                ScrollBar.vertical: ScrollBar { }
            }
        }

        Button {
            id: compute
            text: "Compute"
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                solver.pass_data(factors.text)
                results.text = arithmetic.checked ? solver.results_interval() : solver.results_standard()
            }
        }
    }
}
