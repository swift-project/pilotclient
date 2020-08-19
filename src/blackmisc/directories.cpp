/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/directories.h"
#include "blackmisc/directoryutils.h"
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

    QString CDirectories::getMatchingScriptDirectoryOrDefault() const
    {
        if (m_dirMatchingScript.isEmpty())
        {
            return CDirectoryUtils::shareMatchingScriptDirectory();
        }
        return m_dirMatchingScript;
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
        case IndexDirFlightPlan:             return CVariant::fromValue(m_dirFlightPlan);
        case IndexDirFlightPlanOrDefault:    return CVariant::fromValue(this->getFlightPlanDirectoryOrDefault());
        case IndexDirLastViewJson:           return CVariant::fromValue(m_dirLastViewJson);
        case IndexDirLastViewJsonOrDefault:  return CVariant::fromValue(this->getLastViewJsonDirectoryOrDefault());
        case IndexDirLastModelJson:          return CVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirLastModelJsonOrDefault: return CVariant::fromValue(this->getLastModelDirectoryOrDefault());
        case IndexDirLastModelStashJson:     return CVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirMatchingScript:         return CVariant::fromValue(this->getMatchingScriptDirectoryOrDefault());
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
        case IndexDirMatchingScript: this->setMatchingScriptDirectory(variant.toQString()); break;
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
        if (checkDir.isEmpty()) { return defaultDir; } // empty dir returns true ????
        const QDir d(checkDir);
        if (d.exists()) { return checkDir; }
        return defaultDir;
    }
} // namespace
