#include "setsimulator.h"

using namespace BlackMisc;
using namespace BlackMisc::Settings;

namespace BlackSim
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsSimulator::CSettingsSimulator()
        {
            this->initDefaultValues();
        }

        /*
         * Convert to string
         */
        QString CSettingsSimulator::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("Sel.driver:");
            s.append(" ").append(m_selectedDriver.toQString(i18n));
            return s;
        }

        /*
         * metaTypeId
         */
        int CSettingsSimulator::getMetaTypeId() const
        {
            return qMetaTypeId<CSettingsSimulator>();
        }

        /*
         * is a
         */
        bool CSettingsSimulator::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CSettingsSimulator>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CSettingsSimulator::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSettingsSimulator &>(otherBase);
            return compare(TupleConverter<CSettingsSimulator>::toTuple(*this), TupleConverter<CSettingsSimulator>::toTuple(other));
        }

        /*
         * Marshall
         */
        void CSettingsSimulator::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CSettingsSimulator>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CSettingsSimulator::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CSettingsSimulator>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CSettingsSimulator::operator ==(const CSettingsSimulator &other) const
        {
            if (this == &other) return true;
            return compare(*this, other) == 0;
        }

        /*
         * Unequal?
         */
        bool CSettingsSimulator::operator !=(const CSettingsSimulator &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CSettingsSimulator::getValueHash() const
        {
            return qHash(TupleConverter<CSettingsSimulator>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CSettingsSimulator::toJson() const
        {
            return BlackMisc::serializeJson(CSettingsSimulator::jsonMembers(), TupleConverter<CSettingsSimulator>::toTuple(*this));
        }

        /*
         * From JSON
         */
        void CSettingsSimulator::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CSettingsSimulator::jsonMembers(), TupleConverter<CSettingsSimulator>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CSettingsSimulator::jsonMembers()
        {
            return TupleConverter<CSettingsSimulator>::jsonMembers();
        }

        /*
         * Default values
         */
        void CSettingsSimulator::initDefaultValues()
        {
            this->m_selectedDriver = CSimulatorInfo::FSX();
        }

        /*
         * Register metadata
         */
        void CSettingsSimulator::registerMetadata()
        {
            qRegisterMetaType<CSettingsSimulator>();
            qDBusRegisterMetaType<CSettingsSimulator>();
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessageList CSettingsSimulator::value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            CStatusMessageList msgs;
            if (path == CSettingsSimulator::ValueSelectedDriver())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    if (command == CSettingUtilities::CmdUpdate())
                    {
                        CSimulatorInfo v = value.value<CSimulatorInfo>();
                        changedFlag = (v != this->m_selectedDriver);
                        msgs.push_back(CSettingUtilities::valueChangedMessage(changedFlag, "selected driver"));
                        this->m_selectedDriver = v;
                        return msgs;
                    }
                    return msgs;
                }
            }
            return CSettingUtilities::wrongPathMessages(path);
        }
    } // namespace
} // namespace
