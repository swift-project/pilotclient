/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PIXMAP_H
#define BLACKMISC_PIXMAP_H

#include "blackmiscexport.h"
#include "valueobject.h"
#include <QPixmap>
#include <QReadWriteLock>

namespace BlackMisc
{
    //! Pixmap which can be transferred via DBus.
    //! \remark: Use, when a pixmap cannot be stored in the resources
    class BLACKMISC_EXPORT CPixmap : public CValueObject<CPixmap>
    {
    public:
        //! Default constructor.
        CPixmap() = default;

        //! Constructor.
        CPixmap(const QPixmap &pixmap);

        //! Copy constructor (because of mutex)
        CPixmap(const CPixmap &other);

        //! Copy assignment (because of mutex)
        CPixmap &operator =(const CPixmap &other);

        //! Corresponding pixmap
        const QPixmap &pixmap() const;

        //! Implicit conversion
        operator QPixmap() const;

        //! With Pixmap?
        bool isNull() const;

        //! \copydoc CValueObject::toPixmap
        QPixmap toPixmap() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(BlackMisc::CPixmap)

        //! Init the byte array with data
        void fillByteArray();

        QByteArray m_array;                     //!< data of pixmap
        mutable QPixmap m_pixmap;               //!< cached pixmap, mutable because of lazy initialization
        mutable bool m_hasCachedPixmap = false; //!< pixmap? Mutable because of lazy initialization
        mutable QReadWriteLock m_lock;          //!< lock (because of mutable members)
    };
} // namespace

//! \todo remove flags when gcc 4.7 streaming issue is resolved
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CPixmap, (
                                   attr(o.m_array, flags<DisabledForJson>())
                               ))
Q_DECLARE_METATYPE(BlackMisc::CPixmap)

#endif // guard
