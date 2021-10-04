/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/stringutils.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Simulation::Settings, CModelSettings)

namespace BlackMisc::Simulation::Settings
{
    CModelSettings::CModelSettings()
    { }

    QString CModelSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("Allow exclude: %1").arg(boolToYesNo(this->m_allowExcludeModels));
    }

    QVariant CModelSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAllowExclude: return QVariant::fromValue(this->m_allowExcludeModels);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CModelSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CModelSettings>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAllowExclude: this->setAllowExcludedModels(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // ns
