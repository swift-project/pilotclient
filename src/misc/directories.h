// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DIRECTORIES_H
#define SWIFT_MISC_DIRECTORIES_H

#include <QString>

#include "misc/propertyindexref.h"
#include "misc/settingscache.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CDirectories)

namespace swift::misc
{
    /*!
     * Directories (swift data directories)
     * \remark the main purpose of this class is to serve as setting's value object
     */
    class SWIFT_MISC_EXPORT CDirectories : public CValueObject<CDirectories>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDirFlightPlan = CPropertyIndexRef::GlobalIndexCDirectories,
            IndexDirFlightPlanOrDefault,
            IndexDirLastViewJson,
            IndexDirLastViewJsonOrDefault,
            IndexDirLastModelJson,
            IndexDirLastModelJsonOrDefault,
            IndexDirLastModelStashJson,
            IndexDirLastModelStashJsonOrDefault,
            IndexDirMatchingScript
        };

        //! Constructor
        CDirectories() = default;

        //! Flight plan directory
        const QString &getFlightPlanDirectory() const { return m_dirFlightPlan; }

        //! Flight plan directory or default
        QString getFlightPlanDirectoryOrDefault() const;

        //! Flight plan directory
        void setFlightPlanDirectory(const QString &dir) { m_dirFlightPlan = dir; }

        //! Last view JSON directory
        const QString &getLastViewJsonDirectory() const { return m_dirLastViewJson; }

        //! Has a view JSON directory?
        bool hasLastViewJsonDirectory() const { return !m_dirLastViewJson.isEmpty(); }

        //! Last view JSON directory or default
        QString getLastViewJsonDirectoryOrDefault() const;

        //! Last view JSON directory (default if not more specific)
        void setLastViewJsonDirectory(const QString &dir) { m_dirLastViewJson = dir; }

        //! Last view JSON model stash directory
        const QString &getLastModelDirectory() const { return m_dirLastModelJson; }

        //! Last view JSON model directory or default
        QString getLastModelDirectoryOrDefault() const;

        //! Last view JSON model directory
        void setLastModelDirectory(const QString &dir);

        //! Has a model JSON directory?
        bool hasLastModelDirectory() const { return !m_dirLastModelJson.isEmpty(); }

        //! Last view JSON model directory
        const QString &getLastModelStashDirectory() const { return m_dirLastModelStashJson; }

        //! Last view JSON model stash directory or default
        QString getLastModelStashDirectoryOrDefault() const;

        //! Last view JSON model stash directory
        void setLastModelStashDirectory(const QString &dir);

        //! Matching script directory
        QString getMatchingScriptDirectoryOrDefault() const;

        //! Matching script directory
        void setMatchingScriptDirectory(const QString &dir) { m_dirMatchingScript = dir; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Convert to directory
        static QString fileNameToDirectory(const QString &fileName);

    private:
        QString m_dirFlightPlan; //!< directory for flight plans
        QString m_dirLastViewJson; //!< last JSON save directory
        QString m_dirLastModelJson; //!< last JSON model directory
        QString m_dirLastModelStashJson; //!< last JSON model stash save directory
        QString m_dirMatchingScript; //!< matching script dir

        //! Return checkDir if existing, defaultDir otherwise
        QString existingOrDefaultDir(const QString &checkDir, const QString &defaultDir) const;

        SWIFT_METACLASS(
            CDirectories,
            SWIFT_METAMEMBER(dirFlightPlan),
            SWIFT_METAMEMBER(dirLastViewJson),
            SWIFT_METAMEMBER(dirLastModelJson),
            SWIFT_METAMEMBER(dirLastModelStashJson),
            SWIFT_METAMEMBER(dirMatchingScript));
    };

    namespace settings
    {
        //! ATC stations settings
        struct TDirectorySettings : public TSettingTrait<CDirectories>
        {
            //! \copydoc swift::misc::TSettingTrait::key
            static const char *key() { return "swiftdirectories"; }

            //! \copydoc swift::misc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("swift directories");
                return name;
            }
        };
    } // namespace settings
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CDirectories)

#endif // SWIFT_MISC_DIRECTORIES_H
