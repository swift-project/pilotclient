#include "setnetwork.h"
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
        CSettingsNetwork::CSettingsNetwork()
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

            int result;
            if ((result = compare(this->m_trafficNetworkServerCurrent, other.m_trafficNetworkServerCurrent))) { return result; }
            if ((result = compare(this->m_trafficNetworkServers, other.m_trafficNetworkServers))) { return result; }
            return 0;
        }

        /*
         * Marshall to DBus
         */
        void CSettingsNetwork::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_trafficNetworkServerCurrent;
            argument << this->m_trafficNetworkServers;
        }

        /*
         * Unmarshall from DBus
         */
        void CSettingsNetwork::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_trafficNetworkServerCurrent;
            argument >> this->m_trafficNetworkServers;
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
            QList<uint> hashs;
            hashs << qHash(this->m_trafficNetworkServers);
            return BlackMisc::calculateHash(hashs, "CSettingsNetwork");
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
            return CSettingUtilities::wrongPathMessages(path);
        }

    } // namespace
} // namespace
