/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_VATSIMDATA_H
#define BLACKCORE_DATA_VATSIMDATA_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include <QStringList>

namespace BlackCore
{
    namespace Data
    {
        //! VATSIM data (servers, URLs) cached as last known good setup.
        class BLACKCORE_EXPORT CVatsimSetup :
            public BlackMisc::CValueObject<CVatsimSetup>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexServerFiles = BlackMisc::CPropertyIndex::GlobalIndexCVatsimSetup,
                IndexDataFiles,
                IndexMetarFiles,
                IndexFsdServers,
                IndexCVoiceServers,
                IndexLastLoginUser
            };

            //! Default constructor
            CVatsimSetup();

            //! Destructor.
            ~CVatsimSetup() {}

            //! VATSIM data file URLs
            const BlackMisc::Network::CUrlList &getDataFileUrls() const { return m_dataFileUrls; }

            //! Set VATSIM data file URLs
            void setDataFileUrls(const BlackMisc::Network::CUrlList &urls) { m_dataFileUrls = urls; }

            //! Server file URLs (like data file, only servers)
            const BlackMisc::Network::CUrlList &getServerFileUrls() const { return m_serverFileUrls; }

            //! Set server file URLs (like data file, only servers)
            void setServerFileUrls(const BlackMisc::Network::CUrlList &urls) { m_serverFileUrls = urls; }

            //! METAR file URLs
            const BlackMisc::Network::CUrlList &getMetarFileUrls() const { return m_metarFileUrls; }

            //! METAR file URLs
            void setMetarFileUrls(const BlackMisc::Network::CUrlList &urls) { m_metarFileUrls = urls; }

            //! FSD test servers
            const BlackMisc::Network::CServerList &getFsdServers() const { return m_fsdServers; }

            //! User for last login
            const BlackMisc::Network::CUser &getLastLoginUser() const { return m_lastLoginUser; }

            //! Set FSD servers
            void setFsdServers(const BlackMisc::Network::CServerList &servers) { m_fsdServers = servers; }

            //! Voice servers
            const BlackMisc::Network::CServerList &getVoiceServers() const { return m_voiceServers; }

            //! Set voice servers
            void setVoiceServers(const BlackMisc::Network::CServerList &servers) { m_voiceServers = servers; }

            //! User for last login
            void setLastLoginUser(const BlackMisc::Network::CUser &user) { m_lastLoginUser = user; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(BlackCore::Data::CVatsimSetup)

            BlackMisc::Network::CUrlList    m_serverFileUrls; //!< only the FSD servers
            BlackMisc::Network::CUrlList    m_dataFileUrls;   //!< Full VATSIM files
            BlackMisc::Network::CUrlList    m_metarFileUrls;  //!< METAR files
            BlackMisc::Network::CServerList m_fsdServers;     //!< FSD test servers
            BlackMisc::Network::CServerList m_voiceServers;   //!< voice servers
            BlackMisc::Network::CUser       m_lastLoginUser;  //!< last login user
        };

        //! Trait for global setup data
        struct VatsimSetup : public BlackMisc::CDataTrait<CVatsimSetup>
        {
            //! Key in data cache
            static const char *key() { return "vatsim"; }

            //! Default value
            static const CVatsimSetup &defaultValue()
            {
                static const CVatsimSetup gs;
                return gs;
            }
        };

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CVatsimSetup)
BLACK_DECLARE_TUPLE_CONVERSION(BlackCore::Data::CVatsimSetup, (
                                   attr(o.m_serverFileUrls),
                                   attr(o.m_dataFileUrls),
                                   attr(o.m_metarFileUrls),
                                   attr(o.m_fsdServers),
                                   attr(o.m_voiceServers),
                                   attr(o.m_lastLoginUser),
                                   attr(o.m_timestampMSecsSinceEpoch)
                               ))
#endif // guard
