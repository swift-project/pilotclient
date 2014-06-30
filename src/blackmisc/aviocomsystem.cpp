/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/aviocomsystem.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Valid values?
         */
        bool CComSystem::validValues() const
        {
            if (this->isDefaultValue()) return true; // special case
            return
                (CComSystem::isValidCivilAviationFrequency(this->getFrequencyActive()) ||
                 CComSystem::isValidMilitaryFrequency(this->getFrequencyActive())) &&
                (CComSystem::isValidCivilAviationFrequency(this->getFrequencyStandby()) ||
                 CComSystem::isValidMilitaryFrequency(this->getFrequencyStandby()));
        }

        void CComSystem::setFrequencyActiveMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            if (f == this->getFrequencyActive()) return; // save all the comparisons / rounding
            CComSystem::roundToChannelSpacing(f, this->m_channelSpacing);
            this->CModulator::setFrequencyActive(f);
        }

        void CComSystem::setFrequencyStandbyMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            if (f == this->getFrequencyStandby()) return; // save all the comparisons / rounding
            CComSystem::roundToChannelSpacing(f, this->m_channelSpacing);
            CModulator::setFrequencyStandby(f);
        }

        /*
         * Marshall
         */
        void CComSystem::marshallToDbus(QDBusArgument &argument) const
        {
            CModulator::marshallToDbus(argument);
            argument << TupleConverter<CComSystem>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CComSystem::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CModulator::unmarshallFromDbus(argument);
            argument >> TupleConverter<CComSystem>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CComSystem::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CModulator::getValueHash();
            hashs << qHash(TupleConverter<CComSystem>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CComSystem");
        }

        /*
         * To JSON
         */
        QJsonObject CComSystem::toJson() const
        {
            QJsonObject json = BlackMisc::serializeJson(CComSystem::jsonMembers(), TupleConverter<CComSystem>::toTuple(*this));
            return BlackMisc::Json::appendJsonObject(json, CModulator::toJson());
        }

        /*
         * To JSON
         */
        void CComSystem::fromJson(const QJsonObject &json)
        {
            CModulator::fromJson(json);
            BlackMisc::deserializeJson(json, CComSystem::jsonMembers(), TupleConverter<CComSystem>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CComSystem::jsonMembers()
        {
            return TupleConverter<CComSystem>::jsonMembers();
        }

        /*
         * Compare
         */
        int CComSystem::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CComSystem &>(otherBase);
            int result = compare(TupleConverter<CComSystem>::toTuple(*this), TupleConverter<CComSystem>::toTuple(other));
            return result == 0 ? CModulator::compareImpl(otherBase) : result;
        }

        bool CComSystem::operator ==(const CComSystem &other) const
        {
            if (this == &other) return true;
            if (!CModulator::operator ==(other)) return false;
            return TupleConverter<CComSystem>::toTuple(*this) == TupleConverter<CComSystem>::toTuple(other);
        }

        bool CComSystem::operator !=(const CComSystem &other) const
        {
            return !((*this) == other);
        }

        /*
         * Round to channel spacing
         */
        void CComSystem::roundToChannelSpacing(PhysicalQuantities::CFrequency &frequency, ChannelSpacing channelSpacing)
        {
            double channelSpacingKHz = CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
            double f = frequency.valueRounded(CFrequencyUnit::kHz(), 0);
            quint32 d = static_cast<quint32>(f / channelSpacingKHz);
            frequency.switchUnit(CFrequencyUnit::MHz());
            double f0 = frequency.valueRounded(CFrequencyUnit::MHz(), 3);
            double f1 = CMath::round(d * (channelSpacingKHz / 1000.0), 3);
            double f2 = CMath::round((d + 1) * (channelSpacingKHz / 1000.0), 3);
            bool down = qAbs(f1 - f0) < qAbs(f2 - f0); // which is the closest value
            frequency.setCurrentUnitValue(down ? f1 : f2);
        }

        /*
         * Within channel spacing
         */
        bool CComSystem::isWithinChannelSpacing(const CFrequency &setFrequency, const CFrequency &compareFrequency, CComSystem::ChannelSpacing channelSpacing)
        {
            if (setFrequency == compareFrequency) return true; // shortcut for many of such comparisons
            double channelSpacingKHz = 0.5 * CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
            double compareFrequencyKHz = compareFrequency.value(CFrequencyUnit::kHz());
            double setFrequencyKHz = setFrequency.value(CFrequencyUnit::kHz());
            return (setFrequencyKHz - channelSpacingKHz < compareFrequencyKHz) &&
                   (setFrequencyKHz + channelSpacingKHz > compareFrequencyKHz);
        }

        /*
         * Helper, give me number for channels spacing
         */
        double CComSystem::channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing)
        {
            switch (channelSpacing)
            {
            case ChannelSpacing50KHz: return 50.0;
            case ChannelSpacing25KHz: return 25.0;
            case ChannelSpacing8_33KHz: return 25.0 / 3.0;
            default: qFatal("Wrong channel spacing"); return 0.0; // return just supressing compiler warning
            }
        }

    } // namespace
} // namespace
