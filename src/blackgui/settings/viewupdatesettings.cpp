/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "viewupdatesettings.h"
#include "blackgui/guiutility.h"
#include "blackmisc/stringutils.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackGui;

namespace BlackGui
{
    namespace Settings
    {
        CViewUpdateSettings::CViewUpdateSettings()
        { }

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
            return
                this->m_updateAircraft >= min &&
                this->m_updateAtc >= min &&
                this->m_updateRendering >= min &&
                this->m_updateUser >= min;
        }

        QString CViewUpdateSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            const QString s("Atc: %1 Aircraft: %2 User: %3 Rendering %4");
            return s.arg(m_updateAtc.valueRounded(CTimeUnit::s(), 2)).arg(m_updateAircraft.valueRounded(CTimeUnit::s(), 2))
                   .arg(m_updateUser.valueRounded(CTimeUnit::s(), 2)).arg(m_updateRendering.valueRounded(CTimeUnit::s(), 2));
        }

        QVariant CViewUpdateSettings::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraft:
                return QVariant::fromValue(this->m_updateAircraft);
            case IndexAtc:
                return QVariant::fromValue(this->m_updateAtc);
            case IndexRendering:
                return QVariant::fromValue(this->m_updateRendering);
            case IndexUser:
                return QVariant::fromValue(this->m_updateUser);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CViewUpdateSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CViewUpdateSettings>(); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraft:
                this->m_updateAircraft = variant.value<CTime>();
                break;
            case IndexAtc:
                this->m_updateAtc = variant.value<CTime>();
                break;
            case IndexRendering:
                this->m_updateRendering = variant.value<CTime>();
                break;
            case IndexUser:
                this->m_updateUser = variant.value<CTime>();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // ns
} // ns
