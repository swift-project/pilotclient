/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/setupreader.h"
#include "blackmisc/verify.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/url.h"
#include "blackmisc/statusmessage.h"
#include "blackconfig/buildconfig.h"

#include <QStringBuilder>
#include <QByteArray>
#include <QFile>
#include <QPointer>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackCore
{
    CSetupReader::CSetupReader(QObject *parent) : QObject(parent),
                                                  m_cmdBootstrapUrl {
                                                      { "url", "bootstrapurl" },
                                                      QCoreApplication::translate("application", "Bootstrap URL, e.g. https://datastore.swift-project.org/shared"),
                                                      "bootstrapurl",
                                                      (sApp->getApplicationInfo().isUnitTest()) ? unitTestBootstrapUrl() : ""
                                                  },
                                                  m_cmdBootstrapMode {
                                                      { "bmode", "bootstrapmode" },
                                                      QCoreApplication::translate("application", "Bootstrap mode: explicit, implicit, cache(-only)"),
                                                      "bootstrapmode",
                                                      "explicit"
                                                  }
    {}

    QList<QCommandLineOption> CSetupReader::getCmdLineOptions() const
    {
        return QList<QCommandLineOption> { { m_cmdBootstrapUrl, m_cmdBootstrapMode } };
    }

    CStatusMessageList CSetupReader::asyncLoad()
    {
        CStatusMessageList msgs;
        if (m_localSetupFileValue.isEmpty())
        {
            // TODO Check if file exists or is broken
            msgs = this->readLocalBootstrapFile(CSwiftDirectories::shareDirectory() + "/shared/bootstrap/bootstrap.json");
        }
        else
        {
            msgs = this->readLocalBootstrapFile(m_localSetupFileValue);
        }
        msgs.push_back(this->manageSetupAvailability(false, msgs.isSuccess()));
        return msgs;
    }

    bool CSetupReader::parseCmdLineArguments()
    {
        // copy vars at beginning to simplify a threadsafe version in the future
        const QString cmdLineBootstrapUrl = this->getCmdLineBootstrapUrl();
        BootstrapMode bootstrapMode = stringToEnum(sApp->getParserValue(m_cmdBootstrapMode));
        const bool ignoreCmdBootstrapUrl = m_ignoreCmdBootstrapUrl;
        const bool checkCmdBootstrapUrl = m_checkCmdBootstrapUrl;
        const QString bootstrapUrlFileValue = CGlobalSetup::buildBootstrapFileUrl(cmdLineBootstrapUrl);
        QString localSetupFileValue;
        const QUrl url(bootstrapUrlFileValue);
        const QString urlString(url.toString());
        bool ok = false;

        if (urlString.isEmpty() && bootstrapMode == Explicit)
        {
            bootstrapMode = Implicit; // no URL, we use implicit mode
        }

        do
        {
            // check on local file
            if (url.isLocalFile())
            {
                localSetupFileValue = url.toLocalFile();
                const QFile f(localSetupFileValue);
                if (!f.exists())
                {
                    sApp->cmdLineErrorMessage(QStringLiteral("File '%1' does not exist)").arg(localSetupFileValue));
                    break;
                }
            }

            // check on explicit URL
            if (bootstrapMode == Explicit)
            {
                if (!url.isLocalFile())
                {
                    bool retry = false;

                    // "retry" possible in some cases
                    do
                    {
                        if (ignoreCmdBootstrapUrl || !checkCmdBootstrapUrl || CNetworkUtils::canConnect(url, CNetworkUtils::getLongTimeoutMs()))
                        {
                            ok = true;
                            break;
                        }
                        retry = sApp->cmdLineErrorMessage(QStringLiteral("URL '%1' not reachable").arg(urlString), "", true);
                    }
                    while (retry);
                }
            }
        }
        while (false);

        m_localSetupFileValue = localSetupFileValue;
        m_bootstrapUrlFileValue = bootstrapUrlFileValue;
        m_bootstrapMode = bootstrapMode;
        return ok;
    }

    void CSetupReader::gracefulShutdown()
    {
        m_shutdown = true;
    }

    void CSetupReader::forceAvailabilityUpdate()
    {
        this->manageSetupAvailability(false, false); // fake a failed web read
    }

    CSetupReader::BootstrapMode CSetupReader::stringToEnum(const QString &s)
    {
        const QString bsm(s.toLower().trimmed());
        if (bsm.startsWith("expl")) { return Explicit; }
        if (bsm.startsWith("cache")) { return CacheOnly; }
        return Implicit;
    }

    const QString &CSetupReader::unitTestBootstrapUrl()
    {
        static const QString url("https://datastore.swift-project.org/shared");
        return url;
    }

    CStatusMessageList CSetupReader::readLocalBootstrapFile(const QString &fileName)
    {
        if (fileName.isEmpty()) { return CStatusMessage(this).error(u"No file name for local bootstrap file"); }
        if (!sApp || sApp->isShuttingDown()) { return CStatusMessage(this).error(u"No sApp, shutting down?"); }
        QString fn;
        const QFile file(fileName);
        if (!file.exists())
        {
            // relative name?
            const QString dir(sApp->getCmdSwiftPrivateSharedDir());
            if (dir.isEmpty()) { return CStatusMessage(this).error(u"Empty shared directory '%1' for bootstrap file") << dir; }

            // no version for local files, as those come with the current code
            fn = CFileUtils::appendFilePaths(dir, "bootstrap/" + CSwiftDirectories::bootstrapFileName());
        }
        else
        {
            fn = fileName;
        }

        const QString content(CFileUtils::readFileToString(fn));
        if (content.isEmpty()) { return CStatusMessage(this).error(u"File '%1' not existing or empty") << fn; }

        try
        {
            CGlobalSetup s;
            s.convertFromJson(content);
            s.markAsLoadedFromFile(true);
            const CStatusMessage setMsg = m_setup.set(s);
            const CStatusMessage setInfo = CStatusMessage(this).info(u"Setup cache updated from local file '%1'") << fn;
            return setMsg.isSuccess() ? setInfo : setMsg;
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, this, QStringLiteral("Parsing local setup file '%1'").arg(fn));
        }
    }

    const QStringList &CSetupReader::getLogCategories()
    {
        static const QStringList cats({ "swift.setupreader", CLogCategories::webservice(), CLogCategories::startup() });
        return cats;
    }

    bool CSetupReader::hasCmdLineBootstrapUrl() const
    {
        return !this->getCmdLineBootstrapUrl().isEmpty();
    }

    QString CSetupReader::getCmdLineBootstrapUrl() const
    {
        if (m_ignoreCmdBootstrapUrl) return {};
        return sApp->getParserValue(m_cmdBootstrapUrl);
    }

    void CSetupReader::setIgnoreCmdLineBootstrapUrl(bool ignore)
    {
        m_ignoreCmdBootstrapUrl = ignore;
        this->parseCmdLineArguments(); // T156 this part not threadsafe, currently not a real problem as setup reader runs in main thread
    }

    CGlobalSetup CSetupReader::getSetup() const
    {
        return m_setup.get();
    }

    bool CSetupReader::hasCachedSetup() const
    {
        const CGlobalSetup cachedSetup = m_setup.get();
        const bool cacheAvailable = cachedSetup.wasLoaded();
        return cacheAvailable;
    }

    QDateTime CSetupReader::getSetupCacheTimestamp() const
    {
        return m_setup.getTimestamp();
    }

    bool CSetupReader::prefillCacheWithLocalResourceBootstrapFile()
    {
        if (m_shutdown) { return false; }
        m_setup.synchronize(); // make sure it is loaded
        const CGlobalSetup cachedSetup = m_setup.get();
        const bool cacheAvailable = cachedSetup.wasLoaded();
        if (cacheAvailable)
        {
            CLogMessage(this).info(u"Setup cache prefill (bootstrap already cached, no prefill needed");
            return false;
        }
        const QString fn = CSwiftDirectories::bootstrapResourceFilePath();
        const CStatusMessageList msgs = this->readLocalBootstrapFile(fn);
        CLogMessage::preformatted(msgs);
        return true;
    }

    QString CSetupReader::getLastSuccessfulSetupUrl() const
    {
        QReadLocker l(&m_lockSetup);
        return m_lastSuccessfulSetupUrl;
    }

    void CSetupReader::synchronize()
    {
        m_setup.synchronize();
    }

    CStatusMessageList CSetupReader::getLastSetupReadErrorMessages() const
    {
        QReadLocker l(&m_lockSetup);
        return m_setupReadErrorMsgs;
    }

    const QString &CSetupReader::getBootstrapUrlFile() const
    {
        if (!m_localSetupFileValue.isEmpty()) { return m_localSetupFileValue; }
        return m_bootstrapUrlFileValue;
    }

    QString CSetupReader::getBootstrapModeAsString() const
    {
        switch (m_bootstrapMode)
        {
        case CacheOnly: return QStringLiteral("cache only");
        case Explicit: return QStringLiteral("explicit");
        case Implicit: return QStringLiteral("implicit");
        default: break;
        }
        return {};
    }

    void CSetupReader::setLastSetupReadErrorMessages(const CStatusMessageList &messages)
    {
        QWriteLocker l(&m_lockSetup);
        m_setupReadErrorMsgs = messages.getErrorMessages();
    }

    void CSetupReader::networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url)
    {
        Q_UNUSED(url)
        Q_UNUSED(logId)
        Q_UNUSED(current)
        Q_UNUSED(max)
    }

    CStatusMessageList CSetupReader::manageSetupAvailability(bool webRead, bool localRead)
    {
        Q_ASSERT_X(!(webRead && localRead), Q_FUNC_INFO, "Local and web read together seems to be wrong");
        CStatusMessageList msgs;
        QPointer<CSetupReader> myself(this);

        bool available = false;
        if (webRead || localRead)
        {
            available = true;
        }
        else
        {
            const bool cacheAvailable = m_setup.get().wasLoaded(); // loaded from web or file
            available = cacheAvailable && m_bootstrapMode != Explicit;
        }

        if (available && !webRead && !localRead)
        {
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, u"Setup available, but not updated this time"));
        }
        else if (!available)
        {
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, u"Setup not available"));
            if (m_bootstrapMode == Explicit)
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, u"Mode is 'explicit', likely URL '" % m_bootstrapUrlFileValue % u"' is not reachable"));
            }
        }
        m_setupAvailable = available;
        emit this->setupHandlingCompleted(available);
        return msgs;
    }
} // namespace
