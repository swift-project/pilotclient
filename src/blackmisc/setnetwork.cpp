#include "setnetwork.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsNetwork::CSettingsNetwork() :
            m_bookingServiceUrl("http://vatbook.euroutepro.com/xml2.php"),
            m_dbusServerAddress(BlackCore::CDBusServer::sessionDBusServer())
        {
            // settings
        }

        bool CSettingsNetwork::setCurrentNetworkServer(const CServer &currentServer)
        {
            if (this->m_trafficNetworkServerCurrent == currentServer) return false;
            m_trafficNetworkServerCurrent = currentServer;
            return true;
        }

        /*
         * Convert to string
         */
        QString CSettingsNetwork::convertToQString(bool i18n) const
        {
            QString s("Traffic servers:");
            s.append(" ").append(this->m_trafficNetworkServers.toQString(i18n));
            return s;
        }

        /*
         * metaTypeId
         */
        int CSettingsNetwork::getMetaTypeId() const
        {
            return qMetaTypeId<CSettingsNetwork>();
        }

        /*
         * is a
         */
        bool CSettingsNetwork::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CSettingsNetwork>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CSettingsNetwork::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSettingsNetwork &>(otherBase);
            return compare(TupleConverter<CSettingsNetwork>::toTuple(*this), TupleConverter<CSettingsNetwork>::toTuple(other));
        }

        /*
         * Marshall
         */
        void CSettingsNetwork::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CSettingsNetwork>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CSettingsNetwork::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CSettingsNetwork>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CSettingsNetwork::operator ==(const CSettingsNetwork &other) const
        {
            if (this == &other) return true;
            return compare(*this, other) == 0;
        }

        /*
         * Unequal?
         */
        bool CSettingsNetwork::operator !=(const CSettingsNetwork &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CSettingsNetwork::getValueHash() const
        {
            return qHash(TupleConverter<CSettingsNetwork>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CSettingsNetwork::toJson() const
        {
            return BlackMisc::serializeJson(CSettingsNetwork::jsonMembers(), TupleConverter<CSettingsNetwork>::toTuple(*this));
        }

        /*
         * From JSON
         */
        void CSettingsNetwork::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CSettingsNetwork::jsonMembers(), TupleConverter<CSettingsNetwork>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CSettingsNetwork::jsonMembers()
        {
            return TupleConverter<CSettingsNetwork>::jsonMembers();
        }

        /*
         * Default values
         */
        void CSettingsNetwork::initDefaultValues()
        {
            this->m_trafficNetworkServers.clear();
            CServer currentServer(CServer("Testserver", "Client project testserver", "vatsim-germany.org", 6809, CUser("guest", "Guest Client project", "", "guest")));
            this->setCurrentNetworkServer(currentServer);
            this->addTrafficNetworkServer(this->getCurrentTrafficNetworkServer());
            this->addTrafficNetworkServer(CServer("Europe C2", "VATSIM Server", "88.198.19.202", 6809, CUser("vatsimid", "Black Client", "", "vatsimpw")));
            this->addTrafficNetworkServer(CServer("Europe CC", "VATSIM Server", "5.9.155.43", 6809, CUser("vatsimid", "Black Client", "", "vatsimpw")));
            this->addTrafficNetworkServer(CServer("UK", "VATSIM Server", "109.169.48.148", 6809, CUser("vatsimid", "Black Client", "", "vatsimpw")));
            this->addTrafficNetworkServer(CServer("USA-W", "VATSIM Server", "64.151.108.52", 6809, CUser("vatsimid", "Black Client", "", "vatsimpw")));
        }

        /*
         * Register metadata
         */
        void CSettingsNetwork::registerMetadata()
        {
            qRegisterMetaType<CSettingsNetwork>();
            qDBusRegisterMetaType<CSettingsNetwork>();
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessageList CSettingsNetwork::value(const QString &path, const QString &command, const QVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            CStatusMessageList msgs;
            if (path == CSettingsNetwork::ValueTrafficServers())
            {
                const CServer server = value.value<CServer>();
                if (command == CSettingsNetwork::CmdSetCurrentServer())
                {
                    changedFlag = this->setCurrentNetworkServer(server);
                    if (changedFlag)
                    {
                        // make sure the server is correct int the list too
                        this->m_trafficNetworkServers.replaceOrAdd(&CServer::getName, server.getName(), server);
                    }
                }
                else if (command == CSettingUtilities::CmdUpdate())
                {
                    this->m_trafficNetworkServers.replaceOrAdd(&CServer::getName, server.getName(), server);
                    changedFlag = true;
                }
                else if (command == CSettingUtilities::CmdRemove())
                {
                    changedFlag = this->m_trafficNetworkServers.contains(&CServer::getName, server.getName());
                    this->m_trafficNetworkServers.removeIf(&CServer::getName, server.getName());
                }
                msgs.push_back(CStatusMessage::getInfoMessage("set current server", CStatusMessage::TypeSettings));
                return msgs;
            }
            else if (path == CSettingsNetwork::ValueBookingServiceUrl())
            {
                if (command == CSettingUtilities::CmdUpdate())
                {
                    QString v = value.toString();
                    if (this->m_bookingServiceUrl == v)
                    {
                        msgs.push_back(CSettingUtilities::valueNotChangedMessage("booking URL"));
                    }
                    else
                    {
                        changedFlag = true;
                        msgs.push_back(CSettingUtilities::valueChangedMessage("booking URL"));
                    }
                    return msgs;
                }
            }
            return CSettingUtilities::wrongPathMessages(path);
        }
    } // namespace
} // namespace
