/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dockwidgetsettings.h"
#include "blackgui/guiutility.h"
#include "blackmisc/stringutils.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui
{
    namespace Settings
    {
        CDockWidgetSettings::CDockWidgetSettings()
        { }

        void CDockWidgetSettings::resetMarginsToDefault()
        {
            // this->setMarginsWhenFloating(QMargins(0, 3, 15, 35)); // found by trial and error on windows
            this->setMarginsWhenFloating(QMargins(0, 0, 0, 0));
            this->setMarginsWhenFramelessFloating(QMargins(0, 0, 0, 0));
            this->setMarginsWhenDocked(QMargins(0, 0, 0, 0));
        }

        void CDockWidgetSettings::reset()
        {
            this->resetMarginsToDefault();
            this->m_geometry = "";
        }

        void CDockWidgetSettings::setMarginsWhenFramelessFloating(const QMargins &margins)
        {
            this->m_floatingFramelessMargins = CGuiUtility::marginsToString(margins);
        }

        QMargins CDockWidgetSettings::getMarginsWhenFramelessFloating() const
        {
            return CGuiUtility::stringToMargins(this->m_floatingFramelessMargins);
        }

        void CDockWidgetSettings::setMarginsWhenFloating(const QMargins &margins)
        {
            this->m_floatingMargins = CGuiUtility::marginsToString(margins);
        }

        QMargins CDockWidgetSettings::getMarginsWhenFloating() const
        {
            return CGuiUtility::stringToMargins(this->m_floatingMargins);
        }

        void CDockWidgetSettings::setMarginsWhenDocked(const QMargins &margins)
        {
            this->m_dockedMargins = CGuiUtility::marginsToString(margins);
        }

        QMargins CDockWidgetSettings::getMarginsWhenDocked() const
        {
            return CGuiUtility::stringToMargins(this->m_dockedMargins);
        }

        QByteArray CDockWidgetSettings::getGeometry() const
        {
            return byteArrayFromHexString(this->m_geometry);
        }

        void CDockWidgetSettings::setGeometry(const QByteArray &ba)
        {
            this->m_geometry = bytesToHexString(ba);
        }

        QString CDockWidgetSettings::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CDockWidgetSettings::convertToQString(const QString &separator, bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("floating: ");
            s.append(this->m_floatingMargins);
            s.append(separator);
            s.append("floating, frameless: ");
            s.append(this->m_floatingFramelessMargins);
            s.append(separator);
            s.append("docked: ");
            s.append(this->m_dockedMargins);
            s.append(separator);
            s.append("frameless: ");
            s.append(boolToTrueFalse(this->m_frameless));
            s.append(separator);
            s.append("floating: ");
            s.append(boolToTrueFalse(this->m_floating));
            return s;
        }

        CVariant CDockWidgetSettings::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFloatingMargins:
                return CVariant::fromValue(this->m_floatingMargins);
            case IndexFloatingFramelessMargins:
                return CVariant::fromValue(this->m_floatingFramelessMargins);
            case IndexDockedMargins:
                return CVariant::fromValue(this->m_dockedMargins);
            case IndexFrameless:
                return CVariant::fromValue(this->m_floating);
            case IndexFloating:
                return CVariant::fromValue(this->m_floating);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CDockWidgetSettings::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CDockWidgetSettings>(); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFloatingMargins:
                this->m_floatingMargins = variant.toQString();
                break;
            case IndexFloatingFramelessMargins:
                this->m_floatingFramelessMargins = variant.toQString();
                break;
            case IndexDockedMargins:
                this->m_dockedMargins = variant.toQString();
                break;
            case IndexFloating:
                this->m_floating = variant.toBool();
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
} // ns
