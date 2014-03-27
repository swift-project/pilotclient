#ifndef BLACKMISC_SETTINGUTILITIES_H
#define BLACKMISC_SETTINGUTILITIES_H

#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Settings
    {
        //! \brief Helper / utility methods for settings
        class CSettingUtilities
        {
        private:
            CSettingUtilities() {}

        public:

            //! \brief Command validate
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

            //! \brief Wrong path message
            static BlackMisc::CStatusMessage wrongPathMessage(const QString &path = "");

            //! \brief Wrong path messages
            static BlackMisc::CStatusMessageList wrongPathMessages(const QString &path = "");

            //! \brief Remove leading path
            static QString removeLeadingPath(const QString &path);

            //! \brief Append setting paths
            static QString appendPaths(const QString &part1, const QString &part2, const QString &part3 = "");

        };
    } // namespace
} // namespace
#endif // guard
