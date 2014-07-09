#include "settingutilities.h"
#include <QStandardPaths>
#include <QDir>

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
         * Wrong cmd message
         */
        BlackMisc::CStatusMessage CSettingUtilities::wrongCommandMessage(const QString &command)
        {
            QString msg = "wrong command";
            if (!command.isEmpty())
            {
                msg.append(": ").append(command);
            }
            BlackMisc::CStatusMessage rc(BlackMisc::CStatusMessage::TypeValidation,
                                         BlackMisc::CStatusMessage::SeverityError, msg);
            return rc;
        }

        /*
         * Wrong path name messages
         */
        CStatusMessageList CSettingUtilities::wrongPathMessages(const QString &path)
        {
            BlackMisc::CStatusMessageList wrongPath;
            wrongPath.push_back(CSettingUtilities::wrongPathMessage(path));
            return wrongPath;
        }

        /*
         * Wrong command message
         */
        CStatusMessageList CSettingUtilities::wrongCommandMessages(const QString &command)
        {
            BlackMisc::CStatusMessageList wrongCmds;
            wrongCmds.push_back(CSettingUtilities::wrongCommandMessage(command));
            return wrongCmds;
        }

        /*
         * Value not changed
         */
        CStatusMessage CSettingUtilities::valueNotChangedMessage(const QString &valueName)
        {
            return CStatusMessage(CStatusMessage::TypeSettings, CStatusMessage::SeverityWarning,
                                  QString("Value '%1' not changed").arg(valueName));
        }

        /*
         * Value changed
         */
        CStatusMessage CSettingUtilities::valueChangedMessage(const QString &valueName)
        {
            return CStatusMessage(CStatusMessage::TypeSettings, CStatusMessage::SeverityInfo,
                                  QString("Value '%1' changed").arg(valueName));
        }

        /*
         * Value changed
         */
        CStatusMessage CSettingUtilities::valueChangedMessage(bool changed, const QString &valueName)
        {
            return changed ?
                   valueChangedMessage(valueName) :
                   valueNotChangedMessage(valueName);
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

        /*
         * Init settings dir, if required create it
         */
        bool CSettingUtilities::initSettingsDirectory()
        {
            QDir dir(CSettingUtilities::getSettingsDirectory());
            if (dir.exists()) return true;
            return dir.mkpath(".");
        }

        /*
         * Directory
         */
        const QString &CSettingUtilities::getSettingsDirectory()
        {
            static QString dir = QStandardPaths::writableLocation(QStandardPaths::DataLocation).append("/BlackBox");
            return dir;
        }

        /*
         * File
         */
        const QString &CSettingUtilities::getSettingsFile()
        {
            static QString file(QString(CSettingUtilities::getSettingsDirectory()).append("/settings.json"));
            return file;
        }
    }
}
