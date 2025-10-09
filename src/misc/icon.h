// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_ICON_H
#define SWIFT_MISC_ICON_H

#include <QIcon>
#include <QMetaType>
#include <QPixmap>
#include <QString>

#include "misc/icons.h"
#include "misc/metaclass.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Value object for icons. An icon is stored in the global icon repository and
    //! identified by its index. It contains no(!) pyhsical data for the icon itself.
    class SWIFT_MISC_EXPORT CIcon :
        public mixin::MetaType<CIcon>,
        public mixin::HashByMetaClass<CIcon>,
        public mixin::DBusByMetaClass<CIcon>,
        public mixin::DataStreamByMetaClass<CIcon>,
        public mixin::JsonByMetaClass<CIcon>,
        public mixin::EqualsByMetaClass<CIcon>,
        public mixin::LessThanByMetaClass<CIcon>,
        public mixin::CompareByMetaClass<CIcon>,
        public mixin::String<CIcon>
    {
    public:
        //! Default constructor.
        CIcon() = default;

        //! Constructor.
        CIcon(CIcons::IconIndex index);

        //! Constructor for generated icon
        // CIcon(const QPixmap &pixmap, const QString &descriptiveText);

        //! Constructor for file icons
        // CIcon(const QString &resourceFilePath, const QString &descriptiveText);

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

        //! Identity function
        CIcons::IconIndex toIcon() const { return m_index; }

        //! A QIcon
        QIcon toQIcon() const;

        //! Rotate by n degrees
        void setRotation(int degrees) { m_rotateDegrees = degrees; }

        //! Set descriptive text
        void setDescriptiveText(const QString &text) { m_descriptiveText = text; }

        //! Implicit conversion
        operator QPixmap() const { return this->toPixmap(); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Icon for given index
        static const CIcon &iconByIndex(CIcons::IconIndex index);

        //! Icon for given index
        static const CIcon &iconByIndex(int index);

        //! All icons
        static const CSequence<CIcon> &allIcons();

    private:
        //! Constructor.
        CIcon(CIcons::IconIndex index, const QString &descriptiveText);

        CIcons::IconIndex m_index = CIcons::NotSet;
        int m_rotateDegrees = 0; //!< Rotation
        QString m_descriptiveText; //!< what does it represent?
        QString m_fileResourcePath; //!< file resource path
        QPixmap m_pixmap; //!< Used with generated pixmaps, when not used with index

        SWIFT_METACLASS(
            CIcon,
            SWIFT_METAMEMBER(index),
            SWIFT_METAMEMBER(rotateDegrees, 0, DisabledForComparison | DisabledForHashing),
            SWIFT_METAMEMBER(descriptiveText),
            SWIFT_METAMEMBER(fileResourcePath));
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CIcon)

#endif // SWIFT_MISC_ICON_H
