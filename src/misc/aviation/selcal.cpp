// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/selcal.h"

#include <QStringList>
#include <Qt>
#include <QtGlobal>

#include "misc/pq/frequency.h"
#include "misc/pq/units.h"

using namespace swift::misc::physical_quantities;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSelcal)

namespace swift::misc::aviation
{
    QString CSelcal::convertToQString(bool /** i18n **/) const { return m_code; }

    QString CSelcal::unifyCode(const QString &selcalCandidate)
    {
        QString s;
        for (QChar c : selcalCandidate)
        {
            if (CSelcal::isValidCharacter(c)) { s += c; }
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
        for (const QChar c : m_code) { f.append(CSelcal::audioFrequencyEquivalent(c)); }
        return f;
    }

    const QString &CSelcal::validCharacters()
    {
        static const QString valid = "ABCDEFGHJKLMPQRS";
        return valid;
    }

    bool CSelcal::isValidCharacter(QChar c) { return CSelcal::validCharacters().contains(c.toUpper()); }

    bool CSelcal::isValidCode(const QString &code)
    {
        if (code.length() != 4) return false;
        int p1 {};
        int p2 {};
        int p3 {};
        int p4 {};
        QString codeUpper = code.toUpper();
        if ((p1 = CSelcal::validCharacters().indexOf(codeUpper.at(0))) < 0) return false;
        if ((p2 = CSelcal::validCharacters().indexOf(codeUpper.at(1))) < 0) return false;
        if ((p3 = CSelcal::validCharacters().indexOf(codeUpper.at(2))) < 0) return false;
        if ((p4 = CSelcal::validCharacters().indexOf(codeUpper.at(3))) < 0) return false;
        if (p1 >= p2 || p3 >= p4) return false; // pair in alphabetical order
        if (p1 == p3 || p2 == p3 || p2 == p4 || p3 == p4) // cppcheck-suppress knownConditionTrueFalse
            return false; // given letter can be used only once in a SELCAL code
        return true;
    }

    const physical_quantities::CFrequency &CSelcal::audioFrequencyEquivalent(QChar c)
    {
        int pos = CSelcal::validCharacters().indexOf(c);
        Q_ASSERT(pos >= 0);
        Q_ASSERT(CSelcal::audioFrequencyEquivalents().size() > pos);
        return CSelcal::audioFrequencyEquivalents()[pos];
    }

    const QList<CFrequency> &CSelcal::audioFrequencyEquivalents()
    {
        static const QList<CFrequency> frequencies(
            { CFrequency(312.7, CFrequencyUnit::Hz()), CFrequency(346.7, CFrequencyUnit::Hz()),
              CFrequency(384.6, CFrequencyUnit::Hz()), CFrequency(426.6, CFrequencyUnit::Hz()),
              CFrequency(473.2, CFrequencyUnit::Hz()), CFrequency(524.8, CFrequencyUnit::Hz()),
              CFrequency(582.1, CFrequencyUnit::Hz()), CFrequency(645.7, CFrequencyUnit::Hz()),
              CFrequency(716.1, CFrequencyUnit::Hz()), CFrequency(794.3, CFrequencyUnit::Hz()),
              CFrequency(881.0, CFrequencyUnit::Hz()), CFrequency(977.2, CFrequencyUnit::Hz()),
              CFrequency(1083.9, CFrequencyUnit::Hz()), CFrequency(1202.3, CFrequencyUnit::Hz()),
              CFrequency(1333.5, CFrequencyUnit::Hz()), CFrequency(1479.1, CFrequencyUnit::Hz()) });
        return frequencies;
    }

    namespace private_ns
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
    } // namespace private_ns

    const QStringList &CSelcal::codePairs()
    {
        static const QStringList allCodePairs = private_ns::selcalCodePairs();
        return allCodePairs;
    }
} // namespace swift::misc::aviation
