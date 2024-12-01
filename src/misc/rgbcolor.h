// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_RGBCOLOR_H
#define SWIFT_MISC_RGBCOLOR_H

#include <QColor>
#include <QMetaType>
#include <QPixmap>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CRgbColor)

namespace swift::misc
{
    /*!
     * Color
     */
    class SWIFT_MISC_EXPORT CRgbColor : public CValueObject<CRgbColor>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexRed = swift::misc::CPropertyIndexRef::GlobalIndexCRgbColor,
            IndexGreen,
            IndexBlue,
            IndexWebHex
        };

        //! Constructor
        CRgbColor() = default;

        //! Constructor (hex or color name)
        CRgbColor(const QString &color, bool isName = false);

        //! Constructor
        CRgbColor(int r, int g, int b);

        //! Constructor from QColor
        CRgbColor(const QColor &color);

        //! To QColor
        QColor toQColor() const;

        //! Set by QColor
        bool setQColor(const QColor &color);

        //! Red
        int red() const;

        //! Red 0..1
        double normalizedRed() const;

        //! Red as hex
        QString redHex(int digits = 2) const;

        //! Green
        int green() const;

        //! Green 0..1
        double normalizedGreen() const;

        //! Green as hex
        QString greenHex(int digits = 2) const;

        //! Blue
        int blue() const;

        //! Blue 0..1
        double normalizedBlue() const;

        //! Blue as hex
        QString blueHex(int digits = 2) const;

        //! Hex value
        QString hex(bool withHash = false) const;

        //! Value packed in 24 bit integer
        int packed() const;

        //! Construct from packed 24 bit integer value
        static CRgbColor fromPacked(int rgb);

        //! Hex or color name
        void setByString(const QString &color, bool isName = false);

        //! Valid?
        bool isValid() const;

        //! Color distance [0..1]
        //! http://stackoverflow.com/questions/4754506/color-similarity-distance-in-rgba-color-space/8796867#8796867
        double colorDistance(const CRgbColor &color) const;

        //! Mark as invalid
        void setInvalid();

        //! Representing icon
        CIcons::IconIndex toIcon() const;

        //! Icon as pixmap
        QPixmap toPixmap() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(CPropertyIndexRef index, const CRgbColor &compareValue) const;

        //! Compare with other color
        int compare(const CRgbColor &color) const;

    private:
        int m_r = -1;
        int m_g = -1;
        int m_b = -1;

        //! Color range 255/4095/65535
        double colorRange() const;

        static QString intToHex(int h, int digits = 2);

        SWIFT_METACLASS(
            CRgbColor,
            SWIFT_METAMEMBER(r),
            SWIFT_METAMEMBER(g),
            SWIFT_METAMEMBER(b));
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CRgbColor)

#endif // guard
