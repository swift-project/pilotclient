#include "avairporticao.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAirportIcao::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
        }

        /*
         * Equals callsign?
         */
        bool CAirportIcao::equalsString(const QString &icaoCode) const
        {
            CAirportIcao other(icaoCode);
            return other == (*this);
        }

        /*
         * Unify ICAO code
         */
        QString CAirportIcao::unifyAirportCode(const QString &icaoCode)
        {
            QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            QRegExp reg("[A-Z]{4}");
            return (reg.exactMatch(code)) ? code : "";
        }

        /*
         * Valid ICAO designator?
         */
        bool CAirportIcao::isValidIcaoDesignator(const QString &icaoCode)
        {
            QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

    } // namespace
} // namespace
