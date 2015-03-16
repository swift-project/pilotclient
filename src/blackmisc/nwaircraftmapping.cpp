/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwaircraftmapping.h"
#include "propertyindex.h"
#include "variant.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Network
    {

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping(const QString &source, const QString &packageName, const QString &aircraftDesignator, const QString &airlineDesignator, const QString &model) :
            m_source(source.trimmed()), m_packageName(packageName.trimmed()), m_icao(aircraftDesignator, airlineDesignator), m_model(model, BlackMisc::Simulation::CAircraftModel::TypeModelMapping)
        { }

        /*
         * Convert to string
         */
        QString CAircraftMapping::convertToQString(bool i18n) const
        {
            QString s = QString(this->m_model.toQString(i18n)).append(' ').append(this->m_icao.toQString(i18n));
            return s;
        }

        /*
         * Model string?
         */
        bool CAircraftMapping::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->m_model.matchesModelString(modelString, sensitivity);
        }

        /*
         * Property by index
         */
        CVariant CAircraftMapping::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
            case IndexIcao:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
            case IndexPackageName:
                return QVariant::fromValue(this->m_packageName);
            case IndexSource:
                return QVariant::fromValue(this->m_source);
            default:
                return CValueObjectStdTuple::propertyByIndex(index);
            }
        }

        /*
         * Set property as index
         */
        void CAircraftMapping::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                this->m_model.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexIcao:
                this->m_icao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexPackageName:
                this->m_packageName = variant.toQString();
                break;
            case IndexSource:
                this->m_source = variant.toQString();
                break;
            default:
                CValueObjectStdTuple::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
