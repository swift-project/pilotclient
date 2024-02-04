// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
    CSetupReader::CSetupReader(QObject *parent) : QObject(parent)
    {}

    CStatusMessageList CSetupReader::loadSetup()
    {
        CStatusMessageList msgs;
        msgs = this->readLocalBootstrapFile();
        m_setupAvailable = msgs.isSuccess();
        return msgs;
    }

    CStatusMessageList CSetupReader::readLocalBootstrapFile()
    {
        const QString fileName = CSwiftDirectories::bootstrapResourceFilePath();
        // TODO Check if file exists or is broken

        if (fileName.isEmpty()) { return CStatusMessage(this).error(u"No file name for local bootstrap file"); }
        if (!sApp || sApp->isShuttingDown()) { return CStatusMessage(this).error(u"No sApp, shutting down?"); }
        const QFile file(fileName);
        if (!file.exists())
        {
            return CStatusMessage(this).error(u"File '%1' not existing") << fileName;
        }

        const QString content(CFileUtils::readFileToString(fileName));
        if (content.isEmpty()) { return CStatusMessage(this).error(u"File '%1' empty") << fileName; }

        try
        {
            m_setup.convertFromJson(content);
            return CStatusMessage(this).info(u"Setup loaded from local file '%1'") << fileName;
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, this, QStringLiteral("Parsing local setup file '%1'").arg(fileName));
        }
    }

    const QStringList &CSetupReader::getLogCategories()
    {
        static const QStringList cats({ "swift.setupreader", CLogCategories::webservice(), CLogCategories::startup() });
        return cats;
    }

    CGlobalSetup CSetupReader::getSetup() const
    {
        return m_setup;
    }
} // namespace
