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
#include "ui_abouthtmlcomponent.h"
#include <QTimer>
#include <QPointer>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CAboutHtmlComponent::CAboutHtmlComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAboutHtmlComponent)
        {
            ui->setupUi(this);
            const QPointer<CAboutHtmlComponent> myself(this);
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
            // make links absolute
            static const QString htmlFixed = [ = ]
            {
                // workaround:
                // 1) Only reading as HTML gives proper formatting
                // 2) Reading the file resource fails (likely because of the style sheet)
                const QString html = CFileUtils::readFileToString(CDirectoryUtils::aboutFilePath());
                const QString legalDir = sGui->getGlobalSetup().getLegalDirectoryUrl().getFullUrl();
                return QString(html).replace(QLatin1String("href=\"./"), "href=\"" + legalDir);
            }();
            ui->tbr_About->setHtml(htmlFixed);
        }
    } // ns
} // ns
