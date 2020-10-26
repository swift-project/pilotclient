/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ecosystemcombobox.h"
#include "blackmisc/network/ecosystemlist.h"

using namespace BlackMisc::Network;

namespace BlackGui
{
    CEcosystemComboBox::CEcosystemComboBox(QWidget *parent) : QComboBox(parent)
    {
        this->initAllItems();
    }

    CEcosystemComboBox::CEcosystemComboBox(const CEcosystemList &systems, QWidget *parent) :
        QComboBox(parent), m_systems(systems)
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
            this->addItem(BlackMisc::CIcon(e.toIcon()).toPixmap(), e.getSystemString());
        }
    }
} // ns
