// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBDEBUGDATABASESETUP_H
#define BLACKGUI_COMPONENTS_DBDEBUGDATABASESETUP_H

#include "blackcore/data/globalsetup.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/datacache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbDebugDatabaseSetup;
}
namespace BlackGui::Components
{
    /*!
     * Debug settings for DB (only to be used as developer)
     * \remarks Disabled when not runnig in dev.environment
     */
    class BLACKGUI_EXPORT CDbDebugDatabaseSetup : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbDebugDatabaseSetup(QWidget *parent = nullptr);

        //! Dstructor
        ~CDbDebugDatabaseSetup();

    private:
        //! Changed the debug checkbox
        void onDebugChanged(bool set);

        QScopedPointer<Ui::CDbDebugDatabaseSetup> ui;
        BlackMisc::CData<BlackCore::Data::TGlobalSetup> m_setup { this }; //!< data cache
    };
} // ns

#endif // guard
