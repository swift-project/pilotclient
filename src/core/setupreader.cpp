// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/setupreader.h"

#include <QByteArray>
#include <QFile>
#include <QPointer>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QStringBuilder>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "core/application.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/networkutils.h"
#include "misc/network/url.h"
#include "misc/statusmessage.h"
#include "misc/swiftdirectories.h"
#include "misc/verify.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::data;

namespace swift::core
{
    CSetupReader::CSetupReader(QObject *parent) : QObject(parent) {}

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
        if (!file.exists()) { return CStatusMessage(this).error(u"File '%1' not existing") << fileName; }

        const QString content(CFileUtils::readFileToString(fileName));
        if (content.isEmpty()) { return CStatusMessage(this).error(u"File '%1' empty") << fileName; }

        try
        {
            m_setup.convertFromJson(content);
            return CStatusMessage(this).info(u"Setup loaded from local file '%1'") << fileName;
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, this,
                                                     QStringLiteral("Parsing local setup file '%1'").arg(fileName));
        }
    }

    const QStringList &CSetupReader::getLogCategories()
    {
        static const QStringList cats({ "swift.setupreader", CLogCategories::webservice(), CLogCategories::startup() });
        return cats;
    }

    CGlobalSetup CSetupReader::getSetup() const { return m_setup; }
} // namespace swift::core
