/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/selcal.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"

#include <QStringList>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CSelcal)

namespace BlackMisc::Aviation
{
    QString CSelcal::convertToQString(bool /** i18n **/) const
    {
        return m_code;
    }

    QString CSelcal::unifyCode(const QString &selcalCandidate)
    {
        QString s;
        for (QChar c : selcalCandidate)
        {
            if (CSelcal::isValidCharacter(c)) { s += c;}
        }
        return CSelcal::isValidCode(s) ? s : QString();
    }

    bool CSelcal::equalsString(const QString &code) const
    {
        if (code.isEmpty()) return false;
        return (m_code.compare(code, Qt::CaseInsensitive) == 0);
    }

    QList<CFrequency> CSelcal::getFrequencies() const
    {
        QList<CFrequency> f;
        if (!CSelcal::isValidCode(m_code)) return f;
        f.reserve(m_code.length());
        for (int pos = 0; pos < m_code.length(); pos++)
        {
            f.append(CSelcal::audioFrequencyEquivalent(m_code.at(pos)));
        }
        return f;
    }

    const QString &CSelcal::validCharacters()
    {
        static const QString valid = "ABCDEFGHJKLMPQRS";
        return valid;
    }

    bool CSelcal::isValidCharacter(QChar c)
    {
        return CSelcal::validCharacters().contains(c.toUpper());
    }

    bool CSelcal::isValidCode(const QString &code)
    {
        if (code.length() != 4) return false;
        int p1, p2, p3, p4;
        QString codeUpper = code.toUpper();
        if ((p1 = CSelcal::validCharacters().indexOf(codeUpper.at(0))) < 0) return false;
        if ((p2 = CSelcal::validCharacters().indexOf(codeUpper.at(1))) < 0) return false;
        if ((p3 = CSelcal::validCharacters().indexOf(codeUpper.at(2))) < 0) return false;
        if ((p4 = CSelcal::validCharacters().indexOf(codeUpper.at(3))) < 0) return false;
        if (p1 >= p2 || p3 >= p4) return false; // pair in alphabetical order
        if (p1 == p3 || p2 == p3 || p2 == p4 || p3 == p4) return false; // given letter can be used only once in a SELCAL code
        return true;
    }

    const PhysicalQuantities::CFrequency &CSelcal::audioFrequencyEquivalent(QChar c)
    {
        int pos = CSelcal::validCharacters().indexOf(c);
        Q_ASSERT(pos >= 0);
        Q_ASSERT(CSelcal::audioFrequencyEquivalents().size() > pos);
        return CSelcal::audioFrequencyEquivalents()[pos];
    }

    const QList<CFrequency> &CSelcal::audioFrequencyEquivalents()
    {
        static const QList<CFrequency> frequencies(
        {
            CFrequency(312.7, CFrequencyUnit::Hz()),
            CFrequency(346.7, CFrequencyUnit::Hz()),
            CFrequency(384.6, CFrequencyUnit::Hz()),
            CFrequency(426.6, CFrequencyUnit::Hz()),
            CFrequency(473.2, CFrequencyUnit::Hz()),
            CFrequency(524.8, CFrequencyUnit::Hz()),
            CFrequency(582.1, CFrequencyUnit::Hz()),
            CFrequency(645.7, CFrequencyUnit::Hz()),
            CFrequency(716.1, CFrequencyUnit::Hz()),
            CFrequency(794.3, CFrequencyUnit::Hz()),
            CFrequency(881.0, CFrequencyUnit::Hz()),
            CFrequency(977.2, CFrequencyUnit::Hz()),
            CFrequency(1083.9, CFrequencyUnit::Hz()),
            CFrequency(1202.3, CFrequencyUnit::Hz()),
            CFrequency(1333.5, CFrequencyUnit::Hz()),
            CFrequency(1479.1, CFrequencyUnit::Hz())
        });
        return frequencies;
    }

    namespace Private
    {
        QStringList selcalCodePairs()
        {
            QStringList pairs;
            for (int p1 = 0; p1 < (CSelcal::validCharacters().length() - 1); p1++)
            {
                for (int p2 = p1 + 1; p2 < CSelcal::validCharacters().length(); p2++)
                {
                    QString pair;
                    pair.append(CSelcal::validCharacters().at(p1)).append(CSelcal::validCharacters().at(p2));
                    pairs.append(pair);
                }
            }
            return pairs;
        }
    }

    const QStringList &CSelcal::codePairs()
    {
        static const QStringList allCodePairs = Private::selcalCodePairs();
        return allCodePairs;
    }
} // namespace
