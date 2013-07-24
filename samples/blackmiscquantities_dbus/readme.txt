Starting: dbus-daemon.exe --session
- blocks CMD (sometimes daemon continues to run when pressing CTRL/C)
- does not start without directory session.d, i.e. ..\Qt\Qt5.1.0DBus\qtbase\etc\dbus-1\session.d

qdbuscpp2xml testservice.h -x blackmisc_cpp2xml.dll -o BlackMiscTest.Testservice.xml
Set search path for plugins: env.var. QT_PLUGIN_PATH

Done automatically, but if required manually
Interface: qdbusxml2cpp blackbus.testservice.xml -p itestservice
Adaptor: qdbusxml2cpp blackbus.testservice.xml -a atestservice
