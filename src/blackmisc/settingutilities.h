#ifndef BLACKMISC_SETTINGUTILITIES_H
#define BLACKMISC_SETTINGUTILITIES_H

#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Settings
    {
        class CSettingUtilities
        {
        private:
            CSettingUtilities() {}

        public:

            /*!
             * \brief Validate
             * \return
             */
            static const QString &CmdValidate()
            {
                static const QString cmd("validate");
                return cmd;
            }

            /*!
             * \brief Update
             * \return
             */
            static const QString &CmdUpdate()
            {
                static const QString cmd("update");
                return cmd;
            }

            /*!
             * \brief Remove
             * \return
             */
            static const QString &CmdRemove()
            {
                static const QString cmd("remove");
                return cmd;
            }

            /*!
             * \brief Add
             * \return
             */
            static const QString &CmdAdd()
            {
                static const QString cmd("add");
                return cmd;
            }

            /*!
             * \brief Wrong path message
             * \param path
             * \return
             */
            static BlackMisc::CStatusMessage wrongPathMessage(const QString &path = "");

            /*!
             * \brief Wrong path messages
             * \param path
             * \return
             */
            static BlackMisc::CStatusMessageList wrongPathMessages(const QString &path = "");

            /*!
             * \brief Remove leading path
             * \param path
             * \return
             */
            static QString removeLeadingPath(const QString &path);

            /*!
             * \brief Append paths
             * \param part1
             * \param part2
             * \param part3
             * \return
             */
            static QString appendPaths(const QString &part1, const QString &part2, const QString &part3 = "");

        };
    } // namespace
} // namespace
#endif // guard
