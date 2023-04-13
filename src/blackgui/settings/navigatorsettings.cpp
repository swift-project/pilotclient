/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "navigatorsettings.h"
#include "blackgui/guiutility.h"
#include "blackmisc/stringutils.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackGui;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackGui::Settings, CNavigatorSettings)

namespace BlackGui::Settings
{
    CNavigatorSettings::CNavigatorSettings()
    {}

    void CNavigatorSettings::reset()
    {}

    void CNavigatorSettings::setMargins(const QMargins &margins)
    {
        this->m_margins = CGuiUtility::marginsToString(margins);
    }

    QMargins CNavigatorSettings::getMargins() const
    {
        return CGuiUtility::stringToMargins(this->m_margins);
    }

    QByteArray CNavigatorSettings::getGeometry() const
    {
        return QByteArray::fromHex(this->m_geometry.toLatin1());
    }

    void CNavigatorSettings::setGeometry(const QByteArray &ba)
    {
        this->m_geometry = ba.toHex();
    }

    QString CNavigatorSettings::convertToQString(bool i18n) const
    {
        return convertToQString(", ", i18n);
    }

    QString CNavigatorSettings::convertToQString(const QString &separator, bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s("margins: ");
        s.append(this->m_margins);
        s.append(separator);
        s.append("frameless: ");
        s.append(boolToTrueFalse(this->m_frameless));
        s.append(separator);
        s.append("columns: ");
        s.append(this->m_columns);
        return s;
    }

    QVariant CNavigatorSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMargins:
            return QVariant::fromValue(this->m_margins);
        case IndexFrameless:
            return QVariant::fromValue(this->isFramless());
        case IndexColumns:
            return QVariant::fromValue(this->m_columns);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CNavigatorSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CNavigatorSettings>();
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMargins:
            this->m_margins = variant.toString();
            break;
        case IndexColumns:
            this->m_columns = variant.toInt();
            break;
        case IndexFrameless:
            this->m_frameless = variant.toBool();
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns
