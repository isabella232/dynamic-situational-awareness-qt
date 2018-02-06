// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QtQuick1
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0

Rectangle {
    id: rootMarkup
    color: Material.primary
    opacity: 0.85

    property alias markupEnabled: markupController.drawModeEnabled

    // Modifying this array will change the initial available colors
    property var drawColors: ["#000000", "#ffffff", "#F44336", "#03a9f4", "#fff176"]

    // state strings
    property string drawState: "draw"
    property string editState: "edit"
    property string tocState: "toc"
    property string clearState: "clear"

    // to be emitted for UI purposes
    signal graphicsDeleted()
    signal colorSelected()
    signal colorDialogVisibleChanged(bool dialogVisible)

    onVisibleChanged: {
        if (visible) {
            state = drawState;
            markupController.active = true;
            markupController.drawModeEnabled = true;
        } else {
            markupController.drawModeEnabled = false;
            state = clearState;
        }
    }

    MarkupController {
        id: markupController

        onSketchComplete: nameDialog.open();
    }

    state: clearState
    states: [
        State {
            name: drawState
            PropertyChanges {
                target: markupController
                active: true
            }
            PropertyChanges {
                target: markupController
                drawModeEnabled: true
            }
        },
        State {
            name: editState
            PropertyChanges {
                target: editPane
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: 250 * scaleFactor
            }
            PropertyChanges {
                target: rootMarkup
                height: DsaStyles.mainToolbarHeight
            }
            PropertyChanges {
                target: markupController
                active: true
            }
            PropertyChanges {
                target: markupController
                drawModeEnabled: drawModeEnabled
            }
        },
        State {
            name: tocState
            PropertyChanges {
                target: editPane
                visible: false
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: drawer.width
            }
            PropertyChanges {
                target: rootMarkup
                height: drawer.height
            }
            PropertyChanges {
                target: markupController
                active: true
            }
            PropertyChanges {
                target: markupController
                drawModeEnabled: drawModeEnabled
            }
            PropertyChanges {
                target: markupToc
                visible: true
            }
        },
        State {
            name: clearState
            PropertyChanges {
                target: markupController
                drawModeEnabled: drawModeEnabled
            }
            PropertyChanges {
                target: markupController
                active: active
            }
            PropertyChanges {
                target: editPane
                visible: false
            }
            PropertyChanges {
                target: markupToc
                visible: false
            }
        }
    ]

    Item {
        id: editPane
        width : parent.width
        height: DsaStyles.mainToolbarHeight * scaleFactor
        visible: false

        DropShadow {
            anchors.fill: colorRect
            horizontalOffset: -1
            verticalOffset: 1
            radius: 8 * scaleFactor
            smooth: true
            samples: 16
            color: "#80000000"
            source: colorRect
        }

        Rectangle {
            id: colorRect
            anchors.fill: parent
            color: Material.background

            Text {
                id: colorTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 5 * scaleFactor
                }
                text: qsTr("Draw Color")
                color: Material.foreground
            }

            ListView {
                id: colorView
                anchors {
                    top: colorTitle.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 2 * scaleFactor
                }
                orientation: ListView.Horizontal
                model: colorModel
                height: 30 * scaleFactor
                width: 150 * scaleFactor
                spacing: 5 * scaleFactor
                currentIndex: 0
                snapMode: ListView.SnapOneItem

                delegate: Component {

                    Rectangle {
                        height: 30 * scaleFactor
                        width: height
                        radius: 100 * scaleFactor
                        color: drawColors[index]
                        border {
                            color: Material.accent
                            width: 0.50 * scaleFactor
                        }

                        Image {
                            anchors.centerIn: parent
                            height: parent.height
                            width: height
                            source: DsaResources.iconComplete
                            visible: selected
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                selectColor(parent);
                                parent.ListView.view.currentIndex = index;
                                selected = true;

                                if (markupController.drawModeEnabled)
                                    colorSelected();
                            }
                        }
                    }
                }

                ListModel {
                    id: colorModel
                }
            }

            // button for adding new colors
            RoundButton {
                id: addButton
                anchors {
                    margins: 5 * scaleFactor
                    right: parent.right
                    bottom: parent.bottom
                }
                visible: Qt.platform.os !== "android" // ColorDialog does not scale properly on Android
                height: 20 * scaleFactor
                width: height
                opacity: 0.95

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: implicitWidth
                    opacity: enabled ? 1 : 0.3
                    radius: addButton.radius
                    color: Material.accent

                    Image {
                        anchors.centerIn: parent
                        width: 16 * scaleFactor
                        height: width
                        source: DsaResources.iconAdd
                    }
                }

                onClicked: {
                    if (!newColorDialog.visible) {
                        colorDialogVisibleChanged(true);
                        newColorDialog.open();
                    }
                }
            }
        }
    }

    Item {
        id: markupToc
        anchors.fill: parent

        // TODO
    }

    Dialog {
        id: nameDialog
        property int i: 1
        onAccepted: {
            markupController.setName(nameText.text.length > 0 ? nameText.text : "sketch " + i);
            i++
        }

        Row {
            spacing: 5 * scaleFactor

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "Sketch Name:"
                color: Material.primary
                font {
                    family: DsaStyles.fontFamily
                    pixelSize: 12 * scaleFactor
                }
            }

            QtQuick1.TextField {
                id: nameText
                anchors.verticalCenter: parent.verticalCenter
                placeholderText: "sketch %1".arg(nameDialog.i)
            }
        }

        onVisibleChanged: {
            if (visible) {
                nameText.text = "";
                nameText.focus = true;
            }
        }
    }

    ColorDialog {
        id: newColorDialog
        title: "Choose a Draw Color"

        onAccepted: {
            colorDialogVisibleChanged(false);
            drawColors.push(color);
            colorModel.append({"selected": false});
            colorView.positionViewAtEnd();
        }
    }

    // calls into C++ to create a new SimpleLineSymbol with the desired color
    function selectColor(colorRectangle) {
        colorModel.setProperty(colorView.currentIndex, "selected", false);
        markupController.setColor(colorRectangle.color);
    }

    // initialize the ListModel with the initial draw colors
    Component.onCompleted: {
        for (var i = 0; i < drawColors.length; i++)
            colorModel.append({"drawColor": drawColors[i], "selected": false});

        markupController.setColor(drawColors[0]);
        colorModel.setProperty(colorView.currentIndex, "selected", true);
    }
}
