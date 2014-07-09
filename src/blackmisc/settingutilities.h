#ifndef BLACKMISC_SETTINGUTILITIES_H
#define BLACKMISC_SETTINGUTILITIES_H

#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Settings
    {
        //! Helper / utility methods for settings
        class CSettingUtilities
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

            //! Wrong path message
            static BlackMisc::CStatusMessage wrongPathMessage(const QString &path = "");

            //! Wrong path messages
            static BlackMisc::CStatusMessageList wrongPathMessages(const QString &path = "");

            //! Wrong command message
            static BlackMisc::CStatusMessage wrongCommandMessage(const QString &command);

            //! Wrong command messages
            static BlackMisc::CStatusMessageList wrongCommandMessages(const QString &command);

            //! Value not changed message
            static BlackMisc::CStatusMessage valueNotChangedMessage(const QString &valueName);

            //! Value changed message with flag
            static CStatusMessage valueChangedMessage(bool changed, const QString &valueName);

            //! Value changed message
            static BlackMisc::CStatusMessage valueChangedMessage(const QString &valueName);

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
