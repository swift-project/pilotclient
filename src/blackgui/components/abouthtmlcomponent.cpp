/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"
#include "abouthtmlcomponent.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "ui_abouthtmlcomponent.h"

#include <QFile>
#include <QTimer>
#include <QPointer>
#include <QDesktopServices>

using namespace BlackMisc;

namespace BlackGui::Components
{
    CAboutHtmlComponent::CAboutHtmlComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CAboutHtmlComponent)
    {
        ui->setupUi(this);
        const QPointer<CAboutHtmlComponent> myself(this);
        connect(ui->tbr_About, &QTextBrowser::anchorClicked, this, &CAboutHtmlComponent::onAnchorClicked, Qt::QueuedConnection);

        QTimer::singleShot(2500, this, [ = ]
        {
            if (!myself) { return; }
            myself->loadAbout();
        });
    }

    CAboutHtmlComponent::~CAboutHtmlComponent()
    { }

    void CAboutHtmlComponent::loadAbout()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        // make links absolute
        static const QString htmlFixed = [ = ]
        {
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
        // ui->tbr_About->document()->setMetaInformation(QTextDocument::DocumentUrl, "https://datastore.swift-project.org/legal");
    }

    void CAboutHtmlComponent::onAnchorClicked(const QUrl &url)
    {
        if (!url.isRelative())
        {
            QDesktopServices::openUrl(url);
            return;
        }
        const QString possibleLegalFile = CFileUtils::appendFilePaths(CSwiftDirectories::legalDirectory(), url.fileName());
        QFile f(possibleLegalFile);
        if (f.exists())
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(possibleLegalFile));
        };
    }
} // ns
