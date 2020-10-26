/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/icon.h"
#include "blackmisc/iconlist.h"

#include <QStringBuilder>
#include <QtGlobal>
#include <QDir>

namespace BlackMisc
{
    CIcon::CIcon(CIcons::IconIndex index) :
        CIcon(iconByIndex(index)) {}

    CIcon::CIcon(CIcons::IconIndex index, const QString &descriptiveText) :
        m_index(index), m_descriptiveText(descriptiveText) {}

    //CIcon::CIcon(const QPixmap &pixmap, const QString &descriptiveText) :
    //    m_index(CIcons::IconIsGenerated), m_descriptiveText(descriptiveText), m_pixmap(pixmap)
    //{ }

    //CIcon::CIcon(const QString &resourceFilePath, const QString &descriptiveText) :
    //    m_index(CIcons::IconIsFile), m_descriptiveText(descriptiveText)
    //{
    //    QString fullPath;
    //    m_pixmap = CIcons::pixmapByResourceFileName(QDir::cleanPath(resourceFilePath), fullPath);
    //    m_fileResourcePath = fullPath;
    //}

    CIcons::IconIndex CIcon::getIndex() const
    {
        return m_index;
    }

    bool CIcon::isIndexBased() const
    {
        return m_index < CIcons::IconIsGenerated;
    }

    bool CIcon::isGenerated() const
    {
        return this->getIndex() == CIcons::IconIsGenerated;
    }

    bool CIcon::isFileBased() const
    {
        return this->getIndex() == CIcons::IconIsFile;
    }

    bool CIcon::isSet() const
    {
        return (m_index != CIcons::NotSet);
    }

    QPixmap CIcon::toPixmap() const
    {
        if (this->isSet())
        {
            if (this->isGenerated()) { return m_pixmap; }
            if (this->isFileBased()) { return m_pixmap; }
            return CIcons::pixmapByIndex(this->getIndex(), m_rotateDegrees);
        }
        else
        {
            return CIcons::pixmapByIndex(this->getIndex());
        }
    }

    QIcon CIcon::toQIcon() const
    {
        return QIcon(toPixmap());
    }

    QString CIcon::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s = m_descriptiveText % u' ' % QString::number(m_index);
        return s;
    }

    const CIcon &CIcon::iconByIndex(CIcons::IconIndex index)
    {
        return iconByIndex(static_cast<int>(index));
    }

    const CIcon &CIcon::iconByIndex(int index)
    {
        // changed to index / at based approach during #322 (after Sleepy profiling)
        // this seems to be faster as the findBy approach previously used, but required synced indexes
        Q_ASSERT_X(index >= 0 && index < CIconList::allIcons().size(), "iconForIndex", "wrong index");
        return CIconList::allIcons()[index];
    }
} // namespace
