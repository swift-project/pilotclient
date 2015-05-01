/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "valueobject.h" // outside include guard due to cyclic dependency hack (MS)

#ifndef BLACKMISC_ICON_H
#define BLACKMISC_ICON_H

#include "blackmiscexport.h"
#include "icons.h"
#include <QIcon>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        class CAngle;
    }

    //! Value object for icons. An icon is stored in the global icon repository and
    //! identified by its index. It contains no(!) pyhsical data for the icon itself.
    class BLACKMISC_EXPORT CIcon : public CValueObject<CIcon>
    {
    public:
        //! Default constructor.
        CIcon() = default;

        //! Constructor.
        CIcon(CIcons::IconIndex index, const QString &descriptiveText) :
            m_index(static_cast<int>(index)), m_descriptiveText(descriptiveText) {}

        //! Get descriptive text
        const QString &getDescriptiveText() const { return this->m_descriptiveText; }

        //! Index
        CIcons::IconIndex getIndex() const { return static_cast< CIcons::IconIndex>(this->m_index);}

        //! Corresponding pixmap
        QPixmap toPixmap() const;

        //! A QIcon
        QIcon toQIcon() const;

        //! Icon set?
        bool isSet() const { return (this->m_index != static_cast<int>(CIcons::NotSet));}

        //! Rotate by n degrees
        void setRotation(int degrees) { this->m_rotateDegrees = degrees; }

        //! Rotate by given degrees
        void setRotation(const BlackMisc::PhysicalQuantities::CAngle &rotate);

        //! Set descriptive text
        void setDescriptiveText(const QString &text) { this->m_descriptiveText = text; }

        //! Implicit conversion
        operator QPixmap() const { return this->toPixmap(); }

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! Icon for given index
        static const CIcon &iconByIndex(CIcons::IconIndex index);

        //! Icon for given index
        static const CIcon &iconByIndex(int index);

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CIcon)
        int m_index = static_cast<int>(CIcons::NotSet);
        int m_rotateDegrees = 0;
        QString m_descriptiveText;
    };
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CIcon, (o.m_index, o.m_descriptiveText))
Q_DECLARE_METATYPE(BlackMisc::CIcon)

#endif // guard
