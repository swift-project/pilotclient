// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/callsign.h"
#include "misc/mixin/mixincompare.h"
#include "misc/stringutils.h"
#include "misc/statusmessagelist.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <Qt>
#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CCallsign)

namespace swift::misc::aviation
{
    CCallsign::CCallsign(const QString &callsign, CCallsign::TypeHint hint)
        : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_typeHint(hint)
    {}

    CCallsign::CCallsign(const QString &callsign, const QString &telephonyDesignator, CCallsign::TypeHint hint)
        : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_telephonyDesignator(telephonyDesignator.trimmed()), m_typeHint(hint)
    {}

    CCallsign::CCallsign(const char *callsign, CCallsign::TypeHint hint)
        : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_typeHint(hint)
    {}

    void CCallsign::registerMetadata()
    {
        CValueObject<CCallsign>::registerMetadata();
        qRegisterMetaType<CCallsign::TypeHint>();
    }

    QString CCallsign::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return m_callsign;
    }

    void CCallsign::clear()
    {
        *this = CCallsign();
    }

    int CCallsign::suffixToSortOrder(const QString &suffix)
    {
        if (QStringView(u"FSS") == suffix) { return 1; }
        if (QStringView(u"CTR") == suffix) { return 2; }
        if (QStringView(u"APP") == suffix) { return 3; }
        if (QStringView(u"DEP") == suffix) { return 4; }
        if (QStringView(u"TWR") == suffix) { return 5; }
        if (QStringView(u"GND") == suffix) { return 6; }
        if (QStringView(u"DEL") == suffix) { return 7; }
        if (QStringView(u"ATIS") == suffix) { return 8; }
        if (QStringView(u"SUP") == suffix) { return 9; }
        if (QStringView(u"OBS") == suffix) { return 10; }
        if (QStringView(u"INS") == suffix) { return 11; } // instructor/mentor
        if (QStringView(u"ADM") == suffix) { return 12; } // admin
        if (QStringView(u"VATGOV") == suffix) { return 13; } // VATSIM governors
        if (QStringView(u"VATSIM") == suffix) { return 14; } // VATSIM founder
        if (QStringView(u"EXAM") == suffix) { return 15; }
        return std::numeric_limits<int>::max();
    }

    QString CCallsign::unifyCallsign(const QString &callsign, TypeHint hint)
    {
        const QString ucCallsign = callsign.trimmed().toUpper();

        // Ref T664, allow ATC with hyphen, such as Ml-SNO_CTR
        switch (hint)
        {
        // ATC allows "-", aircraft not
        case Atc: return removeChars(ucCallsign, [](QChar c) { return !c.isLetterOrNumber() && c != '_' && c != '-'; });
        case Aircraft: return removeChars(ucCallsign, [](QChar c) { return !c.isLetterOrNumber() && c != '_'; });
        default: break;
        }

        // no hint
        if (CCallsign::looksLikeAtcCallsign(ucCallsign))
        {
            return removeChars(ucCallsign, [](QChar c) { return !c.isLetterOrNumber() && c != '_' && c != '-'; });
        }

        // strict check
        return removeChars(ucCallsign, [](QChar c) { return !c.isLetterOrNumber() && c != '_'; });
    }

    const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
    {
        if (callsign.m_callsign.startsWith(QStringView(u"VATGOV"))) { return CIcon::iconByIndex(CIcons::NetworkRolePilot); }
        const bool pilot = callsign.getTypeHint() == CCallsign::Aircraft || !callsign.hasSuffix();
        return pilot ?
                   CIcon::iconByIndex(CIcons::NetworkRolePilot) :
                   CCallsign::atcSuffixToIcon(callsign.getSuffix());
    }

    CStatusMessage CCallsign::logMessage(const CCallsign &callsign, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
    {
        static const CLogCategoryList cats({ CLogCategories::aviation() });
        const CStatusMessage m(cats.with(CLogCategoryList::fromQStringList(extraCategories)), s, callsign.isEmpty() ? message.trimmed() : callsign.toQString() + ": " + message.trimmed());
        return m;
    }

    void CCallsign::addLogDetailsToList(CStatusMessageList *log, const CCallsign &callsign, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
    {
        if (!log) { return; }
        if (message.isEmpty()) { return; }
        log->push_back(logMessage(callsign, message, extraCategories, s));
    }

    const CIcon &CCallsign::atcSuffixToIcon(const QString &suffix)
    {
        if (suffix.length() < 3) { return CIcon::iconByIndex(CIcons::NetworkRoleUnknown); }
        const QString sfx = suffix.toUpper();
        if (QStringView(u"APP") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleApproach); }
        if (QStringView(u"DEP") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleDeparture); }
        if (QStringView(u"GND") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleGround); }
        if (QStringView(u"TWR") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleTower); }
        if (QStringView(u"DEL") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleDelivery); }
        if (QStringView(u"CTR") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleCenter); }
        if (QStringView(u"SUP") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleSup); }
        if (QStringView(u"OBS") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleObs); }
        if (QStringView(u"INS") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
        if (QStringView(u"FSS") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleFss); }
        if (QStringView(u"ATIS") == sfx) { return CIcon::iconByIndex(CIcons::AviationAtis); }
        if (QStringView(u"EXAM") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
        if (QStringView(u"VATSIM") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleSup); }
        if (QStringView(u"VATGOV") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleSup); }
        return CIcon::iconByIndex(CIcons::NetworkRoleUnknown);
    }

    bool CCallsign::isAtcCallsign() const
    {
        if (this->getTypeHint() == Atc) { return true; }
        if (!this->hasSuffix()) { return false; }
        return atcCallsignSuffixes().contains(this->getSuffix(), Qt::CaseInsensitive);
    }

    bool CCallsign::isSupervisorCallsign() const
    {
        if (this->getTypeHint() == Aircraft) { return false; }
        return m_callsign.endsWith("SUP");
    }

    bool CCallsign::isBroadcastCallsign() const
    {
        return m_callsignAsSet == "*" || m_callsign == "*" || m_callsignAsSet == "BROADCAST";
    }

    void CCallsign::markAsBroadcastCallsign()
    {
        m_callsignAsSet = "BROADCAST";
        m_callsign = "BROADCAST";
    }

    void CCallsign::markAsWallopCallsign()
    {
        m_callsignAsSet = "SUP";
        m_callsign = "SUP";
    }

    bool CCallsign::isMaybeCopilotCallsign(const CCallsign &pilotCallsign) const
    {
        return m_callsign.startsWith(pilotCallsign.asString()) &&
               m_callsign.size() == pilotCallsign.asString().size() + 1 &&
               m_callsign.at(m_callsign.size() - 1) >= 'A' && m_callsign.at(m_callsign.size() - 1) <= 'Z';
    }

    QString CCallsign::getFsdCallsignString() const
    {
        // mainly used to fix the cross coupled callsigns such as *EDDF_TWR
        if (m_callsignAsSet.startsWith('*')) { return this->getStringAsSet(); }
        return this->asString();
    }

    bool CCallsign::isSameAsSet() const
    {
        return m_callsign == m_callsignAsSet;
    }

    QString CCallsign::getIcaoCode() const
    {
        if (this->isAtcCallsign())
        {
            if (m_callsign.length() >= 4)
            {
                return m_callsign.left(4).toUpper();
            }
        }
        return {};
    }

    bool CCallsign::isAtcAlikeCallsign() const
    {
        if (this->getTypeHint() == Aircraft) { return false; }
        if (!this->hasSuffix()) { return false; }
        return atcAlikeCallsignSuffixes().contains(this->getSuffix(), Qt::CaseInsensitive);
    }

    bool CCallsign::isObserverCallsign() const
    {
        return m_callsignAsSet.endsWith("_OBS", Qt::CaseInsensitive);
    }

    QString CCallsign::getAsObserverCallsignString() const
    {
        if (this->isEmpty()) { return {}; }
        QString obs = this->getStringAsSet();
        if (obs.endsWith("_OBS", Qt::CaseInsensitive)) { return obs; } // already OBS
        if (obs.contains('_')) { obs = obs.left(obs.lastIndexOf('_')); }
        return obs.append("_OBS").toUpper();
    }

    QString CCallsign::getSuffix() const
    {
        QString s;
        if (this->hasSuffix())
        {
            s = this->getStringAsSet().section('_', -1).toUpper();
        }
        return s;
    }

    QString CCallsign::getAirlinePrefix() const
    {
        QString flightNumber;
        return this->getAirlinePrefix(flightNumber);
    }

    QString CCallsign::getAirlinePrefix(QString &flightNumber) const
    {
        QString identification;
        return this->getAirlinePrefix(flightNumber, identification);
    }

    QString CCallsign::getAirlinePrefix(QString &flightNumber, QString &flightIdentification) const
    {
        // DLH1WP only 11 number
        // UPSE123 4 characters, then number

        flightNumber.clear();
        if (m_callsign.length() < 3) { return {}; }
        if (this->isAtcCallsign()) { return {}; }

        thread_local const QRegularExpression regExp("(^[A-Z]{3,})(\\d+)");
        const QRegularExpressionMatch match = regExp.match(m_callsign);
        if (!match.hasMatch()) { return {}; }
        // 0 is whole capture
        const QString airline = match.captured(1);
        flightNumber = match.captured(2); // null string if not exits

        // hard facts
        if (airline.length() == 3) // we allow 3 letters
        {
            flightIdentification = m_callsign.length() > 3 ? m_callsign.mid(3) : QString();
            return airline;
        }
        if (airline.length() == 4 && airline.startsWith('V')) // we allow virtual 4 letter codes, e.g. VDLD
        {
            flightIdentification = m_callsign.length() > 4 ? m_callsign.mid(4) : QString();
            return airline;
        }

        // some people use callsigns like UPSE123
        if (flightNumber.length() >= 1 && airline.length() == 4)
        {
            flightIdentification = m_callsign.mid(3);
            return airline.left(3);
        }

        return {}; // invalid
    }

    QString CCallsign::getFlightIndentification() const
    {
        if (this->isAtcCallsign()) { return {}; }
        QString flightNumber;
        QString identification;
        const QString airline = this->getAirlinePrefix(flightNumber, identification);
        return airline.isEmpty() ? QString() : identification;
    }

    QString CCallsign::getFlightNumber() const
    {
        if (this->isAtcCallsign()) { return {}; }
        QString flightNumber;
        const QString airline = this->getAirlinePrefix(flightNumber);
        return airline.isEmpty() ? QString() : flightNumber;
    }

    int CCallsign::getFlightNumberInt() const
    {
        if (this->isAtcCallsign()) { return -1; }
        bool ok;
        const int fn = this->getFlightNumber().toInt(&ok);
        return ok ? fn : -1;
    }

    bool CCallsign::hasSuffix() const
    {
        return this->getStringAsSet().contains('_');
    }

    bool CCallsign::hasAtcSuffix() const
    {
        const QString s = this->getSuffix();
        return !s.isEmpty() && atcCallsignSuffixes().contains(s);
    }

    int CCallsign::getSuffixSortOrder() const
    {
        return suffixToSortOrder(this->getSuffix());
    }

    bool CCallsign::equalsString(const QString &callsignString) const
    {
        CCallsign other(callsignString);
        return other == (*this);
    }

    QVariant CCallsign::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsignString: return QVariant(this->asString());
        case IndexCallsignStringAsSet: return QVariant(this->getStringAsSet());
        case IndexTelephonyDesignator: return QVariant(this->getTelephonyDesignator());
        case IndexSuffix: return QVariant(this->getSuffix());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CCallsign::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCallsign>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsignString: m_callsign = unifyCallsign(variant.toString()); break;
        case IndexCallsignStringAsSet: m_callsignAsSet = variant.toString(); break;
        case IndexTelephonyDesignator: m_telephonyDesignator = variant.toString(); break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    int CCallsign::comparePropertyByIndex(CPropertyIndexRef index, const CCallsign &compareValue) const
    {
        if (index.isMyself()) { return m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsignString: return m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive);
        case IndexCallsignStringAsSet: return m_callsignAsSet.compare(compareValue.m_callsignAsSet, Qt::CaseInsensitive);
        case IndexTelephonyDesignator: return m_telephonyDesignator.compare(compareValue.m_telephonyDesignator, Qt::CaseInsensitive);
        case IndexSuffix: return this->getSuffix().compare(compareValue.getSuffix(), Qt::CaseInsensitive);
        default:
            return CValueObject::comparePropertyByIndex(index, compareValue);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }

    bool CCallsign::isValid() const
    {
        switch (m_typeHint)
        {
        case Atc: return isValidAtcCallsign(*this);
        case Aircraft: return isValidAircraftCallsign(*this);
        default: return !this->isEmpty();
        }
    }

    bool CCallsign::isValidAircraftCallsign(const QString &callsign)
    {
        if (callsign.length() < 2 || callsign.length() > 10) { return false; }
        return !containsChar(callsign, [](QChar c) { return !c.isUpper() && !c.isDigit(); });
    }

    bool CCallsign::isValidAircraftCallsign(const CCallsign &callsign)
    {
        return isValidAircraftCallsign(callsign.asString());
    }

    bool CCallsign::isValidAtcCallsign(const QString &callsign)
    {
        // Ref T664, allow ATC with hyphen, such as Ml-SNO_CTR
        if (callsign.length() < 2 || callsign.length() > 10) { return false; }
        return !containsChar(callsign, [](QChar c) { return c != '-' && c != '_' && !c.isUpper() && !c.isDigit(); });
    }

    bool CCallsign::isValidAtcCallsign(const CCallsign &callsign)
    {
        return isValidAtcCallsign(callsign.asString());
    }

    const QStringList &CCallsign::atcCallsignSuffixes()
    {
        static const QStringList a({ "APP", "GND", "DEP", "TWR", "DEL", "CTR" });
        return a;
    }

    const QStringList &CCallsign::atcAlikeCallsignSuffixes()
    {
        static const QStringList a({ "ATIS", "APP", "GND", "OBS", "DEP", "TWR", "DEL", "CTR", "SUP", "FSS", "INS" });
        return a;
    }

    bool CCallsign::looksLikeAtcCallsign(const QString &callsign)
    {
        if (!callsign.contains("_")) { return false; }
        const QStringView uc = callsign.toUpper();

        for (const QString &r : CCallsign::atcAlikeCallsignSuffixes())
        {
            if (uc.endsWith(r)) { return true; }
        }
        return false;
    }
} // namespace
