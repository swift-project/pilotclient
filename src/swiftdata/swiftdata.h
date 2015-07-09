/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include "blackcore/context_runtime.h"
#include "blackgui/systemtraywindow.h"
#include "blackgui/components/enableforruntime.h"
#include "blackgui/managedstatusbar.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/identifiable.h"
#include <QScopedPointer>

namespace Ui { class CSwiftData; }
namespace BlackCore { class CWebDataReader; }

//! swift data entry control
class CSwiftData :
    public QMainWindow,
    public BlackMisc::CIdentifiable,
    public BlackGui::Components::CEnableForRuntime
{
    Q_OBJECT

public:
    //! Constructor
    CSwiftData(QWidget *parent = nullptr);

    //! Destructor
    ~CSwiftData();

private slots:
    //! Append log message
    void ps_appendLogMessage(const BlackMisc::CStatusMessage &message);

    //! Style sheet has changed
    void ps_onStyleSheetsChanged();

private:
    void init();
    void initLogDisplay();
    void initStyleSheet();
    void initReaders();
    QScopedPointer<Ui::CSwiftData> ui;
    BlackGui::CManagedStatusBar    m_statusBar;
    BlackCore::CWebDataReader     *m_webDataReader = nullptr;
};

#endif // guard
