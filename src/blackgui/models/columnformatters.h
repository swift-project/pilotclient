/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COLUMNFORMATTERS_H
#define BLACKGUI_COLUMNFORMATTERS_H

#include "blackgui/led.h"
#include "blackmisc/pqallquantities.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/icon.h"
#include <QDateTime>
#include <QDate>
#include <QTime>

namespace BlackGui
{
    namespace Models
    {
        //! Column formatter default implementation, also serving as interface
        class CDefaultFormatter
        {
        public:
            //! Constructor
            CDefaultFormatter(int alignment = alignDefault(), bool i18n = true, const QList<int> &supportedRoles = { Qt::DisplayRole }) :
                m_supportedRoles(supportedRoles), m_alignment(alignment), m_useI18n(i18n) {}

            //! Virtual destructor
            virtual ~CDefaultFormatter() {}

            //! Value provided as QVariant, formatter converts to QString.
            //! Used with Qt::DisplayRole displaying a text.
            virtual QVariant displayRole(const QVariant &dataQVariant) const;

            //! Value provided as QVariant, formatter converts to QString.
            //! Used with Qt::ToolTipRole displaying a text.
            virtual QVariant tooltipRole(const QVariant &value) const
            {
                return displayRole(value);
            }

            //! Value provided as QVariant, formatted as icon (Qt docu: "The data to be rendered as a decoration in the form of an icon").
            //! Used with Qt::DecorationRole displaying an icon, method returns pixmap, icon, or color (see docu)
            virtual QVariant decorationRole(const QVariant &dataQVariant) const;

            //! Qt::Alignment (as QVariant)
            virtual QVariant alignmentRole() const;

            //! Alignment available?
            virtual bool hasAlignment() const { return m_alignment >= 0; }

            //! Receives QVariant of column data, and returns QVariant wrapping string, pixmap, or other values depending on role
            virtual QVariant data(int role, const QVariant &inputData) const;

            //! Default value
            static int alignDefault() { return alignLeftVCenter(); }

            //! Align left/vertically centered
            static int alignLeftVCenter() { return Qt::AlignVCenter | Qt::AlignLeft; }

            //! Align centered
            static int alignCentered() { return Qt::AlignVCenter | Qt::AlignHCenter; }

            //! Align right/vertically centered
            static int alignRightVCenter() { return Qt::AlignVCenter | Qt::AlignRight; }

            //! Display on role
            static const QList<int> &roleDisplay() { static const QList<int> r({ Qt::DisplayRole}); return r; }

            //! Display on role
            static const QList<int> &roleDecorationAndToolTip() { static const QList<int> r({ Qt::DecorationRole, Qt::ToolTipRole}); return r; }

        protected:
            QList<int>  m_supportedRoles = QList<int>({ Qt::DisplayRole});  //!< supports decoration role
            int  m_alignment      = -1;     //!< alignment horizontal/vertically / Qt::Alignment
            bool m_useI18n        = true;   //!< i18n?
        };

        //! Pixmap formatter
        class CPixmapFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CPixmapFormatter(int alignment = alignDefault(), const QList<int> &supportedRoles = roleDecorationAndToolTip()) : CDefaultFormatter(alignment, false, supportedRoles) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual QVariant tooltipRole(const QVariant &dataQVariant) const override;
        };

