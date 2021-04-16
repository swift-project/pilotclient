/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_COLUMNFORMATTERS_H
#define BLACKGUI_MODELS_COLUMNFORMATTERS_H

#include "blackgui/blackguiexport.h"
#include "blackgui/led.h"
#include "blackmisc/icon.h"
#include "blackmisc/icons.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/variant.h"

#include <QFlags>
#include <QList>
#include <QPixmap>
#include <QString>
#include <Qt>
#include <QtGlobal>

namespace BlackGui
{
    namespace Models
    {
        //! Column formatter default implementation, also serving as interface
        class BLACKGUI_EXPORT CDefaultFormatter
        {
        public:
            //! Constructor
            CDefaultFormatter(int alignment = alignDefault(), bool i18n = true, const QList<int> &supportedRoles = { Qt::DisplayRole }) :
                m_supportedRoles(supportedRoles), m_alignment(alignment), m_useI18n(i18n)
            {}

            //! Virtual destructor
            virtual ~CDefaultFormatter() {}

            //! Copy constructor
            CDefaultFormatter(const CDefaultFormatter &) = default;

            //! Copy assignment operator
            CDefaultFormatter &operator =(const CDefaultFormatter &) = default;

            //! Flags
            virtual Qt::ItemFlags flags(Qt::ItemFlags flags, bool editable) const;

            //! Value provided as CVariant, formatter converts to standard types or string.
            //! Used with Qt::DisplayRole displaying a text.
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const;

            //! Value provided as CVariant, formatter converts to standard types or string.
            //! Used with Qt::DisplayRole displaying a text.
            virtual BlackMisc::CVariant editRole(const BlackMisc::CVariant &dataCVariant) const;

            //! Value provided as CVariant, formatter converts to QString.
            //! Used with Qt::ToolTipRole displaying a text.
            virtual BlackMisc::CVariant tooltipRole(const BlackMisc::CVariant &value) const;

            //! Value provided as CVariant, formatted as icon (Qt docu: "The data to be rendered as a decoration in the form of an icon").
            //! Used with Qt::DecorationRole displaying an icon, method returns pixmap, icon, or color (see docu)
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &dataCVariant) const;

            //! Qt::Alignment (as CVariant)
            virtual BlackMisc::CVariant alignmentRole() const;

            //! Value provided as CVariant (expecting a bool), returning as Qt::CheckStae
            virtual BlackMisc::CVariant checkStateRole(const BlackMisc::CVariant &value) const;

            //! Alignment available?
            virtual bool hasAlignment() const { return m_alignment >= 0; }

            //! Is given role supported by formatter
            bool supportsRole(int role) const;

            //! Receives CVariant of column data, and returns CVariant wrapping string, pixmap, or other values depending on role
            virtual BlackMisc::CVariant data(int role, const BlackMisc::CVariant &inputData) const;

            //! Default value
            static int alignDefault();

            //! Align left/vertically centered
            static int alignLeftVCenter() { return Qt::AlignVCenter | Qt::AlignLeft; }

            //! Align left/vertically on top
            static int alignLeftTop() { return Qt::AlignTop | Qt::AlignLeft; }

            //! Align centered
            static int alignCentered() { return Qt::AlignVCenter | Qt::AlignHCenter; }

            //! Align right/vertically centered
            static int alignRightVCenter() { return Qt::AlignVCenter | Qt::AlignRight; }

            //! Display role
            static const QList<int> &roleDisplay() { static const QList<int> r({ Qt::DisplayRole}); return r; }

            //! Display role
            static const QList<int> &rolesDisplayAndEdit() { static const QList<int> r({ Qt::DisplayRole, Qt::EditRole}); return r; }

            //! Decoration + ToolTip role
            static const QList<int> &rolesDecorationAndToolTip() { static const QList<int> r({ Qt::DecorationRole, Qt::ToolTipRole}); return r; }

            //! CheckState role
            static const QList<int> &roleCheckState() { static const QList<int> r({ Qt::CheckStateRole}); return r; }

            //! No roles
            static const QList<int> &rolesNone() { static const QList<int> r; return r; }

        protected:
            //! Standard conversion
            virtual BlackMisc::CVariant keepStandardTypesConvertToStringOtherwise(const BlackMisc::CVariant &inputData) const;

            //! Empty string CVariant
            static const BlackMisc::CVariant &emptyStringVariant();

            //! Empty pixmap CVariant
            static const BlackMisc::CVariant &emptyPixmapVariant();

            QList<int>  m_supportedRoles = roleDisplay();  //!< supports decoration roles
            int  m_alignment = -1;     //!< alignment horizontal/vertically / Qt::Alignment
            bool m_useI18n   = true;   //!< i18n?
        };

