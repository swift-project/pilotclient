/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "managedstatusbar.h"
#include <QLayout>

using namespace BlackMisc;

namespace BlackGui
{

    CManagedStatusBar::CManagedStatusBar(QObject *parent) : QObject(parent)
    {
        this->setObjectName("qo_ManagedStatusBar");
    }

    CManagedStatusBar::~CManagedStatusBar()
    {
        // we are not necessarily the owner of the status bar
        this->m_statusBar->removeWidget(this->m_statusBarLabel);
        this->m_statusBar->removeWidget(this->m_statusBarIcon);

        // labels will be deleted with status bar
        if (this->m_ownStatusBar) { delete m_statusBar; }
    }

    void CManagedStatusBar::initStatusBar(QStatusBar *statusBar)
    {
        if (this->m_statusBar) { return; }
        this->m_ownStatusBar = statusBar ? false : true;
        this->m_statusBar = statusBar ? statusBar : new QStatusBar();
        if (this->m_statusBar->objectName().isEmpty()) { this->m_statusBar->setObjectName("sb_ManagedStatusBar"); }
        if (this->m_ownStatusBar) { m_statusBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);}

        this->m_statusBarIcon = new QLabel(this->m_statusBar);
        this->m_statusBarLabel = new QLabel(this->m_statusBar);
        this->m_statusBarLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        this->m_statusBarIcon->setObjectName(QString("lbl_StatusBarIcon").append(this->m_statusBar->objectName()));
        this->m_statusBarLabel->setObjectName(QString("lbl_StatusBarLabel").append(this->m_statusBar->objectName()));

        // use insert to insert from left to right
        // this keeps any grip on the right size
        this->m_statusBar->insertPermanentWidget(0, this->m_statusBarIcon, 0);  // status icon
        this->m_statusBar->insertPermanentWidget(1, this->m_statusBarLabel, 1); // status text

        // timer
        this->m_timerStatusBar = new QTimer(this);
        this->m_timerStatusBar->setObjectName(this->objectName().append(":m_timerStatusBar"));
        this->m_timerStatusBar->setSingleShot(true);
        connect(this->m_timerStatusBar, &QTimer::timeout, this, &CManagedStatusBar::ps_clearStatusBar);

        // done when injected status bar
        if (this->m_ownStatusBar)
        {
            // self created status bar
            QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            sizePolicy.setHorizontalStretch(0);
            sizePolicy.setVerticalStretch(0);
            sizePolicy.setHeightForWidth(this->m_statusBar->sizePolicy().hasHeightForWidth());

            this->m_statusBar->setSizePolicy(sizePolicy);
            this->m_statusBar->setSizeGripEnabled(false);
        }
    }

    void CManagedStatusBar::show()
    {
        if (!this->m_statusBar) { return; }
        this->m_statusBar->show();
    }

    void CManagedStatusBar::hide()
    {
        if (!this->m_statusBar) { return; }
        this->m_statusBar->hide();

        // reset minimum width
        if (this->m_ownStatusBar)
        {
            this->m_statusBar->setMinimumWidth(50);
        }
    }

    void CManagedStatusBar::displayStatusMessage(const CStatusMessage &statusMessage)
    {
        Q_ASSERT_X(this->m_statusBarIcon, Q_FUNC_INFO, "Missing status bar icon");
        Q_ASSERT_X(this->m_statusBar, Q_FUNC_INFO, "Missing status bar");

        if (statusMessage.isRedundant()) { return; }
        if (statusMessage.wasHandledBy(this)) { return; }
        statusMessage.markAsHandledBy(this);

        this->show();
        this->m_timerStatusBar->start(3000); // start / restart
        this->m_statusBarIcon->setPixmap(statusMessage.toPixmap());
        this->m_statusBarLabel->setText(statusMessage.getMessage());

        // restrict size for own status bars
        if (this->m_ownStatusBar)
        {
            QSize size = this->m_statusBar->window()->size();
            int w = qRound(0.95 * size.width());
            this->m_statusBar->setMaximumWidth(w);
        }
    }

    void CManagedStatusBar::displayStatusMessages(const BlackMisc::CStatusMessageList &statusMessages)
    {
        foreach(CStatusMessage m, statusMessages)
        {
            displayStatusMessage(m);
        }
    }

    void CManagedStatusBar::ps_clearStatusBar()
    {
        if (!this->m_statusBar) { return; }
        this->m_statusBarIcon->clear();
        this->m_statusBarLabel->clear();
    }

} // namespace
