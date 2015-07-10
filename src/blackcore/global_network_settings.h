/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_GLOBAL_NETWORKSETTINGS_H
#define BLACKCORE_GLOBAL_NETWORKSETTINGS_H

#include "blackmisc/network/server.h"
#include "blackcoreexport.h"
#include <QStringList>
#include <QObject>

namespace BlackCore
{
    //! Settings for readers
    class BLACKCORE_EXPORT CGlobalNetworkSettings : public QObject
    {
        Q_OBJECT

    public:
        //! FSD Server
        const BlackMisc::Network::CServer &swiftFSDTestServer() const { return m_fsdSwiftServer; }

        //! Singleton
        static const CGlobalNetworkSettings &instance();

    private:
        //! Default constructor
        CGlobalNetworkSettings();

        //! Destructor.
        ~CGlobalNetworkSettings() {}

        BlackMisc::Network::CServer m_fsdSwiftServer;
    };
}
#endif // guard
