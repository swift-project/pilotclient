#ifndef SAMPLEPLUGIN_H
#define SAMPLEPLUGIN_H

#include <QObject>
#include <QDebug>
#include <QDBusMetaType>
#include <QMetaType>
#include "../blackmiscquantities_dbus/dummy.h"
#include "../blackmiscquantities_dbus/dummynoq.h"

class CXmlSamplePlugin : public QObject, public QDBusCpp2XmlPlugin
{
    Q_OBJECT
    Q_INTERFACES(QDBusCpp2XmlPlugin)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.DBus.Cpp2XmlPlugin")

public:
    virtual void registerMetaTypes();
};

#endif // SAMPLEPLUGIN_H
