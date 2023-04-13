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
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/dbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/icons.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QIcon>
#include <QMetaType>
#include <QPixmap>
#include <QString>
#include <tuple>

namespace BlackMisc
{
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
        public Mixin::String<CIcon>
    {
    public:
        //! Default constructor.
        CIcon() {}

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

        //! \copydoc BlackMisc::Mixin::String::toQString
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

        BLACK_METACLASS(
            CIcon,
            BLACK_METAMEMBER(index),
            BLACK_METAMEMBER(rotateDegrees, 0, DisabledForComparison | DisabledForHashing),
            BLACK_METAMEMBER(descriptiveText),
            BLACK_METAMEMBER(fileResourcePath)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CIcon)

#endif // guard
