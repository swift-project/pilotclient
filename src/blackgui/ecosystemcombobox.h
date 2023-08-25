// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_ECOSYSTEMCOMBOBOX_H
#define BLACKGUI_ECOSYSTEMCOMBOBOX_H

#include "blackmisc/network/ecosystemlist.h"
#include <QComboBox>

namespace BlackGui
{
    //! Combo box widget for ecosystems
    class CEcosystemComboBox : public QComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        CEcosystemComboBox(QWidget *parent = nullptr);

        //! Constructor
        CEcosystemComboBox(const BlackMisc::Network::CEcosystemList &systems, QWidget *parent = nullptr);

        //! Destructor
        virtual ~CEcosystemComboBox() {}

        //! The selected ecosystem
        BlackMisc::Network::CEcosystem getSelectedEcosystem() const;

        //! Set current system
        void setCurrentEcosystem(const BlackMisc::Network::CEcosystem &ecosystem);

        //! Set the supported systems
        void setEcosystems(const BlackMisc::Network::CEcosystemList &systems);

    private:
        //! Init all items
        void initAllItems();

        BlackMisc::Network::CEcosystemList m_systems { BlackMisc::Network::CEcosystemList::allKnownSystems() };
    };
} // ns
#endif // guard
