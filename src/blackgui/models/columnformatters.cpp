/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "columnformatters.h"
#include "blackmisc/geolatitude.h"
#include "blackmisc/variant.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {

        QVariant CDefaultFormatter::displayRole(const QVariant &dataQVariant) const
        {
            // seems to be absurd, but calls the correct methods on CValueObjects
            // so QVariant -> QString -> QVariant is correct
            if (static_cast<QMetaType::Type>(dataQVariant.type()) == QMetaType::QString) return dataQVariant; // shortcut
            return BlackMisc::qVariantToString(dataQVariant, m_useI18n);
        }

        QVariant CDefaultFormatter::decorationRole(const QVariant &dataQVariant) const
        {
            // direct return if type is already correct
            if (static_cast<QMetaType::Type>(dataQVariant.type()) == QMetaType::QPixmap)
            {
                return dataQVariant;
            }
            else if (static_cast<QMetaType::Type>(dataQVariant.type()) == QMetaType::QIcon)
            {
                return dataQVariant;
            }

            // convert to pixmap
            if (static_cast<QMetaType::Type>(dataQVariant.type()) == QMetaType::QImage)
            {
                QImage img = dataQVariant.value<QImage>();
                return QPixmap::fromImage(img);
            }

            // Our CIcon class
            if (dataQVariant.canConvert<BlackMisc::CIcon>())
            {
                BlackMisc::CIcon i = dataQVariant.value<BlackMisc::CIcon>();
                return i.toPixmap();
            }

            // nope
            return QPixmap();
        }

        QVariant CDefaultFormatter::alignmentRole() const
        {
            if (!this->hasAlignment())
            {
                return QVariant(alignDefault()); // default
            }
            else
            {
                return QVariant(m_alignment);
            }
        }

        QVariant CDefaultFormatter::data(int role, const QVariant &inputData) const
        {
            Qt::ItemDataRole roleEnum = static_cast<Qt::ItemDataRole>(role);

            // always supported
            if (roleEnum == Qt::TextAlignmentRole) return { alignmentRole() };

            // checked
            if (this->m_supportedRoles.isEmpty()) return QVariant();
            if (!this->m_supportedRoles.contains(role)) return QVariant();
            switch (roleEnum)
            {
            case Qt::DisplayRole:
                // formatted to string
                return displayRole(inputData);
            case Qt::ToolTipRole:
                // formatted to string
                return tooltipRole(inputData);
            case Qt::DecorationRole:
                // formatted as pixmap, icon, or color
                return decorationRole(inputData);
            default:
                break;
            }
            return QVariant();
        }

        QVariant CPixmapFormatter::displayRole(const QVariant &dataQVariant) const
        {
            Q_UNUSED(dataQVariant);
            Q_ASSERT_X(false, "CPixmapFormatter", "this role should be disabled with pixmaps");
            return QVariant();
        }

        QVariant CPixmapFormatter::tooltipRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.isNull()) return "";
            if (dataQVariant.canConvert<BlackMisc::CIcon>())
            {
                BlackMisc::CIcon icon = dataQVariant.value<BlackMisc::CIcon>();
                return icon.getDescriptiveText();
            }
            return "";
        }

        QVariant CValueObjectFormatter::displayRole(const QVariant &valueObject) const
        {
            if (valueObject.isNull()) return QVariant();
            const BlackMisc::CValueObject *cvo = BlackMisc::CValueObject::fromQVariant(valueObject);
            Q_ASSERT(cvo);
            if (!cvo) return QVariant();
            return QVariant(cvo->toQString(m_useI18n));
        }

        QVariant CValueObjectFormatter::decorationRole(const QVariant &valueObject) const
        {
            if (valueObject.isNull()) return "";
            const BlackMisc::CValueObject *cvo = BlackMisc::CValueObject::fromQVariant(valueObject);
            Q_ASSERT(cvo);
            if (!cvo) return QPixmap();
            return QVariant(cvo->toPixmap());
        }

        QVariant CDateTimeFormatter::displayRole(const QVariant &dateTime) const
        {
            if (dateTime.isNull()) return "";
            if (dateTime.type() == QMetaType::QDateTime)
            {
                QDateTime dt = dateTime.value<QDateTime>();
                return dt.toString(m_formatString);
            }
            else if (dateTime.type() == QMetaType::QDate)
            {
                QDate d = dateTime.value<QDate>();
                return d.toString(m_formatString);
            }
            else if (dateTime.type() == QMetaType::QTime)
            {
                QTime t = dateTime.value<QTime>();
                return t.toString(m_formatString);
            }
            else
            {
                Q_ASSERT_X(false, "formatQVariant", "No QDate, QTime or QDateTime");
                return "";
            }
        }

        QVariant CAirspaceDistanceFormatter::displayRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<BlackMisc::PhysicalQuantities::CLength>())
            {
                // special treatment for some cases
                BlackMisc::PhysicalQuantities::CLength l = dataQVariant.value<BlackMisc::PhysicalQuantities::CLength>();
                if (!l.isNull() && (l.isPositiveWithEpsilonConsidered() || l.isZeroEpsilonConsidered()))
                {
                    return CPhysiqalQuantiyFormatter::displayRole(dataQVariant);
                }
                else
                {
                    return "";
                }
            }
            else
            {
                Q_ASSERT_X(false, "CAirspaceDistanceFormatter::formatQVariant", "No CLength class");
                return "";
            }
        }

        QVariant CComFrequencyFormatter::displayRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<BlackMisc::PhysicalQuantities::CFrequency>())
            {
                // speical treatment for some cases
                BlackMisc::PhysicalQuantities::CFrequency f = dataQVariant.value<BlackMisc::PhysicalQuantities::CFrequency>();
                if (BlackMisc::Aviation::CComSystem::isValidComFrequency(f))
                {
                    return CPhysiqalQuantiyFormatter::displayRole(dataQVariant);
                }
                else
                {
                    return "";
                }
            }
            else
            {
                Q_ASSERT_X(false, "CAviationComFrequencyFormatter::formatQVariant", "No CFrequency class");
                return "";
            }
        }

        QVariant CAircraftSpeedFormatter::displayRole(const QVariant &dataQVariant) const
        {
            // special treatment for some cases
            BlackMisc::PhysicalQuantities::CSpeed s = dataQVariant.value<BlackMisc::PhysicalQuantities::CSpeed>();
            if (!s.isNull() && (s.isPositiveWithEpsilonConsidered() || s.isZeroEpsilonConsidered()))
            {
                return CPhysiqalQuantiyFormatter::displayRole(dataQVariant);
            }
            else
            {
                return "";
            }
        }

        QVariant CStringFormatter::displayRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<QString>()) { return dataQVariant; }
            Q_ASSERT_X(false, "CStringFormatter", "no string value");
            return QVariant();
        }

        QVariant CBoolTextFormatter::displayRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<bool>())
            {
                bool v = dataQVariant.toBool();
                return v ? QVariant(m_trueName) : QVariant(m_falseName);
            }
            Q_ASSERT_X(false, "CBoolTextFormatter", "no boolean value");
            return QVariant();
        }

        CBoolLedFormatter::CBoolLedFormatter(int alignment) : CBoolLedFormatter("on", "off", alignment)
        { }

        CBoolLedFormatter::CBoolLedFormatter(const QString &onName, const QString &offName, int alignment) :
            CBoolTextFormatter(alignment, onName, offName, roleDecorationAndToolTip())
        {
            CLedWidget *led = ledDefault();
            led->setOn(true);
            this->m_pixmapOnLed = led->asPixmap();
            led->setOn(false);
            this->m_pixmapOffLed = led->asPixmap();
            delete led;
        }

        QVariant CBoolLedFormatter::displayRole(const QVariant &dataQVariant) const
        {
            Q_UNUSED(dataQVariant);
            Q_ASSERT_X(false, "CBoolLedFormatter", "this role should be disabled with led boolean");
            return QVariant();
        }

        QVariant CBoolLedFormatter::decorationRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<bool>())
            {
                bool v = dataQVariant.toBool();
                return v ? m_pixmapOnLed : m_pixmapOffLed;
            }
            Q_ASSERT_X(false, "CBoolLedFormatter", "no boolean value");
            return QVariant();
        }

        CBoolIconFormatter::CBoolIconFormatter(int alignment) :
            CBoolIconFormatter(CIcons::StandardIconTick, CIcons::StandardIconEmpty16, "on", "off", alignment)
        { }

        CBoolIconFormatter::CBoolIconFormatter(CIcons::IconIndex onIcon, CIcons::IconIndex offIcon, const QString &onName, const QString &offName, int alignment) :
            CBoolIconFormatter(CIconList::iconForIndex(onIcon), CIconList::iconForIndex(offIcon), onName, offName, alignment)
        { }

        CBoolIconFormatter::CBoolIconFormatter(const CIcon &onIcon, const CIcon &offIcon, const QString &onName, const QString &offName, int alignment) :
            CBoolTextFormatter(alignment, onName, offName, roleDecorationAndToolTip()), m_iconOn(onIcon), m_iconOff(offIcon)
        {
            this->m_iconOn.setDescriptiveText(onName);
            this->m_iconOff.setDescriptiveText(offName);
        }

        QVariant CBoolIconFormatter::displayRole(const QVariant &dataQVariant) const
        {
            Q_UNUSED(dataQVariant);
            Q_ASSERT_X(false, "CBoolIconFormatter", "this role should be disabled with icon boolean");
            return QVariant();
        }

        QVariant CBoolIconFormatter::decorationRole(const QVariant &dataQVariant) const
        {
            if (dataQVariant.canConvert<bool>())
            {
                bool v = dataQVariant.toBool();
                return v ? m_iconOn.toPixmap() : m_iconOff.toPixmap();
            }
            Q_ASSERT_X(false, "CBoolIconFormatter", "no boolean value");
            return QVariant();
        }
    }
}
