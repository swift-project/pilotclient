// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/settings/modelsettings.h"

#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CModelSettings)

namespace swift::misc::simulation::settings
{
    CModelSettings::CModelSettings()
    {}

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
        if (index.isMyself())
        {
            (*this) = variant.value<CModelSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAllowExclude: this->setAllowExcludedModels(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::misc::simulation::settings
