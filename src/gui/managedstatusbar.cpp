// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/managedstatusbar.h"

#include <QFontMetrics>
#include <QLabel>
#include <QSize>
#include <QSizePolicy>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

#include "misc/iterator.h"
#include "misc/sequence.h"

using namespace swift::misc;

namespace swift::gui
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
        m_statusBar->removeWidget(m_errorButton);
        m_statusBar->removeWidget(m_warningButton);

        // labels will be deleted with status bar
        if (m_ownedStatusBar) { delete m_statusBar; }
    }

    void CManagedStatusBar::initStatusBar(QStatusBar *statusBar)
    {
        if (m_statusBar) { return; }
        m_ownedStatusBar = statusBar ? false : true;
        m_statusBar = statusBar ? statusBar : new QStatusBar();
        if (m_statusBar->objectName().isEmpty()) { m_statusBar->setObjectName("sb_ManagedStatusBar"); }

        m_statusBarIcon = new QLabel(m_statusBar);
        m_statusBarIcon->setObjectName(QString("lbl_StatusBarIcon").append(m_statusBar->objectName()));
        m_statusBarLabel = new QLabel(m_statusBar);
        m_statusBarLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        m_statusBarLabel->setObjectName(QString("lbl_StatusBarLabel").append(m_statusBar->objectName()));
        m_warningButton = new QPushButton("WARN", m_statusBar);
        m_warningButton->setObjectName(QString("btn_StatusBarWarn").append(m_statusBar->objectName()));
        m_warningButton->setHidden(!m_showWarnButtonInitially);
        m_warningButton->setToolTip("ACK and show logs");
        connect(m_warningButton, &QPushButton::pressed, this, &CManagedStatusBar::pressedWarnButton);
        m_errorButton = new QPushButton("ERROR", m_statusBar);
        m_errorButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        m_errorButton->setObjectName(QString("btn_StatusBarError").append(m_statusBar->objectName()));
        m_errorButton->setHidden(!m_showErrorButtonInitially);
        m_errorButton->setToolTip("ACK and show logs");
        connect(m_errorButton, &QPushButton::pressed, this, &CManagedStatusBar::pressedErrorButton);

        // use insert to insert from left to right
        // this keeps any grip on the right size
        m_statusBar->insertPermanentWidget(0, m_statusBarIcon, 0); // status icon
        m_statusBar->insertPermanentWidget(1, m_statusBarLabel, 1); // status text
        m_statusBar->insertPermanentWidget(2, m_warningButton, 0);
        m_statusBar->insertPermanentWidget(3, m_errorButton, 0);

        // timer
        m_timerStatusBar.setObjectName(this->objectName().append(":m_timerStatusBar"));
        m_timerStatusBar.setSingleShot(true);
        connect(&m_timerStatusBar, &QTimer::timeout, this, &CManagedStatusBar::clearStatusBar);

        // done when injected status bar
        if (m_ownedStatusBar)
        {
            // self created status bar
            QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
            sizePolicy.setHorizontalStretch(0);
            sizePolicy.setVerticalStretch(0);
            sizePolicy.setHeightForWidth(m_statusBar->sizePolicy().hasHeightForWidth());

            m_statusBar->setMinimumHeight(
                24); // with no minimum height the layout always adjustes when displaying a status message
            m_statusBar->setSizePolicy(sizePolicy);
            m_statusBar->setSizeGripEnabled(false);
        }
    }

    void CManagedStatusBar::showWarningButton()
    {
        if (!m_warningButton) { m_showWarnButtonInitially = true; }
        else { m_warningButton->setHidden(false); }
    }

    void CManagedStatusBar::showErrorButton()
    {
        if (!m_errorButton) { m_showErrorButtonInitially = true; }
        else { m_errorButton->setHidden(false); }
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
        if (m_ownedStatusBar) { m_statusBar->setMinimumWidth(50); }
    }

    void CManagedStatusBar::displayStatusMessage(const CStatusMessage &statusMessage)
    {
        Q_ASSERT_X(m_statusBarIcon, Q_FUNC_INFO, "Missing status bar icon");
        Q_ASSERT_X(m_statusBar, Q_FUNC_INFO, "Missing status bar");
        Q_ASSERT_X(m_statusBarLabel, Q_FUNC_INFO, "Missing label");

        // already displaying a message with severity higher than this one?
        if (statusMessage.isEmpty()) { return; }
        if (!statusMessage.isSeverityHigherOrEqual(m_currentSeverity)) { return; }

        // used with log subscriber, make sure it is not displayed twice
        if (statusMessage.wasHandledBy(this)) { return; }
        statusMessage.markAsHandledBy(this);

        this->show();
        m_timerStatusBar.start(3000); // start / restart
        m_statusBarIcon->setPixmap(CIcon(statusMessage.toIcon()));

        // restrict size for own status bars
        const QSize size = m_statusBar->window()->size();
        const int w = qRound(0.95 * size.width());
        if (m_ownedStatusBar) { m_statusBar->setMaximumWidth(w); }

        if (m_elideMode != Qt::ElideNone)
        {
            const QFontMetrics metrics(m_statusBarLabel->font());
            const QString elidedText = metrics.elidedText(statusMessage.getMessage(), m_elideMode, qRound(0.90 * w));
            m_statusBarLabel->setText(elidedText);
        }
        else { m_statusBarLabel->setText(statusMessage.getMessage()); }
        m_currentSeverity = statusMessage.getSeverity();
    }

    void CManagedStatusBar::displayStatusMessages(const CStatusMessageList &statusMessages)
    {
        for (const CStatusMessage &m : statusMessages) { this->displayStatusMessage(m); }
    }

    void CManagedStatusBar::setSizeGripEnabled(bool enabled)
    {
        if (m_statusBar) { m_statusBar->setSizeGripEnabled(enabled); }
    }

    void CManagedStatusBar::clearStatusBar()
    {
        m_currentSeverity = StatusSeverity::SeverityDebug;
        if (!m_statusBar) { return; }
        m_statusBarIcon->clear();
        m_statusBarLabel->clear();
    }

    void CManagedStatusBar::pressedWarnButton()
    {
        Q_ASSERT_X(m_warningButton, Q_FUNC_INFO, "Missing warning button");
        m_warningButton->setHidden(true);
        emit requestLogPage();
    }

    void CManagedStatusBar::pressedErrorButton()
    {
        Q_ASSERT_X(m_errorButton, Q_FUNC_INFO, "Missing error button");
        m_errorButton->setHidden(true);
        emit requestLogPage();
    }

} // namespace swift::gui