        //! Pixmap formatter
        class CPixmapFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CPixmapFormatter(int alignment = alignDefault(), const QList<int> &supportedRoles = rolesDecorationAndToolTip()) : CDefaultFormatter(alignment, false, supportedRoles) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual BlackMisc::CVariant tooltipRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::decorationRole
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! Width/height
            //! @{
            int getMaxWidth()  const { return m_maxWidth; }
            int getMaxHeight() const { return m_maxHeight; }
            void setMaxWidth(int w)  { m_maxWidth = w; }
            void setMaxHeight(int h) { m_maxHeight = h; }
            //! @}

        private:
            int m_maxWidth  = -1;
            int m_maxHeight = -1;
        };

        //! String formatter, if known the variant already contains the appropriate string
        class CStringFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CStringFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
        };

        //! Just returns a empty "" value
        class CEmptyFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CEmptyFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
        };

        //! Just returns a empty "" value
        class CIncognitoFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CIncognitoFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
        };

        //! Layout will be defined by a delegate
        class CDelegateFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CDelegateFormatter(int alignment = alignCentered(), const QList<int> &supportedRoles = rolesDecorationAndToolTip()) :
                CDefaultFormatter(alignment, false, supportedRoles) {}

            //! \copydoc CDefaultFormatter::flags
            virtual Qt::ItemFlags flags(Qt::ItemFlags flags, bool editable) const override;
        };

        //! Bool value, format as text
        class CBoolTextFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CBoolTextFormatter(int alignment = alignDefault(), const QString &trueName = "true", const QString &falseName = "false", const QList<int> &supportedRoles = roleDisplay()) :
                CDefaultFormatter(alignment, false, supportedRoles),
                m_trueNameVariant(BlackMisc::CVariant::from(trueName)),
                m_falseNameVariant(BlackMisc::CVariant::from(falseName)) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::flags
            virtual Qt::ItemFlags flags(Qt::ItemFlags flags, bool editable) const override;

        protected:
            const BlackMisc::CVariant m_trueNameVariant  = "true";  //!< displayed when true
            const BlackMisc::CVariant m_falseNameVariant = "false"; //!< displayed when false
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
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! Display the LED
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual BlackMisc::CVariant tooltipRole(const BlackMisc::CVariant &dataCVariant) const override
            {
                return CBoolTextFormatter::displayRole(dataCVariant);
            }

        protected:
            BlackMisc::CVariant m_pixmapOnLedVariant;  //!< Pixmap used when on
            BlackMisc::CVariant m_pixmapOffLedVariant; //!< Pixmap used when off

        private:
            //! Default LED
            static CLedWidget *createLedDefault()
            {
                return new CLedWidget(false, CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Rounded);
            }
        };

        //! Format as bool pixmap
        class CBoolIconFormatter : public CBoolTextFormatter
        {
        public:

            //! Constructor
            CBoolIconFormatter(int alignment = alignCentered());

            //! Constructor
            CBoolIconFormatter(const QString &onName, const QString &offName, int alignment = alignCentered());

            //! Constructor
            CBoolIconFormatter(const BlackMisc::CIcon &onIcon, const BlackMisc::CIcon &offIcon, const QString &onName, const QString &offName, int alignment = alignCentered());

            //! Constructor
            CBoolIconFormatter(BlackMisc::CIcons::IconIndex onIcon, BlackMisc::CIcons::IconIndex offIcon, const QString &onName, const QString &offName, int alignment = alignCentered());

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! Display the icon
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual BlackMisc::CVariant tooltipRole(const BlackMisc::CVariant &dataCVariant) const override;

        protected:
            const BlackMisc::CVariant m_iconOnVariant;  //!< Used when on
            const BlackMisc::CVariant m_iconOffVariant; //!< Used when off
        };

        //! Default formatter when column contains CValueObject
        class CValueObjectFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CValueObjectFormatter(int alignment = alignDefault(), bool i18n = true, const QList<int> &supportedRoles = roleDisplay()) : CDefaultFormatter(alignment, i18n, supportedRoles) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &valueObject) const override;

            //! \copydoc CDefaultFormatter::decorationRole
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &valueObject) const override;
        };

        //! Formatter when column contains QDateTime, QDate or QTime
        class CDateTimeFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CDateTimeFormatter(const QString &formatString = formatYmd(), int alignment = alignDefault(), bool i18n = true);

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dateTime) const override;

            //! Year month day
            static const QString &formatYmd() { static const QString f = "yyyy-MM-dd"; return f; }

            //! Year month day hour minute
            static const QString &formatYmdhm() { static const QString f = "yyyy-MM-dd HH:mm"; return f; }

            //! Hour minute
            static const QString &formatHm() { static const QString f = "HH:mm"; return f; }

            //! Hour minute second
            static const QString &formatHms() { static const QString f = "HH:mm:ss"; return f; }

            //! Hour minute second and milliseconds
            static const QString &formatHmsz() { static const QString f = "HH:mm:ss.zzz"; return f; }

        private:
            const QString m_formatString = "yyyy-MM-dd HH:mm"; //!< how the value is displayed
        };

        //! Formatter when column contains an integer
        class CIntegerFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CIntegerFormatter(int alignment = alignRightVCenter()) : CDefaultFormatter(alignment, false) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &expectedInteger) const override;
        };

        //! Formatter when column contains an altitude
        class CAltitudeFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CAltitudeFormatter(bool flightlevel = false, int alignment = alignRightVCenter(), bool i18n = true) : CDefaultFormatter(alignment, i18n), m_flightLevel(flightlevel) {}

            //! Constructor
            CAltitudeFormatter(const BlackMisc::PhysicalQuantities::CLengthUnit &unit, bool flightlevel = false, int alignment = alignRightVCenter(), bool i18n = true) : CDefaultFormatter(alignment, i18n), m_unit(unit), m_flightLevel(flightlevel) {}

            //! Set the unit, normally ft/m
            void setUnit(const BlackMisc::PhysicalQuantities::CLengthUnit &unit);

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &altitude) const override;

        private:
            BlackMisc::PhysicalQuantities::CLengthUnit m_unit = BlackMisc::PhysicalQuantities::CLengthUnit::ft();
            const bool m_flightLevel = false;
        };

        //! Formatter when column contains a color
        class CColorFormatter : public CDefaultFormatter
        {
        public:
            //! Constructor
            CColorFormatter(int alignment = alignCentered(), bool i18n = true);

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! \copydoc CDefaultFormatter::tooltipRole
            virtual BlackMisc::CVariant tooltipRole(const BlackMisc::CVariant &dataCVariant) const override;

            //! Display the icon
            virtual BlackMisc::CVariant decorationRole(const BlackMisc::CVariant &dataCVariant) const override;
        };

        //! Formatter for physical quantities
        template<class MU, class PQ> class CPhysiqalQuantiyFormatter : public CValueObjectFormatter
        {
        public:
            //! Constructor
            CPhysiqalQuantiyFormatter(MU unit = MU::defaultUnit(), int digits = 2, int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true, const QList<int> &supportedRoles = roleDisplay()) : CValueObjectFormatter(alignment, i18n, supportedRoles), m_unit(unit), m_digits(digits), m_withUnit(withUnit) {}

            //! \copydoc BlackGui::Models::CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &physicalQuantity) const override
            {
                if (physicalQuantity.canConvert<PQ>())
                {
                    const PQ pq = physicalQuantity.value<PQ>();
                    return this->displayRole(pq);
                }
                else
                {
                    Q_ASSERT_X(false, "CPhysiqalQuantiyFormatter::displayRole", "No CPhysicalQuantity class");
                    return emptyStringVariant();
                }
            }

            //! Version if value is already available as PQ
            BlackMisc::CVariant displayRole(const PQ &pq) const
            {
                if (pq.isNull())
                {
                    static const BlackMisc::CVariant null("null");
                    return null;
                }
                return pq.valueRoundedWithUnit(m_unit, m_digits, m_useI18n);
            }

            //! Set unit
            virtual void setUnit(const MU &unit) { m_unit = unit; }

            //! Enable unit display
            virtual void enableUnit(bool enable) { m_withUnit = enable; }

            //! Digits
            virtual void setDigits(int digits) { m_digits = digits; }

        protected:
            MU   m_unit;            //!< unit
            int  m_digits = 2;      //!< digits
            bool m_withUnit = true; //!< format with unit?
        };

        //! COM frequencies
        class CComFrequencyFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CFrequencyUnit, BlackMisc::PhysicalQuantities::CFrequency>
        {
        public:
            //! Constructor
            CComFrequencyFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
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

        //! Airspace distance displayed in NM
        class CAirspaceDistanceFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CLengthUnit, BlackMisc::PhysicalQuantities::CLength>
        {
        public:
            //! Constructor
            CAirspaceDistanceFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CLengthUnit::NM(), 1, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
        };

        //! Speed displayed in kts
        class CSpeedKtsFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CSpeedUnit, BlackMisc::PhysicalQuantities::CSpeed>
        {
        public:
            //! Constructor
            CSpeedKtsFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CSpeedUnit::kts(), 0, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual BlackMisc::CVariant displayRole(const BlackMisc::CVariant &dataCVariant) const override;
        };
    } // namespace
} // namespace

#endif // guard
