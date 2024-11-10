// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/pixmap.h"

#include <QBuffer>
#include <QIODevice>
#include <QReadLocker>
#include <QWriteLocker>
#include <QtGlobal>
#include <tuple>

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CPixmap)

namespace swift::misc
{
    CPixmap::CPixmap(const QPixmap &pixmap) : m_pixmap(pixmap), m_hasCachedPixmap(true)
    {
        this->fillByteArray();
    }

    CPixmap::CPixmap(const CPixmap &other) : CValueObject(other)
    {
        *this = other;
    }

    CPixmap &CPixmap::operator=(const CPixmap &other)
    {
        if (this == &other) { return *this; }

        QReadLocker readLock(&other.m_lock);
        auto tuple = std::make_tuple(other.m_pixmap, other.m_hasCachedPixmap, other.m_array);
        readLock.unlock(); // avoid deadlock

        QWriteLocker writeLock(&this->m_lock);
        std::tie(m_pixmap, m_hasCachedPixmap, m_array) = tuple;
        return *this;
    }

    const QPixmap &CPixmap::pixmap() const
    {
        QWriteLocker lock(&this->m_lock);
        if (this->m_hasCachedPixmap) { return this->m_pixmap; }

        // this part here becomes relevant when marshalling via DBus is used
        // in this case only the array is transferred
        this->m_hasCachedPixmap = true;
        if (this->m_array.isEmpty()) { return this->m_pixmap; }
        bool s = this->m_pixmap.loadFromData(this->m_array, "PNG");
        Q_ASSERT(s);
        Q_UNUSED(s);
        return this->m_pixmap;
    }

    bool CPixmap::isNull() const
    {
        QReadLocker lock(&this->m_lock);
        if (this->m_hasCachedPixmap) { return false; }
        return (this->m_array.isEmpty() || this->m_array.isNull());
    }

    CPixmap::operator QPixmap() const
    {
        return pixmap();
    }

    QString CPixmap::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return "Pixmap";
    }

    QPixmap CPixmap::toPixmap() const
    {
        return this->pixmap();
    }

    void CPixmap::fillByteArray()
    {
        // no lock needed because this is a private method only called from a constructor
        QBuffer buffer(&this->m_array);
        buffer.open(QIODevice::WriteOnly);
        this->m_pixmap.save(&buffer, "PNG");
        buffer.close();
    }

} // namespace
