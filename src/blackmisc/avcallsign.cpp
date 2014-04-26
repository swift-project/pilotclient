#include "avcallsign.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CCallsign::convertToQString(bool /** i18n **/) const
        {
            return this->m_callsign;
        }

        /*
         * Unify the callsign
         */
        QString CCallsign::unifyCallsign(const QString &callsign)
        {
            QString unified = callsign.toUpper();
            unified = unified.remove(QRegExp("[^a-zA-Z\\d\\s]"));
            return unified;
        }

        /*
         * Iconify
         */
        const QPixmap &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            static const QPixmap app(QPixmap(":/blackmisc/icons/tower.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap gnd(QPixmap(":/blackmisc/icons/tower.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap del(QPixmap(":/blackmisc/icons/tower.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap twr(QPixmap(":/blackmisc/icons/tower.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap pilot(QPixmap(":/blackmisc/icons/aeropuerto.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap sup(":/blackmisc/icons/SUP.png");
            static const QPixmap unknown(QPixmap(":/blackmisc/icons/question.png").scaledToWidth(16, Qt::SmoothTransformation));

            QString t = callsign.asString().toUpper();
            if (t.length() < 3) return unknown;
            t = t.right(3);

            if (callsign.getStringAsSet().contains("_"))
            {
                if ("APP" == t) return app;
                if ("GND" == t) return gnd;
                if ("TWR" == t) return twr;
                if ("DEL" == t) return del;
                if ("SUP" == t) return sup;
                return unknown;
            }
            else
            {
                return pilot;
            }
        }

        /*
         * Callsign as Observer
         */
        QString CCallsign::getAsObserverCallsignString() const
        {
            if (this->isEmpty()) return "";
            QString obs = this->getStringAsSet();
            if (obs.endsWith("_OBS", Qt::CaseInsensitive)) return obs;
            if (obs.contains('_')) obs = obs.left(obs.lastIndexOf('_'));
            return obs.append("_OBS").toUpper();
        }

        /*
         * Equals callsign?
         */
        bool CCallsign::equalsString(const QString &callsignString) const
        {
            CCallsign other(callsignString);
            return other == (*this);
        }

        /*
         * Compare
         */
        int CCallsign::compareImpl(const CValueObject &otherBase) const
        {
            // intentionally compare on string only!
            const auto &other = static_cast<const CCallsign &>(otherBase);
            return this->m_callsign.compare(other.m_callsign, Qt::CaseInsensitive);
        }

        /*
         * Marshall to DBus
         */
        void CCallsign::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CCallsign>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CCallsign::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CCallsign>::toTuple(*this);
        }

        /*
         * To JSON
         */
        QJsonObject CCallsign::toJson() const
        {
            return BlackMisc::serializeJson(CCallsign::jsonMembers(), TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CCallsign::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CCallsign::jsonMembers(), TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CCallsign::jsonMembers()
        {
            return TupleConverter<CCallsign>::jsonMembers();
        }

        /*
         * Equal?
         */
        bool CCallsign::operator ==(const CCallsign &other) const
        {
            if (this == &other) return true;
            // intentionally not via Tupel converter, compare on string only
            return this->asString().compare(other.asString(), Qt::CaseInsensitive) == 0;
        }

        /*
         * Unequal?
         */
        bool CCallsign::operator !=(const CCallsign &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CCallsign::getValueHash() const
        {
            return qHash(TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * Less than?
         */
        bool CCallsign::operator <(const CCallsign &other) const
        {
            return this->m_callsign < other.m_callsign;
        }

        /*
         * metaTypeId
         */
        int CCallsign::getMetaTypeId() const
        {
            return qMetaTypeId<CCallsign>();
        }

        /*
         * is a
         */
        bool CCallsign::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CCallsign>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CCallsign::registerMetadata()
        {
            qRegisterMetaType<CCallsign>();
            qDBusRegisterMetaType<CCallsign>();
        }

    } // namespace
} // namespace
