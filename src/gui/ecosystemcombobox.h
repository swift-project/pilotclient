// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ECOSYSTEMCOMBOBOX_H
#define SWIFT_GUI_ECOSYSTEMCOMBOBOX_H

#include "misc/network/ecosystemlist.h"
#include <QComboBox>

namespace swift::gui
{
    //! Combo box widget for ecosystems
    class CEcosystemComboBox : public QComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        CEcosystemComboBox(QWidget *parent = nullptr);

        //! Constructor
        CEcosystemComboBox(const swift::misc::network::CEcosystemList &systems, QWidget *parent = nullptr);

        //! Destructor
        virtual ~CEcosystemComboBox() {}

        //! The selected ecosystem
        swift::misc::network::CEcosystem getSelectedEcosystem() const;

        //! Set current system
        void setCurrentEcosystem(const swift::misc::network::CEcosystem &ecosystem);

        //! Set the supported systems
        void setEcosystems(const swift::misc::network::CEcosystemList &systems);

    private:
        //! Init all items
        void initAllItems();

        swift::misc::network::CEcosystemList m_systems { swift::misc::network::CEcosystemList::allKnownSystems() };
    };
} // ns
#endif // guard
