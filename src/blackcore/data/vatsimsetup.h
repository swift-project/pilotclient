// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATA_VATSIMDATA_H
#define BLACKCORE_DATA_VATSIMDATA_H

#include "blackcore/blackcoreexport.h"
#include "misc/datacache.h"
#include "misc/metaclass.h"
#include "misc/network/serverlist.h"
#include "misc/network/url.h"
#include "misc/network/user.h"
#include "misc/propertyindex.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Data, CVatsimSetup)

namespace BlackCore::Data
{
    //! VATSIM data (servers, URLs) cached as last known good setup.
    class BLACKCORE_EXPORT CVatsimSetup :
        public swift::misc::CValueObject<CVatsimSetup>,
        public swift::misc::ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexServerFiles = swift::misc::CPropertyIndexRef::GlobalIndexCVatsimSetup,
            IndexDataFiles,
            IndexMetarFiles,
            IndexFsdServers,
            IndexCVoiceServers
        };

        //! Default constructor
        CVatsimSetup();

        //! VATSIM data file URL
        const swift::misc::network::CUrl &getDataFileUrl() const { return m_dataFileUrl; }

        //! Set VATSIM data file URL
        void setDataFileUrl(const swift::misc::network::CUrl &url) { m_dataFileUrl = url; }

        //! Server file URL
        const swift::misc::network::CUrl &getServerFileUrl() const { return m_serverFileUrl; }

        //! Set server file URL
        void setServerFileUrl(const swift::misc::network::CUrl &url) { m_serverFileUrl = url; }

        //! METAR file URL
        const swift::misc::network::CUrl &getMetarFileUrl() const { return m_metarFileUrl; }

        //! METAR file URL
        void setMetarFileUrl(const swift::misc::network::CUrl &url) { m_metarFileUrl = url; }

        //! Set all URLs and indicate if something has changed
        bool setUrls(const swift::misc::network::CUrl &dataFileUrl, const swift::misc::network::CUrl &serverFileUrl, const swift::misc::network::CUrl &metarFileUrl);

        //! FSD servers
        const swift::misc::network::CServerList &getFsdServers() const { return m_fsdServers; }

        //! Set FSD servers
        void setFsdServers(const swift::misc::network::CServerList &servers) { m_fsdServers = servers; }

        //! Voice servers
        const swift::misc::network::CServerList &getVoiceServers() const { return m_voiceServers; }

        //! Set voice servers
        void setVoiceServers(const swift::misc::network::CServerList &servers) { m_voiceServers = servers; }

        //! Set servers
        bool setServers(const swift::misc::network::CServerList &fsdServers, const swift::misc::network::CServerList &voiceServers);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        swift::misc::network::CUrl m_serverFileUrl; //!< only the FSD servers
        swift::misc::network::CUrl m_dataFileUrl; //!< Full VATSIM file
        swift::misc::network::CUrl m_metarFileUrl; //!< METAR file
        swift::misc::network::CServerList m_fsdServers; //!< FSD test servers
        swift::misc::network::CServerList m_voiceServers; //!< voice servers

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
    struct TVatsimSetup : public swift::misc::TDataTrait<CVatsimSetup>
    {
        //! \copydoc swift::misc::TDataTrait::key
        static const char *key() { return "vatsimsetup"; }

        //! \copydoc swift::misc::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc swift::misc::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("VATSIM setup");
            return name;
        }
    };

    //! Trait for last (most recently) used VATSIM server and user
    struct TVatsimLastServer : public swift::misc::TDataTrait<swift::misc::network::CServer>
    {
        //! \copydoc swift::misc::TDataTrait::isPinned
        static const char *key() { return "vatsimserver"; }

        //! \copydoc swift::misc::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc swift::misc::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Last VATSIM server");
            return name;
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CVatsimSetup)

#endif // guard
