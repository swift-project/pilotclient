/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "infowindowcomponent.h"
#include "ui_infowindowcomponent.h"
#include "../stylesheetutility.h"
#include "blackmisc/icon.h"
#include <QTimer>
#include <QDesktopWidget>

using namespace BlackMisc;
using namespace BlackMisc::Network;


namespace BlackGui
{
    namespace Components
    {

        /*
         * Constructor
         */
        CInfoWindowComponent::CInfoWindowComponent(QWidget *parent) :
            QWizardPage(parent), ui(new Ui::InfoWindow)
        {
            ui->setupUi(this);
            this->hide();
            this->m_hideTimer = new QTimer(this);
            this->m_hideTimer->setSingleShot(true);
            this->onStyleSheetsChanged();

            connect(this->m_hideTimer, &QTimer::timeout, this, &CInfoWindowComponent::hide);
            connect(this->ui->pb_Close, &QPushButton::pressed, this, &CInfoWindowComponent::hide);
            connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CInfoWindowComponent::onStyleSheetsChanged);
        }

        /*
         * Destructor
         */
        CInfoWindowComponent::~CInfoWindowComponent() { }

        /*
         * Info message for some time
         */
        void CInfoWindowComponent::displayStringMessage(const QString &message, int displayTimeMs)
        {
            if (message.isEmpty())
            {
                this->hide();
                return;
            }

            // message and display
            this->ui->te_StringMessage->setText(message);
            this->setCurrentPage(this->ui->pg_StringMessage);
            this->showWindow(displayTimeMs);
        }

        /*
         * Info message for some time
         */
        void CInfoWindowComponent::displayTextMessage(const CTextMessage &textMessage, int displayTimeMs)
        {
            if (textMessage.isEmpty())
            {
                this->hide();
                return;
            }

            // message and display
            this->ui->le_TmFrom->setText(textMessage.getSenderCallsign().asString());
            this->ui->le_TmTo->setText(textMessage.getRecipientCallsign().asString());
            this->ui->le_TmReceived->setText(textMessage.receivedTime());
            this->ui->te_TmText->setText(textMessage.getMessage());

            this->setCurrentPage(this->ui->pg_TextMessage);
            this->showWindow(displayTimeMs);
        }

        /*
         * Display status message
         */
        void CInfoWindowComponent::displayStatusMessage(const CStatusMessage &statusMessage, int displayTimeMs)
        {
            if (statusMessage.isEmpty())
            {
                this->hide();
                return;
            }

            this->ui->le_SmSeverity->setText(statusMessage.getSeverityAsString());
            this->ui->le_SmType->setText(statusMessage.getCategories().toQString()); // TODO should be called ui->le_SmCategories
            this->ui->te_SmStatusMessage->setText(statusMessage.getMessage());
            this->ui->lbl_SmSeverityIcon->setPixmap(statusMessage.toPixmap());

            this->setCurrentPage(this->ui->pg_StatusMessage);
            this->showWindow(displayTimeMs);
        }

        /*
         * Display
         */
        void CInfoWindowComponent::display(const BlackMisc::CVariant &variant, int displayTimeMs)
        {
            if (variant.isNull()) return;
            if (variant.canConvert<CTextMessage>())
                this->displayTextMessage(variant.value<CTextMessage>(), displayTimeMs);
            else if (variant.canConvert<CStatusMessage>())
                this->displayStatusMessage(variant.value<CStatusMessage>(), displayTimeMs);
            else
                this->displayStringMessage(variant.toString(), displayTimeMs);
        }

        /*
         * Init this window
         */
        void CInfoWindowComponent::initWindow()
        {
            // center
            const QRect parent = this->parentWidget()->geometry();
            const QRect myself = this->rect();
            int dx = (parent.width() - myself.width()) / 2;
            int dy = (parent.height() - myself.height()) / 2;
            dy -= 80; // some offset, in order to display further on top
            this->move(dx, dy);
            this->show();
        }

        /*
         * Show window
         */
        void CInfoWindowComponent::showWindow(int displayTimeMs)
        {
            this->initWindow();

            // hide after some time
            this->m_hideTimer->start(displayTimeMs);
        }

        /*
         * Set current widget
         */
        void CInfoWindowComponent::setCurrentPage(QWidget *widget)
        {
            this->ui->sw_DifferentModes->setCurrentWidget(widget);
        }

        /*
         * Style sheet changed
         */
        void CInfoWindowComponent::onStyleSheetsChanged()
        {
            QString st = CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameInfoWindow());
            this->setStyleSheet(st);
        }
    }
} // namespace
