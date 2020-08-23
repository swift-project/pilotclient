/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICON_H
#define BLACKMISC_ICON_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbus.h"
#include "blackmisc/datastream.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/icons.h"
#include "blackmisc/json.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QIcon>
#include <QMetaType>
#include <QPixmap>
#include <QString>
#include <tuple>

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
        public Mixin::HashByMetaClass<CIcon>,
        public Mixin::DBusByMetaClass<CIcon>,
        public Mixin::DataStreamByMetaClass<CIcon>,
        public Mixin::JsonByMetaClass<CIcon>,
        public Mixin::EqualsByMetaClass<CIcon>,
        public Mixin::LessThanByMetaClass<CIcon>,
        public Mixin::CompareByMetaClass<CIcon>,
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

        //! Constructor for file icons
        CIcon(const QString &resourceFilePath, const QString &descriptiveText);

        //! Get descriptive text
        const QString &getDescriptiveText() const { return m_descriptiveText; }

        //! Resource path if any
        const QString &getFileResourcePath() const { return m_fileResourcePath; }

        //! Resource path available?
        bool hasFileResourcePath() const { return !m_fileResourcePath.isEmpty(); }

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
        void setRotation(int degrees) { m_rotateDegrees = degrees; }

        //! Rotate by given degrees
        void setRotation(const BlackMisc::PhysicalQuantities::CAngle &rotate);

        //! Set descriptive text
        void setDescriptiveText(const QString &text) { m_descriptiveText = text; }

        //! Implicit conversion
        operator QPixmap() const { return this->toPixmap(); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Icon for given index
        static const CIcon &iconByIndex(CIcons::IconIndex index);

        //! Icon for given index
        static const CIcon &iconByIndex(int index);

    private:
        CIcons::IconIndex m_index = CIcons::NotSet;
        int m_rotateDegrees = 0;    //!< Rotation
        QString m_descriptiveText;  //!< what does it represent?
        QString m_fileResourcePath; //!< file resource path
        QPixmap m_pixmap;           //!< Used with generated pixmaps, when not used with index

        BLACK_METACLASS(
            CIcon,
            BLACK_METAMEMBER(index),
            BLACK_METAMEMBER(rotateDegrees, 0, DisabledForComparison | DisabledForHashing),
            BLACK_METAMEMBER(descriptiveText),
            BLACK_METAMEMBER(fileResourcePath)
        );
    };

    namespace Private
    {
        //! \private Needed so CValueObjectMetaInfoHelper can copy forward-declared CIcon.
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

Q_DECLARE_METATYPE(BlackMisc::CIcon)

#endif // guard
