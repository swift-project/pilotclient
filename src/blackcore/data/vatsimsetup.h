// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATA_VATSIMDATA_H
#define BLACKCORE_DATA_VATSIMDATA_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/user.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Data, CVatsimSetup)

namespace BlackCore::Data
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
            IndexServerFiles = BlackMisc::CPropertyIndexRef::GlobalIndexCVatsimSetup,
            IndexDataFiles,
            IndexMetarFiles,
            IndexFsdServers,
            IndexCVoiceServers
        };

        //! Default constructor
        CVatsimSetup();

        //! VATSIM data file URL
        const BlackMisc::Network::CUrl &getDataFileUrl() const { return m_dataFileUrl; }

        //! Set VATSIM data file URL
        void setDataFileUrl(const BlackMisc::Network::CUrl &url) { m_dataFileUrl = url; }

        //! Server file URL
        const BlackMisc::Network::CUrl &getServerFileUrl() const { return m_serverFileUrl; }

        //! Set server file URL
        void setServerFileUrl(const BlackMisc::Network::CUrl &url) { m_serverFileUrl = url; }

        //! METAR file URL
        const BlackMisc::Network::CUrl &getMetarFileUrl() const { return m_metarFileUrl; }

        //! METAR file URL
        void setMetarFileUrl(const BlackMisc::Network::CUrl &url) { m_metarFileUrl = url; }

        //! Set all URLs and indicate if something has changed
        bool setUrls(const BlackMisc::Network::CUrl &dataFileUrl, const BlackMisc::Network::CUrl &serverFileUrl, const BlackMisc::Network::CUrl &metarFileUrl);

        //! FSD servers
        const BlackMisc::Network::CServerList &getFsdServers() const { return m_fsdServers; }

        //! Set FSD servers
        void setFsdServers(const BlackMisc::Network::CServerList &servers) { m_fsdServers = servers; }

        //! Voice servers
        const BlackMisc::Network::CServerList &getVoiceServers() const { return m_voiceServers; }

        //! Set voice servers
        void setVoiceServers(const BlackMisc::Network::CServerList &servers) { m_voiceServers = servers; }

        //! Set servers
        bool setServers(const BlackMisc::Network::CServerList &fsdServers, const BlackMisc::Network::CServerList &voiceServers);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        BlackMisc::Network::CUrl m_serverFileUrl; //!< only the FSD servers
        BlackMisc::Network::CUrl m_dataFileUrl; //!< Full VATSIM file
        BlackMisc::Network::CUrl m_metarFileUrl; //!< METAR file
        BlackMisc::Network::CServerList m_fsdServers; //!< FSD test servers
        BlackMisc::Network::CServerList m_voiceServers; //!< voice servers

        BLACK_METACLASS(
            CVatsimSetup,
            BLACK_METAMEMBER(serverFileUrl),
            BLACK_METAMEMBER(dataFileUrl),
            BLACK_METAMEMBER(metarFileUrl),
            BLACK_METAMEMBER(fsdServers),
            BLACK_METAMEMBER(voiceServers),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch)
        );
    };

    //! Trait for global setup data
    struct TVatsimSetup : public BlackMisc::TDataTrait<CVatsimSetup>
    {
        //! \copydoc BlackMisc::TDataTrait::key
        static const char *key() { return "vatsimsetup"; }

        //! \copydoc BlackMisc::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc BlackMisc::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM setup");
            return name;
        }
    };

    //! Trait for last (most recently) used VATSIM server and user
    struct TVatsimLastServer : public BlackMisc::TDataTrait<BlackMisc::Network::CServer>
    {
        //! \copydoc BlackMisc::TDataTrait::isPinned
        static const char *key() { return "vatsimserver"; }

        //! \copydoc BlackMisc::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc BlackMisc::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Last VATSIM server");
            return name;
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CVatsimSetup)

#endif // guard
