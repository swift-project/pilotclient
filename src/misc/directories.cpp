// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/directories.h"
#include "misc/swiftdirectories.h"
#include <QFileInfo>
#include <QDir>

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CDirectories)

namespace swift::misc
{
    QString CDirectories::getFlightPlanDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getFlightPlanDirectory(), CSwiftDirectories::documentationDirectory());
    }

    QString CDirectories::getLastViewJsonDirectoryOrDefault() const
    {
        return this->existingOrDefaultDir(this->getLastViewJsonDirectory(), CSwiftDirectories::documentationDirectory());
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
            return CSwiftDirectories::shareMatchingScriptDirectory();
        }
        return m_dirMatchingScript;
    }

    QString CDirectories::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_dirFlightPlan;
    }

    QVariant CDirectories::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDirFlightPlan: return QVariant::fromValue(m_dirFlightPlan);
        case IndexDirFlightPlanOrDefault: return QVariant::fromValue(this->getFlightPlanDirectoryOrDefault());
        case IndexDirLastViewJson: return QVariant::fromValue(m_dirLastViewJson);
        case IndexDirLastViewJsonOrDefault: return QVariant::fromValue(this->getLastViewJsonDirectoryOrDefault());
        case IndexDirLastModelJson: return QVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirLastModelJsonOrDefault: return QVariant::fromValue(this->getLastModelDirectoryOrDefault());
        case IndexDirLastModelStashJson: return QVariant::fromValue(m_dirLastModelStashJson);
        case IndexDirMatchingScript: return QVariant::fromValue(this->getMatchingScriptDirectoryOrDefault());
        case IndexDirLastModelStashJsonOrDefault: return QVariant::fromValue(this->getLastModelStashDirectoryOrDefault());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CDirectories::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CDirectories>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDirFlightPlan: this->setFlightPlanDirectory(variant.toString()); break;
        case IndexDirLastViewJsonOrDefault:
        case IndexDirLastViewJson: this->setLastViewJsonDirectory(variant.toString()); break;
        case IndexDirLastModelJsonOrDefault:
        case IndexDirLastModelJson: this->setLastModelDirectory(variant.toString()); break;
        case IndexDirLastModelStashJsonOrDefault:
        case IndexDirLastModelStashJson: this->setLastModelStashDirectory(variant.toString()); break;
        case IndexDirMatchingScript: this->setMatchingScriptDirectory(variant.toString()); break;
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
