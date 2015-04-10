/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGUTILITIES_H
#define BLACKMISC_SETTINGUTILITIES_H

#include "blackmiscexport.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Settings
    {
        //! Helper / utility methods for settings
        class BLACKMISC_EXPORT CSettingUtilities
        {
        private:
            CSettingUtilities() {}

        public:
            //! Command validate
            static const QString &CmdValidate()
            {
                static const QString cmd("validate");
                return cmd;
            }

            //! Command Update
            static const QString &CmdUpdate()
            {
                static const QString cmd("update");
                return cmd;
            }

            //! Command Remove
            static const QString &CmdRemove()
            {
                static const QString cmd("remove");
                return cmd;
            }

            //! Command Add
            static const QString &CmdAdd()
            {
                static const QString cmd("add");
                return cmd;
            }

            //! Command Set boolean value true
            static const QString &CmdSetTrue()
            {
                static const QString cmd("set:true");
                return cmd;
            }

            //! Command Set boolean value false
            static const QString &CmdSetFalse()
            {
                static const QString cmd("set:false");
                return cmd;
            }

            //! Remove leading path
            static QString removeLeadingPath(const QString &path);

            //! Append setting paths
            static QString appendPaths(const QString &part1, const QString &part2, const QString &part3 = "");

            //! prepare the settings directory, if required create it
            static bool initSettingsDirectory();

            //! get the settings directory
            static const QString &getSettingsDirectory();

            //! get the settings directory
            static const QString &getSettingsFile();

        };
    } // namespace
} // namespace
#endif // guard
