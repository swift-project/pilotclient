/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/comparefunctions.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Network
    {
        CFsdSetup::CFsdSetup(SendReceiveDetails sendReceive) : m_sendReceive(sendReceive)
        { }

        CFsdSetup::CFsdSetup(const QString &codec, SendReceiveDetails sendReceive)
            : m_textCodec(codec), m_sendReceive(sendReceive) {}

        CFsdSetup::SendReceiveDetails CFsdSetup::getSendReceiveDetails() const
        {
            return static_cast<SendReceiveDetails>(m_sendReceive);
        }

        QString CFsdSetup::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString s("Codec: '%1' details: '%2'");
            return s.arg(this->getTextCodec(), CFsdSetup::sendReceiveDetailsToString(this->getSendReceiveDetails()));
        }

        QString CFsdSetup::sendReceiveDetailsToString(SendReceiveDetails details)
        {
            static const QString ds("Send parts; %1 gnd: %2 interim: %3 Receive parts: %4 gnd: %5 interim: %6 3letter: %7");
            return ds.arg(boolToYesNo(details.testFlag(SendAircraftParts)),
                          boolToYesNo(details.testFlag(SendGndFlag)),
                          boolToYesNo(details.testFlag(SendInterimPositions)),
                          boolToYesNo(details.testFlag(SendVisualPositions)),
                          boolToYesNo(details.testFlag(ReceiveAircraftParts)),
                          boolToYesNo(details.testFlag(ReceiveGndFlag)),
                          boolToYesNo(details.testFlag(ReceiveInterimPositions)),
                          boolToYesNo(details.testFlag(Force3LetterAirlineICAO))
                         );
        }

        void CFsdSetup::setSendReceiveDetails(bool partsSend, bool partsReceive, bool gndSend, bool gndReceive, bool interimSend, bool interimReceive, bool visualSend)
        {
            SendReceiveDetails s = Nothing;
            if (partsSend)    { s |= SendAircraftParts; }
            if (partsReceive) { s |= ReceiveAircraftParts; }
            if (gndSend)      { s |= SendGndFlag; }
            if (gndReceive)   { s |= ReceiveGndFlag; }
            if (interimSend)  { s |= SendInterimPositions; }
            if (interimReceive) { s |= ReceiveInterimPositions; }
            if (visualSend)   { s |= SendVisualPositions; }
            this->setSendReceiveDetails(s);
        }

        void CFsdSetup::setForce3LetterAirlineCodes(bool force)
        {
            SendReceiveDetails d = this->getSendReceiveDetails();
            d.setFlag(Force3LetterAirlineICAO, force);
            this->setSendReceiveDetails(d);
        }

        const CFsdSetup &CFsdSetup::vatsimStandard()
        {
            static const CFsdSetup s(VATSIMDefault);
            return s;
        }

        CStatusMessageList CFsdSetup::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            CStatusMessageList msgs;
            if (this->getTextCodec().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"No codec")); }
            if (!textCodecNames(true, true).contains(this->getTextCodec())) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Unrecognized codec name")); }
            msgs.addCategories(cats);
            return msgs;
        }

        QVariant CFsdSetup::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexTextCodec: return QVariant::fromValue(m_textCodec);
            case IndexSendReceiveDetails: return QVariant::fromValue(m_sendReceive);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CFsdSetup::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CFsdSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexTextCodec: this->setTextCodec(variant.value<QString>()); break;
            case IndexSendReceiveDetails: this->setSendReceiveDetails(variant.value<SendReceiveDetails>()); break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CFsdSetup::comparePropertyByIndex(CPropertyIndexRef index, const CFsdSetup &compareValue) const
        {
            if (index.isMyself()) { return this->convertToQString(true).compare(compareValue.convertToQString()); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexTextCodec: return this->getTextCodec().compare(compareValue.getTextCodec());
            case IndexSendReceiveDetails: return Compare::compare(m_sendReceive, compareValue.m_sendReceive);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }
    } // namespace
} // namespace
