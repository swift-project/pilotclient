#include "avaircraft.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqconstants.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAircraft::convertToQString(bool i18n) const
        {
            QString s(this->m_callsign.toQString(i18n));
            s.append(" ").append(this->m_pilot.toQString(i18n));
            s.append(" ").append(this->m_situation.toQString(i18n));
            s.append(" ").append(this->m_com1system.toQString(i18n));
            s.append(" ").append(this->m_com2system.toQString(i18n));
            s.append(" ").append(this->m_transponder.toQString(i18n));
            return s;
        }

        /*
         * Marshall to DBus
         */
        void CAircraft::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_callsign;
            argument << this->m_pilot;
            argument << this->m_situation;
            argument << this->m_com1system;
            argument << this->m_com2system;
            argument << this->m_transponder;
            argument << this->m_icao;
            argument << this->m_distanceToPlane;
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraft::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_callsign;
            argument >> this->m_pilot;
            argument >> this->m_situation;
            argument >> this->m_com1system;
            argument >> this->m_com2system;
            argument >> this->m_transponder;
            argument >> this->m_icao;
            argument >> this->m_distanceToPlane;
        }

        /*
         * All relevant information
         */
        const PhysicalQuantities::CLength &CAircraft::calculcateDistanceToPlane(const Geo::CCoordinateGeodetic &position)
        {
            this->m_distanceToPlane = Geo::greatCircleDistance(position, this->m_situation.getPosition());
            return this->m_distanceToPlane;
        }

        /*
         * All relevant information?
         */
        bool CAircraft::isValidForLogin() const
        {
            if (this->m_callsign.asString().isEmpty()) return false;
            if (!this->m_pilot.isValid()) return false;
            return true;
        }

        /*
         * Meaningful values
         */
        void CAircraft::initComSystems()
        {
            CComSystem com1("COM1", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            CComSystem com2("COM2", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            this->setCom1System(com1);
            this->setCom2System(com2);
        }

        /*
         * Meaningful values
         */
        void CAircraft::initTransponder()
        {
            CTransponder xpdr("TRANSPONDER", 7000, CTransponder::ModeS);
            this->setTransponder(xpdr);
        }

        /*
         * Equal?
         */
        bool CAircraft::operator ==(const CAircraft &other) const
        {
            if (this == &other) return true;
            return this->getValueHash() == other.getValueHash();
        }

        /*
         * Unequal?
         */
        bool CAircraft::operator !=(const CAircraft &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAircraft::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_callsign);
            hashs << qHash(this->m_pilot);
            hashs << qHash(this->m_situation);
            hashs << qHash(this->m_com1system);
            hashs << qHash(this->m_com2system);
            hashs << qHash(this->m_transponder);
            hashs << qHash(this->m_icao);
            return BlackMisc::calculateHash(hashs, "CAircraft");
        }

        /*
         * metaTypeId
         */
        int CAircraft::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraft>();
        }

        /*
         * is a
         */
        bool CAircraft::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraft>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAircraft::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraft &>(otherBase);

            return this->getCallsign().asString().compare(other.getCallsign().asString(), Qt::CaseInsensitive);
        }

        /*
         * Property by index
         */
        QVariant CAircraft::propertyByIndex(int index) const
        {
            if (index >= static_cast<int>(CAircraftIcao::IndexIcaoDesignator))
                return this->m_icao.propertyByIndex(index);

            if (index >= static_cast<int>(CAircraftSituation::IndexPosition))
                return this->m_situation.propertyByIndex(index);

            switch (index)
            {
            case IndexCallsign:
                return QVariant::fromValue(this->m_callsign);
            case IndexCallsignAsString:
                return QVariant::fromValue(this->m_callsign.toQString(true));
            case IndexCallsignAsStringAsSet:
                return QVariant(this->m_callsign.getStringAsSet());
            case IndexPilotId:
                return QVariant::fromValue(this->m_pilot.getId());
            case IndexPilotRealName:
                return QVariant::fromValue(this->m_pilot.getRealName());
            case IndexDistance:
                return this->m_distanceToPlane.toQVariant();
            case IndexCom1System:
                return this->m_com1system.toQVariant();
            case IndexFrequencyCom1:
                return this->m_com1system.getFrequencyActive().toQVariant();
            case IndexTransponder:
                return this->m_transponder.toQVariant();
            case IndexTansponderFormatted:
                return QVariant::fromValue(this->m_transponder.getTransponderCodeAndModeFormatted());
            case IndexSituation:
                return this->m_situation.toQVariant();
            case IndexIcao:
                return this->m_icao.toQVariant();
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraft", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Property as string by index
         */
        QString CAircraft::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            // special treatment
            // this is required as it is possible an ATC station is not containing all
            // properties
            switch (index)
            {
            case IndexFrequencyCom1:
                if (!CComSystem::isValidCivilAviationFrequency(qv.value<CFrequency>()))
                    return "";
                else
                    return qv.value<CFrequency>().valueRoundedWithUnit(3, i18n);
                break;
            case IndexDistance:
                {
                    CLength distance = qv.value<CLength>();
                    if (distance.isNegativeWithEpsilonConsidered()) return "";
                    return distance.toQString(i18n);
                }
            default:
                break;
            }
            return BlackMisc::qVariantToString(qv, i18n);
        }

        /*
         * Property by index (setter)
         */
        void CAircraft::setPropertyByIndex(const QVariant &variant, int index)
        {
            if (index >= static_cast<int>(CAircraftIcao::IndexIcaoDesignator))
                return this->m_icao.setPropertyByIndex(variant, index);

            if (index >= static_cast<int>(CAircraftSituation::IndexPosition))
                return this->m_situation.setPropertyByIndex(variant, index);

            switch (index)
            {
            case IndexCallsign:
                this->setCallsign(variant.value<CCallsign>());
                break;
            case IndexCallsignAsString:
                this->m_callsign = CCallsign(variant.value<QString>());
                break;
            case IndexPilotRealName:
                this->m_pilot.setRealName(variant.value<QString>());
                break;
            case IndexDistance:
                this->m_distanceToPlane = variant.value<CLength>();
                break;
            case IndexCom1System:
                this->setCom1System(variant.value<CComSystem>());
                break;
            case IndexFrequencyCom1:
                this->m_com1system.setFrequencyActive(variant.value<CFrequency>());
                break;
            case IndexTransponder:
                this->setTransponder(variant.value<CTransponder>());
                break;
            case IndexIcao:
                this->setIcaoInfo(variant.value<CAircraftIcao>());
                break;
            case IndexSituation:
                this->setSituation(variant.value<CAircraftSituation>());
                break;
            default:
                Q_ASSERT_X(false, "CAircraft", "index unknown");
                break;
            }
        }

        /*
         * Register metadata
         */
        void CAircraft::registerMetadata()
        {
            qRegisterMetaType<CAircraft>();
            qDBusRegisterMetaType<CAircraft>();
        }

    } // namespace
} // namespace
