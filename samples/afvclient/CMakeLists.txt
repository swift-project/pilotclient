add_executable(samples_afvclient main.cpp afvclientbridge.h afvclientbridge.cpp qml/qml.qrc)
target_link_libraries(samples_afvclient PUBLIC core misc blackconfig Qt::Core Qt::Qml Qt::Gui)
