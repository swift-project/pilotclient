/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
