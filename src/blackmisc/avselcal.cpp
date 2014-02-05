#include "avselcal.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {

        QList<CFrequency> CSelcal::frequencyEquivalents = QList<CFrequency>();
        QStringList CSelcal::allCodePairs = QStringList();

        /*
         * Convert to string
         */
        QString CSelcal::convertToQString(bool /** i18n **/) const
        {
            return this->m_code;
        }

        /*
         * Marshall to DBus
         */
        void CSelcal::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_code;
        }

        /*
         * Unmarshall from DBus
         */
        void CSelcal::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_code;
        }

        /*
         * Equals code?
         */
        bool CSelcal::equalsString(const QString &code) const
        {
            if (code.isEmpty()) return false;
            return (this->m_code.compare(code, Qt::CaseInsensitive) == 0);
        }

        /*
         * SELCAL code frequencies
         */
        QList<CFrequency> CSelcal::getFrequencies() const
        {
            QList<CFrequency> f;
            if (!CSelcal::isValidCode(this->m_code)) return f;
            for (int pos = 0; pos < this->m_code.length(); pos++)
            {
                f.append(CSelcal::audioFrequencyEquivalent(this->m_code.at(pos)));
            }
            return f;
        }

        /*
         * Valid characters
         */
        const QString &CSelcal::validCharacters()
        {
            static const QString valid = "ABCDEFGHJKLMPQRS";
            return valid;
        }

        /*
         * Valid character?
         */
        bool CSelcal::isValidCharacter(QChar c)
        {
            return CSelcal::validCharacters().contains(c);
        }

        /*
         * Valid code
         */
        bool CSelcal::isValidCode(const QString &code)
        {
            if (code.length() != 4) return false;
            int p1, p2, p3, p4;
            if ((p1 = CSelcal::validCharacters().indexOf(code.at(0))) < 0) return false;
            if ((p2 = CSelcal::validCharacters().indexOf(code.at(1))) < 0) return false;
            if ((p3 = CSelcal::validCharacters().indexOf(code.at(2))) < 0) return false;
            if ((p4 = CSelcal::validCharacters().indexOf(code.at(3))) < 0) return false;
            if (p1 >= p2 || p3 >= p4) return false; // pair in alphabetical order
            if (p1 == p3 || p2 == p3 || p2 == p4 || p3 == p4) return false; // given letter can be used only once in a SELCAL code
            return true;
        }

        /*
         * Character to frequency equivalent
         */
        const PhysicalQuantities::CFrequency &CSelcal::audioFrequencyEquivalent(QChar c)
        {
            if (CSelcal::frequencyEquivalents.isEmpty())
            {
                QList<CFrequency> frequencies;
                frequencies
                        << CFrequency(312.7, CFrequencyUnit::Hz())
                        << CFrequency(346.7, CFrequencyUnit::Hz())
                        << CFrequency(384.6, CFrequencyUnit::Hz())
                        << CFrequency(426.6, CFrequencyUnit::Hz())
                        << CFrequency(473.2, CFrequencyUnit::Hz())
                        << CFrequency(524.8, CFrequencyUnit::Hz())
                        << CFrequency(582.1, CFrequencyUnit::Hz())
                        << CFrequency(645.7, CFrequencyUnit::Hz())
                        << CFrequency(716.1, CFrequencyUnit::Hz())
                        << CFrequency(794.3, CFrequencyUnit::Hz())
                        << CFrequency(881.0, CFrequencyUnit::Hz())
                        << CFrequency(977.2, CFrequencyUnit::Hz())
                        << CFrequency(1083.9, CFrequencyUnit::Hz())
                        << CFrequency(1202.3, CFrequencyUnit::Hz())
                        << CFrequency(1333.5, CFrequencyUnit::Hz())
                        << CFrequency(1479.1, CFrequencyUnit::Hz());
                CSelcal::frequencyEquivalents = frequencies;
            }
            int pos = CSelcal::validCharacters().indexOf(c);
            Q_ASSERT(pos >= 0);
            Q_ASSERT(CSelcal::frequencyEquivalents.size() > pos);
            return CSelcal::frequencyEquivalents.at(pos);
        }

        /*
         * Code pairs
         */
        const QStringList &CSelcal::codePairs()
        {
            if (CSelcal::allCodePairs.isEmpty())
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
                CSelcal::allCodePairs = pairs;
            }
            return CSelcal::allCodePairs;
        }

        /*
         * Equal?
         */
        bool CSelcal::operator ==(const CSelcal &other) const
        {
            if (this == &other) return true;
            return (this->m_code.compare(other.m_code, Qt::CaseInsensitive) == 0);
        }

        /*
         * Unequal?
         */
        bool CSelcal::operator !=(const CSelcal &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CSelcal::getValueHash() const
        {
            return qHash(this->m_code);
        }

        /*
         * metaTypeId
         */
        int CSelcal::getMetaTypeId() const
        {
            return qMetaTypeId<CSelcal>();
        }

        /*
         * is a
         */
        bool CSelcal::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CSelcal>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CSelcal::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSelcal &>(otherBase);
            return this->m_code.compare(other.getCode(), Qt::CaseInsensitive);
        }

        /*
         * Register metadata
         */
        void CSelcal::registerMetadata()
        {
            qRegisterMetaType<CSelcal>();
            qDBusRegisterMetaType<CSelcal>();
        }

    } // namespace
} // namespace
