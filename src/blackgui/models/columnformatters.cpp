/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columnformatters.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/rgbcolor.h"
#include "blackmisc/variant.h"

#include <QDate>
#include <QDateTime>
#include <QImage>
#include <QMetaType>
#include <QTime>
#include <QVariant>
#include <QScopedPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Models
    {
        Qt::ItemFlags CDefaultFormatter::flags(Qt::ItemFlags flags, bool editable) const
        {
            return editable ? (flags | Qt::ItemIsEditable) : (flags & ~Qt::ItemIsEditable);
        }

        CVariant CDefaultFormatter::displayRole(const CVariant &dataCVariant) const
        {
            return keepStandardTypesConvertToStringOtherwise(dataCVariant);
        }

        CVariant CDefaultFormatter::editRole(const CVariant &dataCVariant) const
        {
            return keepStandardTypesConvertToStringOtherwise(dataCVariant);
        }

        CVariant CDefaultFormatter::tooltipRole(const CVariant &value) const
        {
            if (static_cast<QMetaType::Type>(value.type()) == QMetaType::QString) { return value; }
            return value.toQString(m_useI18n);
        }

        CVariant CDefaultFormatter::decorationRole(const CVariant &dataCVariant) const
        {
            // direct return if type is already correct
            const QMetaType::Type type = static_cast<QMetaType::Type>(dataCVariant.type());

            if (type == QMetaType::QPixmap) { return dataCVariant; }
            if (type == QMetaType::QIcon)   { return dataCVariant; }

            // convert to pixmap
            if (type == QMetaType::QImage)
            {
                const QImage img = dataCVariant.value<QImage>();
                return CVariant::from(QPixmap::fromImage(img));
            }

            // Our CIcon class
            if (dataCVariant.canConvert<CIcon>())
            {
                const CIcon i = dataCVariant.value<CIcon>();
                return CVariant::from(i.toPixmap());
            }

            // nope
            return CVariant::from(QPixmap());
        }

        CVariant CDefaultFormatter::alignmentRole() const
        {
            if (this->hasAlignment())
            {
                return CVariant::from(m_alignment);
            }
            return CVariant::from(alignDefault()); // default
        }

        CVariant CDefaultFormatter::checkStateRole(const CVariant &value) const
        {
            bool b = value.toBool();
            Qt::CheckState cs = b ? Qt::Checked : Qt::Unchecked;
            return CVariant::fromValue(static_cast<int>(cs));
        }

        bool CDefaultFormatter::supportsRole(int role) const
        {
            // generally supported?
            if (role == Qt::TextAlignmentRole || role == Qt::UserRole) { return true; }

            // specific?
            return m_supportedRoles.contains(role);
        }

        CVariant CDefaultFormatter::data(int role, const CVariant &inputData) const
        {
            if (!this->supportsRole(role)) { return CVariant(); }
            const Qt::ItemDataRole roleEnum = static_cast<Qt::ItemDataRole>(role);

            // always supported
            if (roleEnum == Qt::TextAlignmentRole) return { alignmentRole() };

            // check
            if (role == Qt::UserRole) { return CDefaultFormatter::displayRole(inputData); } // just as data provider
            switch (roleEnum)
            {
            case Qt::DisplayRole: return displayRole(inputData); // formatted to standard types or string
            case Qt::EditRole: return editRole(inputData); // formatted to standard types or string
            case Qt::ToolTipRole: return tooltipRole(inputData); // formatted to string
            case Qt::DecorationRole: return decorationRole(inputData); // formatted as pixmap, icon, or color
            case Qt::CheckStateRole: return checkStateRole(inputData); // as Qt check state
            default: break;
            }
            return CVariant();
        }

        int CDefaultFormatter::alignDefault()
        {
            return alignLeftVCenter();
        }

        CVariant CDefaultFormatter::keepStandardTypesConvertToStringOtherwise(const CVariant &inputData) const
        {
            if (static_cast<QMetaType::Type>(inputData.type()) == QMetaType::QString) { return inputData; }
            if (static_cast<QMetaType::Type>(inputData.type()) == QMetaType::Bool)  { return inputData; }
            if (static_cast<QMetaType::Type>(inputData.type()) == QMetaType::Int) { return inputData; }
            return inputData.toQString(m_useI18n);
        }

        const CVariant &CDefaultFormatter::emptyStringVariant()
        {
            static const CVariant e = CVariant::from(QString());
            return e;
        }

        const CVariant &CDefaultFormatter::emptyPixmapVariant()
        {
            static const CVariant e = CVariant::from(QPixmap());
            return e;
        }

        CVariant CPixmapFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant);
            Q_ASSERT_X(false, "CPixmapFormatter", "this role should be disabled with pixmaps");
            return {};
        }

        CVariant CPixmapFormatter::tooltipRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.isNull()) { return {}; }
            if (dataCVariant.canConvert<CIcon>())
            {
                const CIcon icon = dataCVariant.value<CIcon>();
                return icon.getDescriptiveText();
            }
            return emptyStringVariant();
        }

        CVariant CPixmapFormatter::decorationRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.isNull()) { return {}; }
            if (m_maxWidth < 0 && m_maxHeight < 0) { return CDefaultFormatter::decorationRole(dataCVariant); }

            QPixmap pm;
            if (dataCVariant.canConvert<CIcon>())
            {
                const CIcon icon = dataCVariant.value<CIcon>();
                pm = icon.toPixmap();
            }

            if (pm.isNull()) { return {}; }
            const int pmw = pm.width();
            const int pmh = pm.height();

            if (m_maxHeight >= 0 && m_maxHeight < pmh)
            {
                return CVariant::fromValue(pm.scaledToHeight(m_maxHeight));
            }

            if (m_maxWidth >= 0 && m_maxWidth < pmw)
            {
                return CVariant::fromValue(pm.scaledToWidth(m_maxWidth));
            }

            return CVariant::fromValue(pm);
        }

        CVariant CValueObjectFormatter::displayRole(const CVariant &valueObject) const
        {
            return CVariant(valueObject.toQString(m_useI18n));
        }

        CVariant CValueObjectFormatter::decorationRole(const CVariant &valueObject) const
        {
            return CVariant(valueObject.toPixmap());
        }

        CDateTimeFormatter::CDateTimeFormatter(const QString &formatString, int alignment, bool i18n) :
            CDefaultFormatter(alignment, i18n, { Qt::DisplayRole }), m_formatString(formatString)
        {
            // void
        }

        CVariant CDateTimeFormatter::displayRole(const CVariant &dateTime) const
        {
            if (dateTime.isNull()) return {};
            if (static_cast<QMetaType::Type>(dateTime.type()) == QMetaType::QDateTime)
            {
                const QDateTime dt = dateTime.value<QDateTime>();
                return dt.toString(m_formatString);
            }
            else if (static_cast<QMetaType::Type>(dateTime.type()) == QMetaType::QDate)
            {
                const QDate d = dateTime.value<QDate>();
                return d.toString(m_formatString);
            }
            else if (static_cast<QMetaType::Type>(dateTime.type()) == QMetaType::QTime)
            {
                const QTime t = dateTime.value<QTime>();
                return t.toString(m_formatString);
            }
            else if (dateTime.isIntegral())
            {
                const QDateTime t = QDateTime::fromMSecsSinceEpoch(dateTime.value<qint64>());
                return t.toString(m_formatString);
            }
            else
            {
                Q_ASSERT_X(false, "formatQVariant", "No QDate, QTime or QDateTime");
                return {};
            }
        }

        CVariant CAirspaceDistanceFormatter::displayRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<CLength>())
            {
                // special treatment for some cases
                const CLength l = dataCVariant.value<CLength>();
                const bool valid = !l.isNull() && (l.isPositiveWithEpsilonConsidered() || l.isZeroEpsilonConsidered());
                return valid ? CPhysiqalQuantiyFormatter::displayRole(dataCVariant) : emptyStringVariant();
            }
            else
            {
                Q_ASSERT_X(false, "CAirspaceDistanceFormatter::formatQVariant", "No CLength class");
                return emptyStringVariant();
            }
        }

        CVariant CComFrequencyFormatter::displayRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<CFrequency>())
            {
                // special treatment for some cases
                const CFrequency f = dataCVariant.value<CFrequency>();
                if (CComSystem::isValidComFrequency(f))
                {
                    return CPhysiqalQuantiyFormatter::displayRole(dataCVariant);
                }
                return emptyStringVariant();
            }
            else
            {
                Q_ASSERT_X(false, "CAviationComFrequencyFormatter::formatQVariant", "No CFrequency class");
                return emptyStringVariant();
            }
        }

        CVariant CSpeedKtsFormatter::displayRole(const CVariant &dataCVariant) const
        {
            // special treatment for some cases
            const CSpeed s = dataCVariant.value<CSpeed>();
            if (!s.isNull() && (s.isPositiveWithEpsilonConsidered() || s.isZeroEpsilonConsidered()))
            {
                return CPhysiqalQuantiyFormatter::displayRole(dataCVariant);
            }
            return emptyStringVariant();
        }

        CVariant CStringFormatter::displayRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<QString>()) { return dataCVariant; }
            if (!dataCVariant.isValid()) { static const CVariant iv("invalid"); return iv; }
            return CVariant::from(QStringLiteral("Invalid type: '%1'").arg(dataCVariant.typeName()));
        }

        Qt::ItemFlags CDelegateFormatter::flags(Qt::ItemFlags flags, bool editable) const
        {
            flags = CDefaultFormatter::flags(flags, editable);
            return flags;
        }

        CVariant CBoolTextFormatter::displayRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<bool>())
            {
                const bool v = dataCVariant.toBool();
                return v ? CVariant(m_trueNameVariant) : CVariant(m_falseNameVariant);
            }
            Q_ASSERT_X(false, "CBoolTextFormatter", "no boolean value");
            return CVariant();
        }

        Qt::ItemFlags CBoolTextFormatter::flags(Qt::ItemFlags flags, bool editable) const
        {
            return CDefaultFormatter::flags(flags, editable);
        }

        CBoolLedFormatter::CBoolLedFormatter(int alignment) : CBoolLedFormatter("on", "off", alignment)
        { }

        CBoolLedFormatter::CBoolLedFormatter(const QString &onName, const QString &offName, int alignment) :
            CBoolTextFormatter(alignment, onName, offName, rolesDecorationAndToolTip())
        {
            // one time pixmap creation
            QScopedPointer<CLedWidget> led(createLedDefault());
            led->setOn(true);
            m_pixmapOnLedVariant = CVariant::fromValue(led->asPixmap());
            led->setOn(false);
            m_pixmapOffLedVariant = CVariant::fromValue(led->asPixmap());
        }

        CVariant CBoolLedFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant);
            Q_ASSERT_X(false, Q_FUNC_INFO, "this role should be disabled with led boolean");
            return CVariant();
        }

        CVariant CBoolLedFormatter::decorationRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<bool>())
            {
                const bool v = dataCVariant.toBool();
                return v ? m_pixmapOnLedVariant : m_pixmapOffLedVariant;
            }
            Q_ASSERT_X(false, "CBoolLedFormatter", "no boolean value");
            return CVariant();
        }

        CBoolIconFormatter::CBoolIconFormatter(int alignment) :
            CBoolIconFormatter(CIcons::StandardIconTick16, CIcons::StandardIconCross16, "on", "off", alignment)
        { }

        CBoolIconFormatter::CBoolIconFormatter(const QString &onName, const QString &offName, int alignment) :
            CBoolIconFormatter(CIcons::StandardIconTick16, CIcons::StandardIconCross16, onName, offName, alignment)
        { }

        CBoolIconFormatter::CBoolIconFormatter(CIcons::IconIndex onIcon, CIcons::IconIndex offIcon, const QString &onName, const QString &offName, int alignment) :
            CBoolIconFormatter(CIcon::iconByIndex(onIcon), CIcon::iconByIndex(offIcon), onName, offName, alignment)
        { }

        CBoolIconFormatter::CBoolIconFormatter(const CIcon &onIcon, const CIcon &offIcon, const QString &onName, const QString &offName, int alignment) :
            CBoolTextFormatter(alignment, onName, offName, rolesDecorationAndToolTip()),
            m_iconOnVariant(CVariant::fromValue(onIcon.toPixmap())), m_iconOffVariant(CVariant::fromValue(offIcon.toPixmap()))
        { }

        CVariant CBoolIconFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant)
            Q_ASSERT_X(false, "CBoolIconFormatter", "this role should be disabled with icon boolean");
            return CVariant();
        }

        CVariant CBoolIconFormatter::decorationRole(const CVariant &dataCVariant) const
        {
            if (dataCVariant.canConvert<bool>())
            {
                const bool v = dataCVariant.toBool();
                return v ? m_iconOnVariant : m_iconOffVariant;
            }
            Q_ASSERT_X(false, "CBoolIconFormatter", "no boolean value");
            return CVariant();
        }

        CVariant CBoolIconFormatter::tooltipRole(const CVariant &dataCVariant) const
        {
            return CBoolTextFormatter::displayRole(dataCVariant);
        }

        CVariant CAltitudeFormatter::displayRole(const CVariant &altitude) const
        {
            CAltitude alt(altitude.to<CAltitude>());
            if (m_flightLevel)
            {
                alt.toFlightLevel();
            }
            else
            {
                alt.switchUnit(m_unit);
            }
            return alt.toQString(m_useI18n);
        }

        CColorFormatter::CColorFormatter(int alignment, bool i18n) : CDefaultFormatter(alignment, i18n, rolesDecorationAndToolTip())
        {}

        CVariant CColorFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant)
            Q_ASSERT_X(false, Q_FUNC_INFO, "this role should be disabled with RGB color");
            return CVariant();
        }

        CVariant CColorFormatter::decorationRole(const CVariant &dataCVariant) const
        {
            const CRgbColor rgbColor(dataCVariant.to<CRgbColor>());
            if (!rgbColor.isValid())  { return emptyPixmapVariant(); }
            return CVariant::fromValue(CIcon(rgbColor.toIcon()).toPixmap());
        }

        CVariant CColorFormatter::tooltipRole(const CVariant &dataCVariant) const
        {
            static const CVariant empty(CVariant::fromValue(QPixmap()));
            const CRgbColor rgbColor(dataCVariant.to<CRgbColor>());
            if (!rgbColor.isValid())  { return emptyStringVariant(); }
            return rgbColor.hex(true);
        }

        CVariant CIntegerFormatter::displayRole(const CVariant &expectedInteger) const
        {
            bool ok = false;
            switch (expectedInteger.type())
            {
            case QMetaType::LongLong:
                {
                    const qlonglong ll = expectedInteger.toLongLong(&ok);
                    if (ok) { return QString::number(ll); }
                    break;
                }
            case QMetaType::ULongLong:
                {
                    const qulonglong ll = expectedInteger.toULongLong(&ok);
                    if (ok) { return QString::number(ll); }
                    break;
                }
            default:
                break;
            }

            const int i = expectedInteger.toInt(&ok);
            if (ok) { return QString::number(i); }
            return CVariant();
        }

        CVariant CEmptyFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant);
            static const CVariant empty("");
            return empty;
        }

        CVariant CIncognitoFormatter::displayRole(const CVariant &dataCVariant) const
        {
            Q_UNUSED(dataCVariant);
            static const CVariant masked("******");
            return masked;
        }

    } // namespace
} // namespace
