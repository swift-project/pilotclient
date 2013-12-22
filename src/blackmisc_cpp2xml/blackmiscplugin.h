/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_CPP2XML_H
#define BLACKMISC_CPP2XML_H

#include <QDBusMetaType>
#include <QMetaType>

/*!
 * Plugin to register all relevant blackmisc classes for QDBusCpp2XmlPlugin
 * This needs to be recompiled whenever a new DBus compliant class has been created.
 * Compile as "release", otherwise plugin will not work.
 */
class BlackmiscPlugin : public QObject, public QDBusCpp2XmlPlugin
{
    Q_OBJECT
    Q_INTERFACES(QDBusCpp2XmlPlugin)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.DBus.Cpp2XmlPlugin")

public:
    /*!
     * \brief Register Metatypes
     */
    virtual void registerMetaTypes();
};

#endif // guard
