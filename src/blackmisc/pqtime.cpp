#include "pqtime.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CTime::CTime(const QTime &time) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            int seconds = QTime().secsTo(time);
            CTime converted(seconds, CTimeUnit::s());
            converted.setUnit(CTimeUnit::hms());
            this->set(converted);
        }

        CTime::CTime(const QString &time) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            CTime parsed;
            parsed.parseFromString(time);
            this->set(parsed);
        }

        void CTime::parseFromString(const QString &time)
        {
            Q_ASSERT(time.length() == 5 || time.length() == 8);
            QTime t;
            if (time.length() == 5)
                t = QTime::fromString("hh:mm");
            else if (time.length() == 8)
                t = QTime::fromString("hh:mm:ss");
            CTime parsed(t);
            if (time.length() == 5)
                parsed.setUnit(CTimeUnit::hrmin());
            else if (time.length() == 8)
                parsed.setUnit(CTimeUnit::hms());
            else
                parsed.setUnit(CTimeUnit::nullUnit()); // indicates invalid
            this->set(parsed);
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