        //! String formatter, if known the variant already contains the appropriate string
        class CStringFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CStringFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;
        };

        //! Bool value, format as text
        class CBoolTextFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CBoolTextFormatter(int alignment = alignDefault(), const QString &trueName = "true", const QString &falseName = "false", const QList<int> &supportedRoles = roleDisplay()) :
                CDefaultFormatter(alignment, false, supportedRoles), m_trueName(trueName), m_falseName(falseName) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;

        protected:
            QString m_trueName = "true";   //!< displayed when true
            QString m_falseName = "false"; //!< displayed when false
        };

        //! Format as bool LED value
        class CBoolLedFormatter : public CBoolTextFormatter
        {
        public:

            //! Constructor
            CBoolLedFormatter(int alignment = alignDefault());

            //! Constructor
            CBoolLedFormatter(const QString &onName, const QString &offName, int alignment = alignDefault());

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;

            //! Display the LED
            virtual QVariant decorationRole(const QVariant &dataQVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual QVariant tooltipRole(const QVariant &dataQVariant) const override
            {
                return CBoolTextFormatter::displayRole(dataQVariant);
            }

            //! Default LED
            static BlackGui::CLedWidget *ledDefault()
            {
                return new BlackGui::CLedWidget(false, BlackGui::CLedWidget::Yellow, BlackGui::CLedWidget::Black, BlackGui::CLedWidget::Rounded);
            }

        protected:
            QPixmap m_pixmapOnLed;  //!< Pixmap used when on
            QPixmap m_pixmapOffLed; //!< Pixmap used when off
        };

        //! Format as bool pixmap
        class CBoolIconFormatter : public CBoolTextFormatter
        {
        public:

            //! Constructor
            CBoolIconFormatter(int alignment = alignDefault());

            //! Constructor
            CBoolIconFormatter(const BlackMisc::CIcon &onIcon, const BlackMisc::CIcon &offIcon, const QString &onName, const QString &offName, int alignment = alignDefault());

            //! Constructor
            CBoolIconFormatter(BlackMisc::CIcons::IconIndex onIcon, BlackMisc::CIcons::IconIndex offIcon, const QString &onName, const QString &offName, int alignment = alignDefault());

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;

            //! Display the icon
            virtual QVariant decorationRole(const QVariant &dataQVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual QVariant tooltipRole(const QVariant &dataQVariant) const override
            {
                return CBoolTextFormatter::displayRole(dataQVariant);
            }

        protected:
            BlackMisc::CIcon m_iconOn;  //!< Used when on
            BlackMisc::CIcon m_iconOff; //!< Used when off
        };

        //! Default formatter when column contains CValueObject
        class CValueObjectFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CValueObjectFormatter(int alignment = alignDefault(), bool i18n = true, QList<int> supportedRoles = {Qt::DisplayRole}) : CDefaultFormatter(alignment, i18n, supportedRoles) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &valueObject) const override;

            //! \copydoc CDefaultFormatter::asPixmap
            virtual QVariant decorationRole(const QVariant &valueObject) const;
        };

        //! Formatter when column contains QDateTime, QDate or QTime
        class CDateTimeFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CDateTimeFormatter(const QString formatString = formatYmd(), int alignment = alignDefault(), bool i18n = true) :
                CDefaultFormatter(alignment, i18n, { Qt::DisplayRole }), m_formatString(formatString) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dateTime) const override;

            //! Year month day
            static const QString &formatYmd() { static const QString f = "yyyy-MM-dd"; return f; }

            //! Year month day hour minute
            static const QString &formatYmdhm() { static const QString f = "yyyy-MM-dd HH:mm"; return f; }

            //! Hour minute
            static const QString &formatHm() { static const QString f = "HH:mm"; return f; }

            //! Hour minute second
            static const QString &formatHms() { static const QString f = "HH:mm:ss"; return f; }

        private:
            QString m_formatString = "yyyy-MM-dd HH:mm"; //!< how the value is displayed
        };

        //! Formatter for physical quantities
        template<class MU, class PQ> class CPhysiqalQuantiyFormatter : public CValueObjectFormatter
        {
        public:
            //! Constructor
            CPhysiqalQuantiyFormatter(MU unit = MU::defaultUnit(), int digits = 2, int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true, QList<int> supportedRoles = { Qt::DisplayRole }) : CValueObjectFormatter(alignment, i18n, supportedRoles), m_unit(unit), m_digits(digits), m_withUnit(withUnit) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &physicalQuantity) const override
            {
                if (physicalQuantity.canConvert<PQ>())
                {
                    PQ pq = physicalQuantity.value<PQ>();
                    if (!m_unit.isNull())
                    {
                        pq.switchUnit(m_unit);
                    }
                    return pq.valueRoundedWithUnit(m_digits, m_useI18n);
                }
                else
                {
                    Q_ASSERT_X(false, "CPhysiqalQuantiyFormatter::displayRole", "No CPhysicalQuantity class");
                    return "";
                }
            }

            //! Set unit
            virtual void setUnit(const MU &unit) { m_unit = unit; }

            //! Enable unit display
            virtual void enableUnit(bool enable) { m_withUnit = enable; }

            //! Digits
            virtual void setDigits(int digits) { m_digits = digits; }

        protected:
            MU   m_unit; //!< unit
            int  m_digits = 2; //!< digits
            bool m_withUnit = true; //!< format with unit?
        };

        //! COM frequencies
        class CComFrequencyFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CFrequencyUnit, BlackMisc::PhysicalQuantities::CFrequency>
        {
        public:
            //! Constructor
            CComFrequencyFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;
        };

        //! Angle in degrees
        class CAngleDegreeFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CAngleUnit, BlackMisc::PhysicalQuantities::CAngle>
        {
        public:
            //! Constructor
            CAngleDegreeFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 0, alignment, withUnit, i18n) {}
        };

        //! Latitude or Longitude formatter
        class CLatLonFormatter : public CValueObjectFormatter
        {
        public:
            //! Constructor
            CLatLonFormatter(int alignment = alignRightVCenter()) : CValueObjectFormatter(alignment) {}
        };

        //! Airspace distance
        class CAirspaceDistanceFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CLengthUnit, BlackMisc::PhysicalQuantities::CLength>
        {
        public:
            //! Constructor
            CAirspaceDistanceFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CLengthUnit::NM(), 1, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;
        };

        //! Airspace distance
        class CAircraftSpeedFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CSpeedUnit, BlackMisc::PhysicalQuantities::CSpeed>
        {
        public:
            //! Constructor
            CAircraftSpeedFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CSpeedUnit::kts(), 0, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual QVariant displayRole(const QVariant &dataQVariant) const override;
        };

    } // namespace
} // namespace

#endif // guard
