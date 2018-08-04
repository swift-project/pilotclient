/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelsettings.h"
#include "blackmisc/stringutils.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            CModelSettings::CModelSettings()
            { }

            QString CModelSettings::convertToQString(bool i18n) const
            {
                Q_UNUSED(i18n);
                static const QString ms("Allow exclude: %1");
                return ms.arg(boolToYesNo(this->m_allowExcludeModels));
            }

            CVariant CModelSettings::propertyByIndex(const CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                const ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexAllowExclude: return CVariant::fromValue(this->m_allowExcludeModels);
                default: return CValueObject::propertyByIndex(index);
                }
            }

            void CModelSettings::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
            {
                if (index.isMyself()) { (*this) = variant.to<CModelSettings>(); return; }
                const ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexAllowExclude: this->setAllowExcludedModels(variant.toBool()); break;
                default: CValueObject::setPropertyByIndex(index, variant); break;
                }
            }
        } // ns
    } // ns
} // ns
