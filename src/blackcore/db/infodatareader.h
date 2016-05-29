/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_INFODATAREADER_H
#define BLACKCORE_DB_INFODATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/db/dbinfolist.h"

#include <QObject>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    namespace Db
    {
        //! Read information about data from Database
        class BLACKCORE_EXPORT CInfoDataReader : public CDatabaseReader
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

            //! Get info list
            //! \threadsafe
            BlackMisc::Db::CDbInfoList getDbInfoObjects() const;

            //! Get info list size
            //! \threadsafe
            int getDbInfoObjectCount() const;

            //! All data read?
            //! \threadsafe
            bool areAllDataRead() const;

            //! URL info objects web service
            BlackMisc::Network::CUrl getInfoObjectsUrl() const;

        public slots:
            //! Allow to call CInfoDataReader::ps_read directly, special for info objects
            void read(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::InfoObjectEntity, const QDateTime &newerThan = QDateTime());

        signals:
            //! Combined read signal
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        protected:
            //! \name cache handling for base class
            //! @{
            virtual void syncronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const override;
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;
            virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const override;
            //! @}

        private slots:
            //! Info objects have been read
            void ps_parseInfoObjectsData(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::InfoObjectEntity, const QDateTime &newerThan = QDateTime());

        private:
            BlackMisc::Db::CDbInfoList m_infoObjects;
            BlackMisc::Network::CUrl   m_urlInfoObjects;
            mutable QReadWriteLock     m_lockInfoObjects;

            //! Base URL
            BlackMisc::Network::CUrl getBaseUrl() const;
        };
    } // ns
} // ns
#endif // guard
