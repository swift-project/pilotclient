// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PIXMAP_H
#define SWIFT_MISC_PIXMAP_H

#include "misc/swiftmiscexport.h"
#include "misc/metaclass.h"
#include "misc/valueobject.h"

#include <QByteArray>
#include <QMetaType>
#include <QPixmap>
#include <QReadWriteLock>
#include <QString>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CPixmap)

namespace swift::misc
{
    class CStatusMessage;

    //! Pixmap which can be transferred via DBus.
    //! \remark: Use, when a pixmap cannot be stored in the resources
    class SWIFT_MISC_EXPORT CPixmap : public CValueObject<CPixmap>
    {
    public:
        //! Default constructor.
        CPixmap() = default;

        //! Constructor.
        CPixmap(const QPixmap &pixmap);

        //! Copy constructor (because of mutex)
        CPixmap(const CPixmap &other);

        //! Copy assignment (because of mutex)
        CPixmap &operator=(const CPixmap &other);

        //! Destructor.
        ~CPixmap() = default;

        //! Corresponding pixmap
        const QPixmap &pixmap() const;

        //! Implicit conversion
        operator QPixmap() const;

        //! With Pixmap?
        bool isNull() const;

        //! \copydoc CIcon::toPixmap
        QPixmap toPixmap() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        //! Init the byte array with data
        void fillByteArray();

        QByteArray m_array; //!< data of pixmap
        mutable QPixmap m_pixmap; //!< cached pixmap, mutable because of lazy initialization
        mutable bool m_hasCachedPixmap = false; //!< pixmap? Mutable because of lazy initialization
        mutable QReadWriteLock m_lock; //!< lock (because of mutable members)

        SWIFT_METACLASS(
            CPixmap,
            SWIFT_METAMEMBER(array));
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::CPixmap)

#endif // guard
