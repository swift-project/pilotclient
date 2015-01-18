/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "pixmap.h"
#include <QBuffer>

namespace BlackMisc
{

    CPixmap::CPixmap(const QPixmap &pixmap) : m_pixmap(pixmap), m_hasCachedPixmap(true)
    {
        this->fillByteArray();
    }

    const QPixmap &CPixmap::pixmap() const
    {
        if (this->m_hasCachedPixmap) { return this->m_pixmap; }
        this->m_hasCachedPixmap = true;
        if (this->m_array.isEmpty()) { return this->m_pixmap; }
        bool s = this->m_pixmap.loadFromData(this->m_array, "PNG");
        Q_ASSERT(s);
        Q_UNUSED(s);
        return this->m_pixmap;
    }

    bool CPixmap::isNull() const
    {
        if (this->m_hasCachedPixmap) { return false; }
        return (this->m_array.isEmpty() || this->m_array.isNull());
    }

    CPixmap::operator QPixmap() const
    {
        return QPixmap(pixmap());
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
        QBuffer buffer(&this->m_array);
        buffer.open(QIODevice::WriteOnly);
        this->m_pixmap.save(&buffer, "PNG");
        buffer.close();
    }

} // namespace
