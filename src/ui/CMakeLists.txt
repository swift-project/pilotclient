add_executable(swift
        main.cpp
        QmlContextNetwork.cpp
        QmlContextNetwork.h
        QmlContextSimulator.h
        PageController.cpp
        resources.qrc
        SimSelection.h
)

target_link_libraries(swift PUBLIC core misc config Qt::Core Qt::Qml Qt::Gui)

