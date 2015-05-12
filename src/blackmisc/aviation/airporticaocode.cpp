#include "blackmisc/aviation/airporticaocode.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CAirportIcaoCode::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
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
