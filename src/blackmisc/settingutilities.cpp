#include "settingutilities.h"
#include <QStandardPaths>
#include <QDir>

namespace BlackMisc
{
    namespace Settings
    {
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
