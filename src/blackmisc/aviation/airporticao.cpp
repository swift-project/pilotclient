#include "blackmisc/aviation/airporticao.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CAirportIcao::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
        }

        bool CAirportIcao::equalsString(const QString &icaoCode) const
        {
            CAirportIcao other(icaoCode);
            return other == (*this);
        }

        QString CAirportIcao::unifyAirportCode(const QString &icaoCode)
        {
            QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            QRegExp reg("[A-Z]{4}");
            return (reg.exactMatch(code)) ? code : "";
        }

        bool CAirportIcao::isValidIcaoDesignator(const QString &icaoCode)
        {
            QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

    } // namespace
} // namespace
