Starting: dbus-daemon.exe --session
- blocks CMD (sometimes daemon continues to run when pressing CTRL/C)
- does not start without directory session.d, i.e. ..\Qt\Qt5.1.0DBus\qtbase\etc\dbus-1\session.d

!!! The includes are set in the qmake file, there are the header files
!!! which are used in the interface
/// qdbuscpp2xml context_network.h -x blackmisc_cpp2xml.dll -o blackcore.contextnetwork.xml
qdbuscpp2xml context_network_interface.h -x blackmisc_cpp2xml.dll -o blackcore.contextnetwork.xml
qdbuscpp2xml context_settings_interface.h -x blackmisc_cpp2xml.dll -o blackcore.contextsettings.xml
qdbuscpp2xml context_application_interface.h -x blackmisc_cpp2xml.dll -o blackcore.contextapplication.xml
qdbuscpp2xml context_voice_interface.h -x blackmisc_cpp2xml.dll -o blackcore.contextvoice.xml

Set search path for plugins: env.var. QT_PLUGIN_PATH

Done automatically (qmake), but if required manually
Interface:
qdbusxml2cpp -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h -p contextnetwork_interface.h: H:\Projects\Qt\VatsimClient\client\src\blackcore\blackcore.contextnetwork.xml
qdbusxml2cpp -i contextnetwork_interface.h -p :contextnetwork_interface.cpp H:\Projects\Qt\VatsimClient\client\src\blackcore\blackcore.contextnetwork.xml

Adaptor: The : indicates generation of cpp file
qdbusxml2cpp -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h -a contextnetwork_adaptor.h blackcore.contextnetwork.xml
qdbusxml2cpp -i context_adaptor.h -a :contextnetwork_adaptor.cpp blackcore.contextnetwork.xml

Manually:
<signal name="statusMessage">
<signal name="textMessagesReceived">
