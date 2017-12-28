/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/managedstatusbar.h"
#include "blackmisc/iterator.h"
#include "blackmisc/sequence.h"

#include <QLabel>
#include <QSize>
#include <QSizePolicy>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

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
        m_statusBar->removeWidget(m_statusBarLabel);
        m_statusBar->removeWidget(m_statusBarIcon);

        // labels will be deleted with status bar
        if (m_ownStatusBar) { delete m_statusBar; }
    }

    void CManagedStatusBar::initStatusBar(QStatusBar *statusBar)
    {
        if (m_statusBar) { return; }
        m_ownStatusBar = statusBar ? false : true;
        m_statusBar = statusBar ? statusBar : new QStatusBar();
        if (m_statusBar->objectName().isEmpty()) { m_statusBar->setObjectName("sb_ManagedStatusBar"); }
        if (m_ownStatusBar) { m_statusBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);}

        m_statusBarIcon = new QLabel(m_statusBar);
        m_statusBarLabel = new QLabel(m_statusBar);
        m_statusBarLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        m_statusBarIcon->setObjectName(QString("lbl_StatusBarIcon").append(m_statusBar->objectName()));
        m_statusBarLabel->setObjectName(QString("lbl_StatusBarLabel").append(m_statusBar->objectName()));

        // use insert to insert from left to right
        // this keeps any grip on the right size
        m_statusBar->insertPermanentWidget(0, m_statusBarIcon, 0);  // status icon
        m_statusBar->insertPermanentWidget(1, m_statusBarLabel, 1); // status text

        // timer
        m_timerStatusBar = new QTimer(this);
        m_timerStatusBar->setObjectName(this->objectName().append(":m_timerStatusBar"));
        m_timerStatusBar->setSingleShot(true);
        connect(m_timerStatusBar, &QTimer::timeout, this, &CManagedStatusBar::clearStatusBar);

        // done when injected status bar
        if (m_ownStatusBar)
        {
            // self created status bar
            QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            sizePolicy.setHorizontalStretch(0);
            sizePolicy.setVerticalStretch(0);
            sizePolicy.setHeightForWidth(m_statusBar->sizePolicy().hasHeightForWidth());

            m_statusBar->setMinimumHeight(24); // with no minimum height the layout always adjustes when displaying a status message
            m_statusBar->setSizePolicy(sizePolicy);
            m_statusBar->setSizeGripEnabled(false);
        }
    }

    void CManagedStatusBar::show()
    {
        if (!m_statusBar) { return; }
        m_statusBar->show();
    }

    void CManagedStatusBar::hide()
    {
        if (!m_statusBar) { return; }
        m_statusBar->hide();

        // reset minimum width
        if (m_ownStatusBar)
        {
            m_statusBar->setMinimumWidth(50);
        }
    }

    void CManagedStatusBar::displayStatusMessage(const CStatusMessage &statusMessage)
    {
        Q_ASSERT_X(m_statusBarIcon, Q_FUNC_INFO, "Missing status bar icon");
        Q_ASSERT_X(m_statusBar, Q_FUNC_INFO, "Missing status bar");

        // already displaying a message with severity higher than this one?
        if (!statusMessage.isSeverityHigherOrEqual(m_currentSeverity)) { return; }

        // used with log subscriber, make sure it is not displayed twice
        if (statusMessage.wasHandledBy(this)) { return; }
        statusMessage.markAsHandledBy(this);

        this->show();
        m_timerStatusBar->start(3000); // start / restart
        m_statusBarIcon->setPixmap(statusMessage.toPixmap());
        m_statusBarLabel->setText(statusMessage.getMessage());
        m_currentSeverity = statusMessage.getSeverity();

        // restrict size for own status bars
        if (m_ownStatusBar)
        {
            QSize size = m_statusBar->window()->size();
            int w = qRound(0.95 * size.width());
            m_statusBar->setMaximumWidth(w);
        }
    }

    void CManagedStatusBar::displayStatusMessages(const BlackMisc::CStatusMessageList &statusMessages)
    {
        foreach (CStatusMessage m, statusMessages)
        {
            displayStatusMessage(m);
        }
    }

    void CManagedStatusBar::clearStatusBar()
    {
        m_currentSeverity = StatusSeverity::SeverityDebug;
        if (!m_statusBar) { return; }
        m_statusBarIcon->clear();
        m_statusBarLabel->clear();
    }

} // namespace
