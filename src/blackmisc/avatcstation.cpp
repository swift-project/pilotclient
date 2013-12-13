#include "avatcstation.h"
#include "aviocomsystem.h"
#include "blackmiscfreefunctions.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Constructor
         */
        CAtcStation::CAtcStation()  :
            m_distanceToPlane(-1.0, CLengthUnit::NM()), m_isOnline(false), m_atis(CInformationMessage::ATIS), m_metar(CInformationMessage::METAR)
        {
            // void
        }

        /*
         * Constructor
         */
        CAtcStation::CAtcStation(const QString &callsign)  :
            m_callsign(callsign), m_distanceToPlane(-1.0, CLengthUnit::NM()), m_isOnline(false), m_atis(CInformationMessage::ATIS), m_metar(CInformationMessage::METAR)
        {
            // void
        }

        /*
         * Constructor
         */
        CAtcStation::CAtcStation(const CCallsign &callsign, const CUser &controller, const CFrequency &frequency, const CCoordinateGeodetic &pos, const CLength &range, bool isOnline,
                                 const QDateTime &bookedFromUtc, const QDateTime &bookedUntilUtc, const CInformationMessage &atis, const CInformationMessage &metar) :
            m_callsign(callsign), m_controller(controller), m_frequency(frequency), m_position(pos),
            m_range(range), m_distanceToPlane(-1.0, CLengthUnit::NM()), m_isOnline(isOnline),
            m_bookedFromUtc(bookedFromUtc), m_bookedUntilUtc(bookedUntilUtc), m_atis(atis), m_metar(metar)
        {
            // void
        }

        /*
         * Convert to string
         */
        QString CAtcStation::convertToQString(bool i18n) const
        {
            QString s = i18n ?
                        QCoreApplication::translate("Aviation", "ATC station") :
                        "ATC station";
            s.append(' ').append(this->m_callsign.toQString(i18n));

            // position
            s.append(' ').append(this->m_position.toQString(i18n));

            // Online?
            s.append(' ');
            if (this->m_isOnline)
            {
                i18n ? s.append(QCoreApplication::translate("Aviation", "online")) : s.append("online");
            }
            else
            {
                i18n ? s.append(QCoreApplication::translate("Aviation", "offline")) : s.append("offline");
            }

            // controller name
            if (!this->m_controller.isValid())
            {
                s.append(' ');
                s.append(this->m_controller.toQString(i18n));
            }

            // frequency
            s.append(' ');
            s.append(this->m_frequency.toQString(i18n));

            // ATIS
            if (this->hasAtis())
            {
                s.append(' ');
                s.append(this->m_atis.toQString(i18n));
            }

            // METAR
            if (this->hasMetar())
            {
                s.append(' ');
                s.append(this->m_metar.toQString(i18n));
            }

            // range
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "range"))) : s.append("range");
            s.append(": ");
            s.append(this->m_range.toQString(i18n));

            // distance to plane
            if (this->m_distanceToPlane.isPositiveWithEpsilonConsidered())
            {
                i18n ? s.append(QCoreApplication::translate("Aviation", "distance")) : s.append("distance");
                s.append(' ');
                s.append(this->m_distanceToPlane.toQString(i18n));
            }

            // from / to
            if (!this->hasBookingTimes()) return s;

            // append from
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "from(UTC)"))) : s.append("from(UTC)");
            s.append(": ");
            if (this->m_bookedFromUtc.isNull())
            {
                s.append('-');
            }
            else
            {
                s.append(this->m_bookedFromUtc.toString("yy-MM-dd HH:mm"));
            }

            // append to
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "until(UTC)"))) : s.append("to(UTC)");
            s.append(": ");
            if (this->m_bookedFromUtc.isNull())
            {
                s.append('-');
            }
            else
            {
                s.append(this->m_bookedUntilUtc.toString("yy-MM-dd HH:mm"));
            }
            return s;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Aviation", "ATC station");
            (void)QT_TRANSLATE_NOOP("Aviation", "online");
            (void)QT_TRANSLATE_NOOP("Aviation", "offline");
            (void)QT_TRANSLATE_NOOP("Aviation", "from(UTC)");
            (void)QT_TRANSLATE_NOOP("Aviation", "until(UTC)");
            (void)QT_TRANSLATE_NOOP("Aviation", "range");
            (void)QT_TRANSLATE_NOOP("Aviation", "distance");

        }

        /*
         * Marshall to DBus
         */
        void CAtcStation::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_callsign;
            argument << this->m_frequency;
            argument << this->m_controller;
            argument << this->m_position;
            argument << this->m_range;
            argument << this->m_distanceToPlane;
            argument << this->m_isOnline;
            argument << this->m_bookedFromUtc;
            argument << this->m_bookedUntilUtc;
            argument << this->m_atis;
            argument << this->m_metar;
        }

        /*
         * Unmarshall from DBus
         */
        void CAtcStation::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_callsign;
            argument >> this->m_frequency;
            argument >> this->m_controller;
            argument >> this->m_position;
            argument >> this->m_range;
            argument >> this->m_distanceToPlane;
            argument >> this->m_isOnline;
            argument >> this->m_bookedFromUtc;
            argument >> this->m_bookedUntilUtc;
            argument >> this->m_atis;
            argument >> this->m_metar;
        }

        /*
         * Register metadata
         */
        void CAtcStation::registerMetadata()
        {
            qRegisterMetaType<CAtcStation>();
            qDBusRegisterMetaType<CAtcStation>();
        }

        /*
         * Equal?
         */
        bool CAtcStation::operator ==(const CAtcStation &other) const
        {
            if (this == &other) return true;

            if (other.getCallsign() != this->getCallsign() ||
                    other.getRange() != this->getRange() ||
                    other.getFrequency() != this->getFrequency() ||
                    other.getPosition() != this->getPosition()) return false;

            if (other.getController() != this->getController()) return false;
            if (other.getAtis() != this->getAtis()) return false;
            if (other.getMetar() != this->getMetar()) return false;

            return this->getBookedFromUtc() == other.getBookedFromUtc() &&
                   this->getBookedUntilUtc() == other.getBookedUntilUtc();
        }

        /*
         * Unequal?
         */
        bool CAtcStation::operator !=(const CAtcStation &other) const
        {
            return !((*this) == other);
        }

        /*
         * SyncronizeControllerData
         */
        void CAtcStation::syncronizeControllerData(CAtcStation &otherStation)
        {
            if (this->m_controller == otherStation.getController()) return;
            CUser otherController = otherStation.getController();
            this->m_controller.syncronizeData(otherController);
            otherStation.setController(otherController);
        }

        /*
         * Distance to planne
         */
        const CLength &CAtcStation::calculcateDistanceToPlane(const CCoordinateGeodetic &position)
        {
            this->m_distanceToPlane = greatCircleDistance(this->m_position, position);
            return this->m_distanceToPlane;
        }

        /*
         * Booked now
         */
        bool CAtcStation::isBookedNow() const
        {
            if (!this->hasValidBookingTimes()) return false;
            QDateTime now = QDateTime::currentDateTimeUtc();
            if (this->m_bookedFromUtc > now) return false;
            if (now > this->m_bookedUntilUtc) return false;
            return true;
        }

        /*!
         * \brief When booked?
         * \return
         */
        CTime CAtcStation::bookedWhen() const
        {
            if (!this->hasValidBookingTimes()) return CTime(-365.0, CTimeUnit::d());
            QDateTime now = QDateTime::currentDateTimeUtc();
            qint64 diffMs;
            if (this->m_bookedFromUtc > now)
            {
                // future
                diffMs = now.msecsTo(this->m_bookedFromUtc);
                return CTime(diffMs / 1000.0, CTimeUnit::s());
            }
            else if (this->m_bookedUntilUtc > now)
            {
                // now
                return CTime(0.0, CTimeUnit::s());
            }
            else
            {
                // past
                diffMs = m_bookedUntilUtc.msecsTo(now);
                return CTime(-diffMs / 1000.0, CTimeUnit::s());
            }
        }

        /*
         * Hash
         */
        uint CAtcStation::getValueHash() const
        {
            QList<uint> hashs;
            hashs << this->m_callsign.getValueHash();
            hashs << this->m_frequency.getValueHash();
            hashs << this->m_controller.getValueHash();
            hashs << this->m_position.getValueHash();
            hashs << this->m_range.getValueHash();
            hashs << this->m_distanceToPlane.getValueHash();
            hashs << this->m_metar.getValueHash();
            hashs << this->m_atis.getValueHash();
            hashs << qHash(this->m_isOnline ? 1 : 3);
            hashs << qHash(this->m_bookedFromUtc);
            hashs << qHash(this->m_bookedUntilUtc);
            return BlackMisc::calculateHash(hashs, "CAtcStation");
        }

        /*
         * Property by index
         */
        QVariant CAtcStation::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexBookedFrom:
                return QVariant(this->m_bookedFromUtc);
            case IndexBookedUntil:
                return QVariant(this->m_bookedUntilUtc);
            case IndexCallsign:
                return this->m_callsign.toQVariant();
            case IndexCallsignAsString:
                return QVariant(this->m_callsign.asString());
            case IndexCallsignAsStringAsSet:
                return QVariant(this->m_callsign.getStringAsSet());
            case IndexController:
                return this->m_controller.toQVariant();
            case IndexControllerRealname:
                return QVariant(this->getControllerRealname());
            case IndexControllerId:
                return QVariant(this->getControllerId());
            case IndexFrequency:
                return this->m_frequency.toQVariant();
            case IndexIsOnline:
                return QVariant(this->m_isOnline);
            case IndexLatitude:
                return this->latitude().toQVariant();
            case IndexDistance:
                return this->m_distanceToPlane.toQVariant();
            case IndexLongitude:
                return this->longitude().toQVariant();
            case IndexPosition:
                return this->m_position.toQVariant();
            case IndexRange:
                return this->m_range.toQVariant();
            case IndexAtis:
                return this->m_atis.toQVariant();
            case IndexAtisMessage:
                return QVariant(this->m_atis.getMessage());
            case IndexMetar:
                return this->m_metar.toQVariant();
            case IndexMetarMessage:
                return QVariant(this->m_metar.getMessage());
            default:
                break;
            }

            Q_ASSERT_X(false, "CAtcStation", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Set property as index
         */
        void CAtcStation::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexBookedFrom:
                this->setBookedFromUtc(variant.value<QDateTime>());
                break;
            case IndexBookedUntil:
                this->setBookedUntilUtc(variant.value<QDateTime>());
                break;
            case IndexCallsign:
                this->setCallsign(variant.value<CCallsign>());
                break;
            case IndexCallsignAsString:
                this->setCallsign(CCallsign(variant.value<QString>()));
                break;
            case IndexController:
                this->setController(variant.value<CUser>());
                break;
            case IndexControllerRealname:
                this->setControllerRealname(variant.value<QString>());
                break;
            case IndexControllerId:
                this->setControllerId(variant.value<QString>());
                break;
            case IndexFrequency:
                this->setFrequency(variant.value<CFrequency>());
                break;
            case IndexIsOnline:
                this->setOnline(variant.value<bool>());
                break;
            case IndexPosition:
                this->setPosition(variant.value<CCoordinateGeodetic>());
                break;
            case IndexRange:
                this->setRange(variant.value<CLength>());
                break;
            case IndexDistance:
                this->setDistanceToPlane(variant.value<CLength>());
                break;
            case IndexAtis:
                this->setAtis(variant.value<CInformationMessage>());
                break;
            case IndexAtisMessage:
                this->setAtisMessage(variant.value<QString>());
                break;
            case IndexMetar:
                this->setMetar(variant.value<CInformationMessage>());
                break;
            case IndexMetarMessage:
                this->setMetarMessage(variant.value<QString>());
                break;
            default:
                Q_ASSERT_X(false, "CAtcStation", "index unknown (setter)");
                break;
            }
        }

        /*
         * Compare
         */
        int CAtcStation::compare(const QVariant &qv) const
        {
            Q_ASSERT(qv.canConvert<CAtcStation>());
            Q_ASSERT(!qv.isNull() && qv.isValid());
            CAtcStation atc = qv.value<CAtcStation>();
            return this->getCallsign().compare(atc.getCallsign());
        }

        /*
         * Property as string by index
         */
        QString CAtcStation::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            // special treatment
            // this is required as it is possible an ATC station is not containing all
            // properties
            switch (index)
            {
            case IndexFrequency:
                if (!CComSystem::isValidCivilAviationFrequency(qv.value<CFrequency>()))
                {
                    return "";
                }
                break;
            case IndexDistance:
                {
                    CLength distance = qv.value<CLength>();
                    if (distance.isNegativeWithEpsilonConsidered()) return "";
                    return distance.toQString(i18n);
                }

            case IndexBookedFrom:
            case IndexBookedUntil:
                {
                    QDateTime dt = qv.value<QDateTime>();
                    if (dt.isNull() || !dt.isValid()) return "";
                    return dt.toString("yyyy-MM-dd HH:mm");
                    break;
                }
            default:
                break;
            }
            return BlackMisc::qVariantToString(qv, i18n);
        }

    } // namespace
} // namespace
