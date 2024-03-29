// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELMATCHERLOGENABLE_H
#define BLACKGUI_COMPONENTS_MODELMATCHERLOGENABLE_H

#include "blackmisc/network/connectionstatus.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CModelMatcherLogEnable;
}
namespace BlackGui::Components
{
    //! Enable/disable matching logs
    class CModelMatcherLogEnable : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelMatcherLogEnable(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelMatcherLogEnable() override;

        //! Reverse lookup enabled?
        bool isReverseLookupLogEnabled() const;

        //! Matching log.enabled
        bool isMatchingLogEnabled() const;

    private:
        QScopedPointer<Ui::CModelMatcherLogEnable> ui;

        //! Flag changed
        void enabledCheckboxChanged(bool enabled);

        //! Init GUI
        void initGui();

        //! Has required context
        bool hasContexts() const;

        //! When values changed elsewhere
        void valuesChanged();

        //! Connection status has been changed
        void connectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);
    };
} // ns

#endif // guard
