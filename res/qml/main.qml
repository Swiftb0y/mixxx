import "." as Skin
import Mixxx 0.1 as Mixxx
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.11
import "Theme"
import "test" as Test

ApplicationWindow {
    // I work flawlessly
    // Test.ControlButton {
    //     // group: "[Channel1]"
    //     // key: "pfl"
    //     // activeColor: "yellow"
    // }

    id: root

    width: 1920
    height: 1080
    visible: true

    // I'm broken sometimes
    Skin.ControlButton {
        // group: "[Channel1]"
        // key: "pfl"
        // activeColor: "yellow"

    }

}
