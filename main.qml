import QtQuick 2.15
import QtQuick.Controls 2.15
import com.example.backend 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Связь C++ и QML"

    Backend {
        id: backend
    }

    Column {
        anchors.centerIn: parent
        spacing: 10

        Text {
            text: backend.message
            font.pixelSize: 20
        }

        Button {
            text: "Изменить сообщение"
            onClicked: {
                backend.message = "Сообщение изменено!"
            }
        }
    }
}

