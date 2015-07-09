/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_READERSETTINGS_H
#define BLACKCORE_READERSETTINGS_H

#include "blackcoreexport.h"
#include <QStringList>
#include <QObject>

namespace BlackCore
{
    //! Settings for readers
    class BLACKCORE_EXPORT CReaderSettings : public QObject
    {
        Q_OBJECT

    public:
        //! ICAO Reader protocol
        const QString &protocolIcaoReader() const { return m_protocolIcaoReader; }

        //! ICAO Reader server
        const QString &serverIcaoReader() const { return m_serverIcaoReader; }

        //! ICAO Reader base URL
        const QString &baseUrlIcaoReader() const { return m_baseUrlIcaoReader; }

        //! Model Reader protocol
        const QString &protocolModelReader() const { return m_protocolModelReader; }

        //! Model Reader server
        const QString &serverModelReader() const { return m_serverModelReader; }

        //! Model Reader server
        const QString &baseUrlModelReader() const { return m_baseUrlModelReader; }

        //! URL to read bookings
        const QString &bookingsUrl() const { return m_bookingsUrl; }

        //! VATSIM data file URLs
        const QStringList &vatsimDataFileUrls() const { return m_vatsimDataFileUrls; }

        //! Singleton
        static const CReaderSettings &instance();

    private:
        //! Default constructor
        CReaderSettings();

        //! Destructor.
        ~CReaderSettings() {}

        QString m_protocolIcaoReader;
        QString m_serverIcaoReader;
        QString m_baseUrlIcaoReader;
        QString m_protocolModelReader;
        QString m_serverModelReader;
        QString m_baseUrlModelReader;
        QString m_bookingsUrl;
        QStringList m_vatsimDataFileUrls;
    };
}
#endif // guard
