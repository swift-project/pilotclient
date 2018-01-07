#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/compare.h"

#include <QRegularExpression>
#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {

        QString CAirportIcaoCode::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
        }

        int CAirportIcaoCode::comparePropertyByIndex(const CPropertyIndex &index, const CAirportIcaoCode &compareValue) const
        {
            Q_UNUSED(index);
            return this->m_icaoCode.compare(compareValue.getIcaoCode(), Qt::CaseInsensitive);
        }

        bool CAirportIcaoCode::hasValidIcaoCode() const
        {
            return CAirportIcaoCode::isValidIcaoDesignator(this->getIcaoCode());
        }

        bool CAirportIcaoCode::equalsString(const QString &icaoCode) const
        {
            CAirportIcaoCode other(icaoCode);
            return other == (*this);
        }

        QString CAirportIcaoCode::unifyAirportCode(const QString &icaoCode)
        {
            const QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            if (containsChar(code, [](QChar c) { return !c.isLetterOrNumber(); })) { return ""; }
            return code;
        }

        bool CAirportIcaoCode::isValidIcaoDesignator(const QString &icaoCode)
        {
            const QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

        bool CAirportIcaoCode::containsNumbers(const QString &icaoCode)
        {
            return (containsChar(icaoCode, [](QChar c) { return c.isDigit(); }));
        }
    } // namespace
} // namespace
