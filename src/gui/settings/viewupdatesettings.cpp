// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewupdatesettings.h"

#include <QStringList>

#include "gui/guiutility.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::gui;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::gui::settings, CViewUpdateSettings)

namespace swift::gui::settings
{
    void CViewUpdateSettings::reset()
    {
        this->m_updateAircraft = CTime(10.0, CTimeUnit::s());
        this->m_updateAtc = CTime(10.0, CTimeUnit::s());
        this->m_updateRendering = CTime(10.0, CTimeUnit::s());
        this->m_updateUser = CTime(10.0, CTimeUnit::s());
    }

    bool CViewUpdateSettings::isValid() const
    {
        // too fast updates cause CPU overload
        static const CTime min(5.0, CTimeUnit::s());
        return this->m_updateAircraft >= min && this->m_updateAtc >= min && this->m_updateRendering >= min &&
               this->m_updateUser >= min;
    }

    QString CViewUpdateSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s("Atc: %1 Aircraft: %2 User: %3 Rendering %4");
        return s.arg(m_updateAtc.valueRounded(CTimeUnit::s(), 2))
            .arg(m_updateAircraft.valueRounded(CTimeUnit::s(), 2))
            .arg(m_updateUser.valueRounded(CTimeUnit::s(), 2))
            .arg(m_updateRendering.valueRounded(CTimeUnit::s(), 2));
    }

    QVariant CViewUpdateSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAircraft: return QVariant::fromValue(this->m_updateAircraft);
        case IndexAtc: return QVariant::fromValue(this->m_updateAtc);
        case IndexRendering: return QVariant::fromValue(this->m_updateRendering);
        case IndexUser: return QVariant::fromValue(this->m_updateUser);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CViewUpdateSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CViewUpdateSettings>();
            return;
        }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAircraft: this->m_updateAircraft = variant.value<CTime>(); break;
        case IndexAtc: this->m_updateAtc = variant.value<CTime>(); break;
        case IndexRendering: this->m_updateRendering = variant.value<CTime>(); break;
        case IndexUser: this->m_updateUser = variant.value<CTime>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::gui::settings
