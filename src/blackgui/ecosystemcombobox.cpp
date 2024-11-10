// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ecosystemcombobox.h"
#include "misc/network/ecosystemlist.h"

using namespace swift::misc::network;

namespace BlackGui
{
    CEcosystemComboBox::CEcosystemComboBox(QWidget *parent) : QComboBox(parent)
    {
        this->initAllItems();
    }

    CEcosystemComboBox::CEcosystemComboBox(const CEcosystemList &systems, QWidget *parent) : QComboBox(parent), m_systems(systems)
    {
        this->initAllItems();
    }

    CEcosystem CEcosystemComboBox::getSelectedEcosystem() const
    {
        if (this->currentIndex() < 0 || this->currentIndex() >= m_systems.size()) { return CEcosystem(); }
        return m_systems[this->currentIndex()];
    }

    void CEcosystemComboBox::setCurrentEcosystem(const CEcosystem &ecosystem)
    {
        if (m_systems.contains(ecosystem))
        {
            this->setCurrentText(ecosystem.getSystemString());
        }
    }

    void CEcosystemComboBox::setEcosystems(const CEcosystemList &systems)
    {
        m_systems = systems;
        this->initAllItems();
    }

    void CEcosystemComboBox::initAllItems()
    {
        this->clear();
        for (const CEcosystem &e : m_systems)
        {
            this->addItem(e.getSystemString());
        }
    }
} // ns
