/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/actionhotkey.h"
#include <QStringBuilder>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Input, CActionHotkey)

namespace BlackMisc::Input
{
    CActionHotkey::CActionHotkey(const QString &action) : m_action(action)
    {}

    CActionHotkey::CActionHotkey(const CIdentifier &identifier, const CHotkeyCombination &combination, const QString &action) : m_identifier(identifier), m_combination(combination), m_action(action)
    {}

    QString CActionHotkey::convertToQString(bool /* i18n */) const
    {
        const QString s =
            m_identifier.getMachineName() %
            u' ' %
            m_combination.asStringWithDeviceNames() %
            // m_combination.toQString() %
            u' ' %
            m_action;
        return s;
    }

    void CActionHotkey::setCombination(const CHotkeyCombination &combination)
    {
        m_combination = combination;
    }

    bool CActionHotkey::isForSameMachineId(const CActionHotkey &key) const
    {
        return this->getApplicableMachine().hasSameMachineId(key.getApplicableMachine());
    }

    bool CActionHotkey::isForSameMachineName(const CActionHotkey &key) const
    {
        return this->getApplicableMachine().hasSameMachineName(key.getApplicableMachine());
    }

    bool CActionHotkey::isForSameMachine(const CActionHotkey &key) const
    {
        return this->isForSameMachineId(key) || this->isForSameMachineName(key);
    }

    void CActionHotkey::updateToCurrentMachine()
    {
        m_identifier.updateToCurrentMachine();
    }

    void CActionHotkey::setObject(const CActionHotkey &obj)
    {
        m_action = obj.m_action;
        m_combination = obj.m_combination;
    }

    QVariant CActionHotkey::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIdentifier: return QVariant::fromValue(m_identifier);
        case IndexIdentifierAsString: return QVariant::fromValue(m_identifier.getMachineName());
        case IndexAction: return QVariant::fromValue(m_action);
        case IndexActionAsString: return QVariant::fromValue(m_action);
        case IndexCombination: return QVariant::fromValue(m_combination);
        case IndexCombinationAsString: return QVariant::fromValue(QString(m_combination.toQString()));
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CActionHotkey::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CActionHotkey>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAction:
        {
            m_action = variant.value<QString>();
            break;
        }
        case IndexCombination:
        case IndexCombinationAsString:
            m_combination = variant.value<CHotkeyCombination>();
            break;
        case IndexObject:
            this->setObject(variant.value<CActionHotkey>());
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // BlackMisc
