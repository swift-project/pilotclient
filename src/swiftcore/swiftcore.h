/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTCORE_H
#define SWIFTCORE_H

#include "blackgui/mainwindowaccess.h"
#include "blackgui/systemtraywindow.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/statusmessage.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

class QWidget;

namespace Ui { class CSwiftCore; }

/*!
 * swift core control GUI
 */
class CSwiftCore :
    public BlackGui::CSystemTrayWindow,
    public BlackGui::IMainWindowAccess,
    public BlackMisc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftCore(QWidget *parent = nullptr);

    //! Destructor
    virtual ~CSwiftCore();

private slots:
    //! \name PushButton slots
    //! @[
    void ps_startCorePressed();
    void ps_stopCorePressed();
    void ps_appendLogMessage(const BlackMisc::CStatusMessage &message);
    void ps_p2pModeToggled(bool checked);
    //! @}

    //! Style sheet has changed
    virtual void ps_onStyleSheetsChanged();

private:
    //! \name Init
    //! @[
    void initSlots();
    void initLogDisplay();
    void initStyleSheet();
    void initDBusMode();
    //! @}

    void startCore(const QString &dBusAdress);
    void stopCore();
    QString getDBusAddress() const;

    QScopedPointer<Ui::CSwiftCore> ui;
};

#endif // guard
