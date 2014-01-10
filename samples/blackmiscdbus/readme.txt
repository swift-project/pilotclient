Starting: dbus-daemon.exe --session
- blocks CMD (sometimes daemon continues to run when pressing CTRL/C)
- does not start without directory session.d, i.e. ..\Qt\Qt5.1.0DBus\qtbase\etc\dbus-1\session.d

!!! The includes are set in the qmake file, there are the header files
!!! which are used in the interface
qdbuscpp2xml testservice.h -x blackmisc_cpp2xml.dll -o BlackMiscTest.Testservice.xml
qdbuscpp2xml datacontext.h -x blackmisc_cpp2xml.dll -o BlackMiscTest.Datacontext.xml
Set search path for plugins: env.var. QT_PLUGIN_PATH

Done automatically (qmake), but if required manually
Interface: qdbusxml2cpp blackbus.testservice.xml -p itestservice
Adaptor: qdbusxml2cpp blackbus.testservice.xml -a atestservice
