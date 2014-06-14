/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_XBUS_PLUGIN_H
#define BLACKSIM_XBUS_PLUGIN_H

//! \file

//! \namespace XBus

#include <QObject>

namespace XBus
{
    /*!
     * Main plugin class
     */
    class CPlugin : public QObject
    {
        Q_OBJECT

    public:
        CPlugin();

    private:
    };
}

#endif // guard
