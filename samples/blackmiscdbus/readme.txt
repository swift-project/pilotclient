# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

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

-- Removed after changing to "handwritten" interface

# Causes nmake to run qdbusxml2cpp to automatically generate the dbus adaptor and interface classes,
# then automatically adds them to the sources to compile
# DBUS_ADAPTORS += BlackMiscTest.Testservice.xml BlackMiscTest.Datacontext.xml
# DBUS_INTERFACES += BlackMiscTest.Testservice.xml BlackMiscTest.Datacontext.xml
# DBUS_INTERFACES += BlackMiscTest.Datacontext.xml
# QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i blackmisc/blackmiscallvalueclasses.h
# QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i blackmisc/blackmiscallvalueclasses.h

