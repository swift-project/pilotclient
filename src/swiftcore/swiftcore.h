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

#include "blackcore/context_runtime.h"
#include "blackgui/systemtraywindow.h"
#include "blackgui/components/enableforruntime.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/identifiable.h"

#include <QScopedPointer>

namespace Ui { class CSwiftCore; }

//! swift core control
class CSwiftCore :
    public BlackGui::CSystemTrayWindow,
    public BlackMisc::CIdentifiable,
    public BlackGui::Components::CEnableForRuntime
{
    Q_OBJECT

public:

    //! SwiftCore setup information
    struct SetupInfo
    {
        SetupInfo() {}

        bool m_minimzed = false; //!< Start minimized to tray
        QString m_dbusAddress;   //!< DBus address (session, system, p2p)
    };

    //! Constructor
    CSwiftCore(const SetupInfo &info, QWidget *parent = nullptr);

    //! Destructor
    ~CSwiftCore();

private slots:
    // PushButton slots
    void ps_startCorePressed();
    void ps_stopCorePressed();
    void ps_appendLogMessage(const BlackMisc::CStatusMessage &message);
    void ps_p2pModeToggled(bool checked);

    //! Style sheet has changed
    virtual void ps_onStyleSheetsChanged();

private:
    void initSlots();
    void initLogDisplay();
    void initStyleSheet();
    void startCore(const SetupInfo &setup);
    void stopCore();
    QString getDBusAddress() const;

    QScopedPointer<Ui::CSwiftCore> ui;
};

#endif // guard
