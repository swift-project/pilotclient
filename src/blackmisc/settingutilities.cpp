#include "settingutilities.h"

namespace BlackMisc
{
    namespace Settings
    {
        /*
         * Wrong path name message
         */
        BlackMisc::CStatusMessage CSettingUtilities::wrongPathMessage(const QString &path)
        {
            QString msg = "wrong path";
            if (!path.isEmpty())
            {
                msg.append(": ").append(path);
            }
            BlackMisc::CStatusMessage rp(BlackMisc::CStatusMessage::TypeValidation,
                                         BlackMisc::CStatusMessage::SeverityError, msg);
            return rp;
        }

        /*
         * Wrong path name messages
         */
        CStatusMessageList CSettingUtilities::wrongPathMessages(const QString &path)
        {
            BlackMisc::CStatusMessageList rps;
            rps.push_back(CSettingUtilities::wrongPathMessage(path));
            return rps;
        }

        /*
         * leading path
         */
        QString CSettingUtilities::removeLeadingPath(const QString &path)
        {
            int sl = path.indexOf('/');
            Q_ASSERT(sl >= 0);
            Q_ASSERT(path.length() > sl + 2);
            return path.mid(sl + 1);
        }

        /*
         * Append paths
         */
        QString CSettingUtilities::appendPaths(const QString &part1, const QString &part2, const QString &part3)
        {
            QString p(part1);
            if (part2.isEmpty()) return p;
            p.append('/').append(part2);
            if (part3.isEmpty()) return p;
            p.append('/').append(part3);
            return p;
        }
    }
}
