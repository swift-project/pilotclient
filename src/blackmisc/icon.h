/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICON_H
#define BLACKMISC_ICON_H

#include "blackmiscexport.h"
#include "icons.h"
#include "variant.h"
#include "tuple.h"
#include "inheritancetraits.h"
#include "dictionary.h"
#include <QIcon>

namespace BlackMisc
{
    class CIcon;
    namespace PhysicalQuantities { class CAngle; }

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit icon-related functions.
         */
        template <class Derived, CIcons::IconIndex IconIndex = CIcons::StandardIconUnknown16>
        class Icon
        {
        public:
            //! As icon, not implemented by all classes
            CIcon toIcon() const;

            //! As pixmap, required for most GUI views
            QPixmap toPixmap() const;

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::Icon,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_ICON(DERIVED)      \
    using ::BlackMisc::Mixin::Icon<DERIVED>::toIcon;    \
    using ::BlackMisc::Mixin::Icon<DERIVED>::toPixmap;
    } // Mixin

    //! Value object for icons. An icon is stored in the global icon repository and
    //! identified by its index. It contains no(!) pyhsical data for the icon itself.
    class BLACKMISC_EXPORT CIcon :
        public Mixin::MetaType<CIcon>,
        public Mixin::HashByTuple<CIcon>,
        public Mixin::DBusByTuple<CIcon>,
        public Mixin::JsonByTuple<CIcon>,
        public Mixin::EqualsByTuple<CIcon>,
        public Mixin::LessThanByTuple<CIcon>,
        public Mixin::CompareByTuple<CIcon>,
        public Mixin::String<CIcon>,
        public Mixin::Icon<CIcon>
    {
    public:
        //! Default constructor.
        CIcon() {}

        //! Constructor.
        CIcon(CIcons::IconIndex index, const QString &descriptiveText);

        //! Constructor for generated icon
        CIcon(const QPixmap &pixmap, const QString &descriptiveText);

        //! Construcror for file icons
        CIcon(const QString &resourceFilePath, const QString &descriptiveText);

        //! Get descriptive text
        const QString &getDescriptiveText() const { return this->m_descriptiveText; }

        //! Index
        CIcons::IconIndex getIndex() const;

        //! Index based
        bool isIndexBased() const;

        //! Generated icon
        bool isGenerated() const;

        //! File based?
        bool isFileBased() const;

        //! Icon set?
        bool isSet() const;

        //! Corresponding pixmap
        QPixmap toPixmap() const;

        //! A QIcon
        QIcon toQIcon() const;

        //! Rotate by n degrees
        void setRotation(int degrees) { this->m_rotateDegrees = degrees; }

        //! Rotate by given degrees
        void setRotation(const BlackMisc::PhysicalQuantities::CAngle &rotate);

        //! Set descriptive text
        void setDescriptiveText(const QString &text) { this->m_descriptiveText = text; }

        //! Implicit conversion
        operator QPixmap() const { return this->toPixmap(); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Icon for given index
        static const CIcon &iconByIndex(CIcons::IconIndex index);

        //! Icon for given index
        static const CIcon &iconByIndex(int index);

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CIcon)
        CIcons::IconIndex m_index = CIcons::NotSet;
        int m_rotateDegrees = 0;    //!< Rotation
        QString m_descriptiveText;  //!< what does it represent?
        QPixmap m_pixmap;           //!< Used with generated pixmaps, when not used with index
    };

    namespace Private
    {
        //! \private Needed so we can copy forward-declared CIcon.
        inline void assign(CIcon &a, const CIcon &b) { a = b; }
    }

    namespace Mixin
    {
        template <class Derived, CIcons::IconIndex IconIndex>
        CIcon Icon<Derived, IconIndex>::toIcon() const
        {
            return CIcon::iconByIndex(IconIndex);
        }
        template <class Derived, CIcons::IconIndex IconIndex>
        QPixmap Icon<Derived, IconIndex>::toPixmap() const
        {
            return derived()->toIcon().toPixmap();
        }
    }
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CIcon, (
                                   attr(o.m_index),
                                   attr(o.m_rotateDegrees, flags < DisabledForComparison | DisabledForHashing > ()),
                                   attr(o.m_descriptiveText)
                               ))

Q_DECLARE_METATYPE(BlackMisc::CIcon)

#endif // guard
