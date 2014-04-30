#include "pqtime.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CTime::CTime(const QTime &time) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            int seconds = QTime(0, 0, 0).secsTo(time);
            CTime converted(seconds, CTimeUnit::s());
            converted.switchUnit(CTimeUnit::hms());
            *this = converted;
        }

        void CTime::parseFromString(const QString &time)
        {
            QTime t;
            const QString ts = time.trimmed();
            if (ts.contains(":") && (ts.length() == 8 || ts.length() == 5))
            {
                if (ts.length() == 5)
                    t = QTime::fromString(ts, "hh:mm");
                else if (ts.length() == 8)
                    t = QTime::fromString(ts, "hh:mm:ss");
                CTime parsed(t);
                if (ts.length() == 5)
                    parsed.switchUnit(CTimeUnit::hrmin());
                else if (ts.length() == 8)
                    parsed.switchUnit(CTimeUnit::hms());
                *this = parsed;
            }
            else
                CPhysicalQuantity::parseFromString(ts);
        }

        QTime CTime::toQTime() const
        {
            CTime copy(*this);
            copy.setUnit(CTimeUnit::hms());
            QString s = copy.toQString(false);
            QTime t = QTime::fromString(s, "hh:mm:ss");
            return t;
        }
    }
}
