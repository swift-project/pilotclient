/* Copyright (C) 2018
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIRECTORIES_H
#define BLACKMISC_DIRECTORIES_H

#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CDirectories)

namespace BlackMisc
{
    /*!
     * Directories (swift data directories)
     * \remark the main purpose of this class is to serve as setting's value object
     */
    class BLACKMISC_EXPORT CDirectories : public CValueObject<CDirectories>
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
        CDirectories() {}

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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Convert to directory
        static QString fileNameToDirectory(const QString &fileName);

    private:
        QString m_dirFlightPlan;         //!< directory for flight plans
        QString m_dirLastViewJson;       //!< last JSON save directory
        QString m_dirLastModelJson;      //!< last JSON model directory
        QString m_dirLastModelStashJson; //!< last JSON model stash save directory
        QString m_dirMatchingScript;     //!< matching script dir

        //! Return checkDir if existing, defaultDir otherwise
        QString existingOrDefaultDir(const QString &checkDir, const QString &defaultDir) const;

        BLACK_METACLASS(
            CDirectories,
            BLACK_METAMEMBER(dirFlightPlan),
            BLACK_METAMEMBER(dirLastViewJson),
            BLACK_METAMEMBER(dirLastModelJson),
            BLACK_METAMEMBER(dirLastModelStashJson),
            BLACK_METAMEMBER(dirMatchingScript)
        );
    };

    namespace Settings
    {
        //! ATC stations settings
        struct TDirectorySettings : public TSettingTrait<CDirectories>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "swiftdirectories"; }

            //! \copydoc BlackMisc::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("swift directories"); return name; }
        };
    }
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CDirectories)

#endif // guard
