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

        bool CAirportIcaoCode::equalsString(const QString &icaoCode) const
        {
            CAirportIcaoCode other(icaoCode);
            return other == (*this);
        }

        QString CAirportIcaoCode::unifyAirportCode(const QString &icaoCode)
        {
            QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            QRegularExpression reg("^[A-Z0-9]{4}$");
            auto match = reg.match(code);
            return match.hasMatch() ? code : QString();
        }

        bool CAirportIcaoCode::isValidIcaoDesignator(const QString &icaoCode)
        {
            QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

    } // namespace
} // namespace
