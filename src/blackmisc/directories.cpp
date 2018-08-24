/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/directories.h"
#include "directoryutils.h"
#include <QFileInfo>
#include <QDir>

namespace BlackMisc
{
    QString CDirectories::getFlightPlanDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getFlightPlanDirectory(), CDirectoryUtils::documentationDirectory());
    }

    QString CDirectories::getLastViewJsonDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getLastViewJsonDirectory(), CDirectoryUtils::documentationDirectory());
    }

    QString CDirectories::getLastModelDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getLastModelDirectory(), this->getLastViewJsonDirectoryOrDefault());
    }

    void CDirectories::setLastModelDirectory(const QString &dir)
    {
        m_dirLastModelJson = dir;
        if (this->hasLastViewJsonDirectory()) { return; }
        this->setLastViewJsonDirectory(dir);
    }

    QString CDirectories::getLastModelStashDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getLastModelStashDirectory(), this->getLastModelDirectoryOrDefault());
    }

    void CDirectories::setLastModelStashDirectory(const QString &dir)
    {
        m_dirLastModelStashJson = dir;
        if (this->hasLastModelDirectory()) { return; }
        this->setLastModelDirectory(dir);
    }

    QString CDirectories::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_dirFlightPlan;
    }

    CVariant CDirectories::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDirFlightPlan: return CVariant::fromValue(m_dirFlightPlan);
        case IndexDirFlightPlanOrDefault: return CVariant::fromValue(this->getFlightPlanDirectoryOrDefault());
        case IndexDirLastViewJson: return CVariant::fromValue(m_dirLastViewJson);
        case IndexDirLastViewJsonOrDefault: return CVariant::fromValue(this->getLastViewJsonDirectoryOrDefault());
        case IndexDirLastModelJson: return CVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirLastModelJsonOrDefault: return CVariant::fromValue(this->getLastModelDirectoryOrDefault());
        case IndexDirLastModelStashJson: return CVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirLastModelStashJsonOrDefault: return CVariant::fromValue(this->getLastModelStashDirectoryOrDefault());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CDirectories::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CDirectories>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDirFlightPlan: this->setFlightPlanDirectory(variant.toQString()); break;
        case IndexDirLastViewJsonOrDefault:
        case IndexDirLastViewJson: this->setLastViewJsonDirectory(variant.toQString()); break;
        case IndexDirLastModelJsonOrDefault:
        case IndexDirLastModelJson: this->setLastModelDirectory(variant.toQString()); break;
        case IndexDirLastModelStashJsonOrDefault:
        case IndexDirLastModelStashJson: this->setLastModelStashDirectory(variant.toQString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CDirectories::fileNameToDirectory(const QString &fileName)
    {
        const QFileInfo fi(fileName);
        return fi.dir().absolutePath();
    }

    QString CDirectories::existingOrDefaultDir(const QString &checkDir, const QString &defaultDir) const
    {
        const QDir d(checkDir);
        if (d.exists()) { return checkDir; }
        return defaultDir;
    }
} // namespace
