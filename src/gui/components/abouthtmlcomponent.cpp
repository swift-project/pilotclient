// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "abouthtmlcomponent.h"

#include <QDesktopServices>
#include <QFile>
#include <QPointer>
#include <QTimer>

#include "ui_abouthtmlcomponent.h"

#include "gui/guiapplication.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::misc;

namespace swift::gui::components
{
    CAboutHtmlComponent::CAboutHtmlComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CAboutHtmlComponent)
    {
        ui->setupUi(this);
        const QPointer<CAboutHtmlComponent> myself(this);
        connect(ui->tbr_About, &QTextBrowser::anchorClicked, this, &CAboutHtmlComponent::onAnchorClicked,
                Qt::QueuedConnection);

        QTimer::singleShot(2500, this, [=] {
            if (!myself) { return; }
            myself->loadAbout();
        });
    }

    CAboutHtmlComponent::~CAboutHtmlComponent() = default;

    void CAboutHtmlComponent::loadAbout()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        // make links absolute
        static const QString htmlFixed = [=] {
            // workaround:
            // 1) Only reading as HTML gives proper formatting
            // 2) Reading the file resource fails (likely because of the style sheet)
            const QString html = CFileUtils::readFileToString(CSwiftDirectories::aboutFilePath());
            return html;

            // no longer replacing the URLs, doing this on anchor clicked
            // const QString legalDir = sGui->getGlobalSetup().getLegalDirectoryUrl().getFullUrl();
            // return QString(html).replace(QLatin1String("href=\"./"), "href=\"" + legalDir);
        }();

        ui->tbr_About->setHtml(htmlFixed);
        ui->tbr_About->setOpenLinks(false);

        // base URL
        // ui->tbr_About->document()->setMetaInformation(QTextDocument::DocumentUrl,
        // "https://datastore.swift-project.org/legal");
    }

    void CAboutHtmlComponent::onAnchorClicked(const QUrl &url)
    {
        if (!url.isRelative())
        {
            QDesktopServices::openUrl(url);
            return;
        }
        const QString possibleLegalFile =
            CFileUtils::appendFilePaths(CSwiftDirectories::legalDirectory(), url.fileName());
        QFile f(possibleLegalFile);
        if (f.exists()) { QDesktopServices::openUrl(QUrl::fromLocalFile(possibleLegalFile)); };
    }
} // namespace swift::gui::components
