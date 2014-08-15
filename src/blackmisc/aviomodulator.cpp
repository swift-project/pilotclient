/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/avioadfsystem.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"

using BlackMisc::PhysicalQuantities::CFrequency;
using BlackMisc::PhysicalQuantities::CFrequencyUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Toggle standby <-> active
         */
        template <class AVIO> void CModulator<AVIO>::toggleActiveStandby()
        {
            CFrequency a = this->m_frequencyActive;
            this->m_frequencyActive = this->m_frequencyStandby;
            this->m_frequencyStandby = a;
        }

        /*
         * Register metadata
         */
        template <class AVIO> void CModulator<AVIO>::registerMetadata()
        {
            qRegisterMetaType<AVIO>();
            qDBusRegisterMetaType<AVIO>();
        }

        /*
         * To JSON
         */
        template <class AVIO> QJsonObject CModulator<AVIO>::toJson() const
        {
            return BlackMisc::serializeJson(CModulator::jsonMembers(), TupleConverter<CModulator>::toTuple(*this));
        }

        /*
         * To JSON
         */
        template <class AVIO> void CModulator<AVIO>::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CModulator::jsonMembers(), TupleConverter<CModulator>::toTuple(*this));
        }

        /*
         * Property by index
         */
        template <class AVIO> QVariant CModulator<AVIO>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                return this->getFrequencyActive().propertyByIndex(index.copyFrontRemoved());
            case IndexStandbyFrequency:
                return this->getFrequencyStandby().propertyByIndex(index.copyFrontRemoved());
            case IndexEnabled:
                return QVariant(this->isEnabled());
            case IndexInputVolume:
                return QVariant(this->getVolumeInput());
            case IndexOutputVolume:
                return QVariant(this->getVolumeOutput());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Property by index
         */
        template <class AVIO> void CModulator<AVIO>::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->fromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                this->m_frequencyActive.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexStandbyFrequency:
                this->m_frequencyStandby.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexEnabled:
                this->setEnabled(variant.toBool());
                break;
            case IndexInputVolume:
                this->setVolumeInput(variant.toInt());
                break;
            case IndexOutputVolume:
                this->setVolumeOutput(variant.toInt());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        /*
         * Members
         */
        template <class AVIO> const QStringList &CModulator<AVIO>::jsonMembers()
        {
            return TupleConverter<CModulator>::jsonMembers();
        }

        /*
         * Equal operator ==
         */
        template <class AVIO> bool CModulator<AVIO>::operator ==(const CModulator<AVIO> &other) const
        {
            if (this == &other) return true;
            if (!CAvionicsBase::operator ==(other)) return false;
            return TupleConverter<CModulator>::toTuple(*this) == TupleConverter<CModulator>::toTuple(other);
        }

        /*
         * Equal operator !=
         */
        template <class AVIO> bool CModulator<AVIO>::operator !=(const CModulator<AVIO> &other) const
        {
            return !(other == (*this));
        }

        /*
         * To DBus
         */
        template <class AVIO> void CModulator<AVIO>::marshallToDbus(QDBusArgument &argument) const
        {
            CAvionicsBase::marshallToDbus(argument);
            argument << TupleConverter<CModulator>::toTuple(*this);
        }

        /*
         * From DBus
         */
        template <class AVIO> void CModulator<AVIO>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CAvionicsBase::unmarshallFromDbus(argument);
            argument >> TupleConverter<CModulator>::toTuple(*this);
        }

        /*
         * Compare
         */
        template <class AVIO> int CModulator<AVIO>::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CModulator &>(otherBase);
            int result = compare(TupleConverter<CModulator>::toTuple(*this), TupleConverter<CModulator>::toTuple(other));
            return result == 0 ? CAvionicsBase::compareImpl(otherBase) : result;
        }

        /*
         * Value hash
         */
        template <class AVIO> uint CModulator<AVIO>::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CAvionicsBase::getValueHash();
            hashs << qHash(TupleConverter<CModulator>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CModulator");
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CModulator<CComSystem>;
        template class CModulator<CNavSystem>;
        template class CModulator<CAdfSystem>;

    } // namespace
} // namespace
