// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PIXMAP_H
#define BLACKMISC_PIXMAP_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QByteArray>
#include <QMetaType>
#include <QPixmap>
#include <QReadWriteLock>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CPixmap)

namespace BlackMisc
{
    class CStatusMessage;

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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        //! Init the byte array with data
        void fillByteArray();

        QByteArray m_array; //!< data of pixmap
        mutable QPixmap m_pixmap; //!< cached pixmap, mutable because of lazy initialization
        mutable bool m_hasCachedPixmap = false; //!< pixmap? Mutable because of lazy initialization
        mutable QReadWriteLock m_lock; //!< lock (because of mutable members)

        BLACK_METACLASS(
            CPixmap,
            BLACK_METAMEMBER(array)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CPixmap)

#endif // guard
