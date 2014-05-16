/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_SETTINGS_SERVER_H
#define BLACKMISC_SETTINGS_SERVER_H

#include "nwserverlist.h"
#include "valueobject.h"
#include "statusmessagelist.h"
#include "settingutilities.h"
#include <QJsonObject>

namespace BlackMisc
{
    class CVariant;

    namespace Settings
    {
        //! \brief Value object encapsulating information of network related settings.
        //! \remarks Not only traffic network settings, but also URLs, DBus address, ...
        class CSettingsNetwork : public BlackMisc::CValueObject
        {
        public:
            //! \brief Default constructor.
            CSettingsNetwork();

            //! \brief Destructor.
            virtual ~CSettingsNetwork() {}

            //! \brief Update
            static const QString &CmdSetCurrentServer()
            {
                static const QString cmd("currenttrafficserver");
                return cmd;
            }

            //! \brief Path
            static const QString &ValueTrafficServers()
            {
                static const QString value("trafficservers");
                return value;
            }

            //! \brief Path
            static const QString &ValueBookingServiceUrl()
            {
                static const QString value("bookingserviceurl");
                return value;
            }

            //! \brief Path
            static const QString &ValueDBusServerAddress()
            {
                static const QString value("dbuserveraddress");
                return value;
            }

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Value object, traffic network server objects
            BlackMisc::Network::CServerList getTrafficNetworkServers() const { return m_trafficNetworkServers; }

            //! \brief Selected traffic network server
            BlackMisc::Network::CServer getCurrentTrafficNetworkServer() const { return m_trafficNetworkServerCurrent; }

            //! \brief URL of booking service
            QString getBookingServiceUrl() const { return m_bookingServiceUrl; }

            //! \brief Address for DBus Server
            QString getDBusServerAddress() const { return m_dbusServerAddress; }

            //! \brief Selected traffic network server
            bool setCurrentNetworkServer(const BlackMisc::Network::CServer &currentServer);

            //! \brief Traffic network server objects
            void addTrafficNetworkServer(const BlackMisc::Network::CServer &server) { m_trafficNetworkServers.push_back(server); }

            //! \brief Equal operator ==
            bool operator ==(const CSettingsNetwork &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CSettingsNetwork &other) const;

            //! \copydoc BlackCore::IContextSettings
            virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag);

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! \brief init with meaningful default values
            void initDefaultValues();

            //! \brief Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSettingsNetwork)
            BlackMisc::Network::CServerList m_trafficNetworkServers;
            BlackMisc::Network::CServer m_trafficNetworkServerCurrent;
            QString m_bookingServiceUrl;
            QString m_dbusServerAddress;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingsNetwork)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Settings::CSettingsNetwork, (o.m_trafficNetworkServers, o.m_trafficNetworkServerCurrent, o.m_bookingServiceUrl, o.m_dbusServerAddress))


#endif // guard
