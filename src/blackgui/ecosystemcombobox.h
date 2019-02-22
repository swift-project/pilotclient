/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
