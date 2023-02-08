/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "textmessagesettings.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/simulation/simulatedaircraft.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackGui::Settings, CTextMessageSettings)

namespace BlackGui::Settings
{
    CTextMessageSettings::CTextMessageSettings()
    { }

    bool CTextMessageSettings::popupPrivateMessages() const
    {
        return this->getPopupAllMessages() || m_popupPrivateMessages;
    }

    bool CTextMessageSettings::popupSupervisorMessages() const
    {
        return this->getPopupAllMessages() || m_popupSupervisorMessages;
    }

    bool CTextMessageSettings::popupFrequencyMessages() const
    {
        return this->getPopupAllMessages() || m_popupFrequencyMessages;
    }

    bool CTextMessageSettings::popupSelcalMessages() const
    {
        return this->getPopupAllMessages() || m_popupSelcalMessages;
    }

    void CTextMessageSettings::disableAllPopups()
    {
        m_popupPrivateMessages    = false;
        m_popupSupervisorMessages = false;
        m_popupFrequencyMessages  = false;
        m_popupAllMessages        = false;
        m_popupSelcalMessages     = false;
        m_focus                   = false;
    }

    bool CTextMessageSettings::popup(const CTextMessage &textMessage) const
    {
        if (this->getPopupAllMessages()) { return true; }
        if (textMessage.isPrivateMessage() && this->popupPrivateMessages()) { return true; }
        if (textMessage.isSupervisorMessage() && this->popupSupervisorMessages())  { return true; }
        if (textMessage.isSelcalMessage() && this->popupSelcalMessages()) { return true; }
        return false;
    }

    bool CTextMessageSettings::popup(const CTextMessage &textMessage, const CSimulatedAircraft &ownAircraft) const
    {
        if (this->popup(textMessage)) { return true; }
        if (textMessage.isRadioMessage())
        {
            if (!this->popupFrequencyMessages()) { return false; }
            if (ownAircraft.hasComActiveFrequency(textMessage.getFrequency())) { return true; }
        }
        return false;
    }

    QString CTextMessageSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("Private: %1 supervisor: %2 frequency: %3 all: %4 focus: %5 latest 1st: %6").arg(
                    boolToOnOff(this->getPopupPrivateMessages()),
                    boolToOnOff(this->getPopupSupervisorMessages()),
                    boolToOnOff(this->getPopupFrequencyMessages()),
                    boolToOnOff(this->getPopupAllMessages()),
                    boolToYesNo(this->focusOverlayWindow()),
                    boolToYesNo(this->isLatestFirst())
                );
    }

    QVariant CTextMessageSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPopupAllMessages:        return QVariant::fromValue(this->getPopupAllMessages());
        case IndexPopupFrequencyMessages:  return QVariant::fromValue(this->popupFrequencyMessages());
        case IndexPopupPrivateMessages:    return QVariant::fromValue(this->popupPrivateMessages());
        case IndexPopupSupervisorMessages: return QVariant::fromValue(this->popupSupervisorMessages());
        case IndexPopupSelcalMessages:     return QVariant::fromValue(this->popupSelcalMessages());
        case IndexStyle:       return QVariant::fromValue(this->getStyleSheet());
        case IndexLatestFirst: return QVariant::fromValue(this->isLatestFirst());
        case IndexFocus:       return QVariant::fromValue(this->focusOverlayWindow());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CTextMessageSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CTextMessageSettings>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexPopupAllMessages:        this->setPopupAllMessages(variant.toBool()); break;
        case IndexPopupFrequencyMessages:  this->setPopupFrequencyMessages(variant.toBool()); break;
        case IndexPopupSupervisorMessages: this->setSupervisorMessages(variant.toBool());   break;
        case IndexPopupPrivateMessages:    this->setPopupPrivateMessages(variant.toBool()); break;
        case IndexPopupSelcalMessages:     this->setPopupSelcalMessages(variant.toBool());  break;
        case IndexFocus:       this->setFocusOverlayWindows(variant.toBool()); break;
        case IndexLatestFirst: this->setLatestFirst(variant.toBool()); break;
        case IndexStyle:       this->setStyleSheet(variant.toString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // ns
