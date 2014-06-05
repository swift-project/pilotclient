#include "pqtime.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CTime::CTime(int hours, int minutes, int seconds) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            double value = hours + minutes / 100.0 + seconds / 10000.0;
            if (minutes == 0 && seconds == 0)
            {
                (*this) = CTime(hours, CTimeUnit::h());
            }
            else
            {
                if (seconds == 0)
                    (*this) = CTime(value, CTimeUnit::hrmin());
                else
                    (*this) = CTime(value, CTimeUnit::hms());
            }
        }

        CTime::CTime(const QTime &time) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            CTime converted(time.hour(), time.minute(), time.second());
            (*this) = converted;
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
                (*this) = CTime(t);
            }
            else
                CPhysicalQuantity::parseFromString(ts);
        }

        QTime CTime::toQTime() const
        {
            CTime copy(*this);
            copy.setUnit(CTimeUnit::hms());
            QString s = copy.toQString(false).replace('h', ':').replace('m', ':').replace('s', "");
            QTime t = s.length() == 8 ?
                      QTime::fromString(s, "hh:mm:ss") :
                      QTime::fromString(s, "hh:mm");
            return t;
        }

        QList<int> CTime::getHrsMinSecParts() const
        {
            QTime t = this->toQTime();
            QList<int> parts;
            parts << t.hour() << t.minute() << t.second();
            return parts;
        }
    }
}
