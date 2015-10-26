#include "blackmisc/aviation/airporticaocode.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CAirportIcaoCode::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
        }

        int CAirportIcaoCode::comparePropertyByIndex(const CAirportIcaoCode &compareValue, const CPropertyIndex &index) const
        {
            Q_UNUSED(index);
            return this->m_icaoCode.compare(compareValue.getIcaoCode(), Qt::CaseInsensitive);
        }

        bool CAirportIcaoCode::equalsString(const QString &icaoCode) const
        {
            CAirportIcaoCode other(icaoCode);
            return other == (*this);
        }

        QString CAirportIcaoCode::unifyAirportCode(const QString &icaoCode)
        {
            QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            QRegExp reg("[A-Z]{4}");
            return (reg.exactMatch(code)) ? code : "";
        }

        bool CAirportIcaoCode::isValidIcaoDesignator(const QString &icaoCode)
        {
            QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

    } // namespace
} // namespace
