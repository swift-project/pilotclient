/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/nwvoicecapabilities.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {

        /*
         * Constructor
         */
        CVoiceCapabilities::CVoiceCapabilities(const QString &flightPlanRemarks)
        {
            this->fromFlightPlanRemarks(flightPlanRemarks);
        }

        /*
         * Convert to string
         */
        QString CVoiceCapabilities::convertToQString(bool /** i18n **/) const
        {
            switch (this->m_voiceCapabilities)
            {
            case Voice:
                {
                    static const QString v("voice");
                    return v;
                }
            case TextOnly:
                {
                    static const QString t("text only");
                    return t;
                }
            case VoiceReceivingOnly:
                {
                    static const QString r("voice listening only");
                    return r;
                }
            default:
            case Unknown:
                {
                    static const QString u("unknown");
                    return u;
                }
            }
            Q_ASSERT("Wrong index");
            return QString(""); // never reached
        }

        void CVoiceCapabilities::fromFlightPlanRemarks(const QString &flightPlanRemarks)
        {
            if (flightPlanRemarks.isEmpty())
            {
                this->m_voiceCapabilities = Unknown;
                return;
            }

            QString r = flightPlanRemarks.toLower();
            if (r.contains("/v/") || r.contains("/voice/"))
            {
                this->setCapabilities(Voice);
                return;
            }
        }

        /*
         * metaTypeId
         */
        int CVoiceCapabilities::getMetaTypeId() const
        {
            return qMetaTypeId<CVoiceCapabilities>();
        }

        /*
         * is a
         */
        bool CVoiceCapabilities::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CVoiceCapabilities>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CVoiceCapabilities::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CVoiceCapabilities &>(otherBase);
            return compare(TupleConverter<CVoiceCapabilities>::toTuple(*this), TupleConverter<CVoiceCapabilities>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CVoiceCapabilities::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CVoiceCapabilities>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CVoiceCapabilities::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CVoiceCapabilities>::toTuple(*this);
        }

        /*
         * Equal?
         */
        const QPixmap &CVoiceCapabilities::toIcon() const
        {
            static const QPixmap v(QPixmap(":/blackmisc/icons/capvoice.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap t(QPixmap(":/blackmisc/icons/captextonly.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap u(QPixmap(":/blackmisc/icons/capunknown.png").scaledToWidth(16, Qt::SmoothTransformation));
            static const QPixmap r(QPixmap(":/blackmisc/icons/capvoicereceive.png").scaledToWidth(16, Qt::SmoothTransformation));

            switch (this->m_voiceCapabilities)
            {
            case Voice:
                return v;
            case TextOnly:
                return t;
            case Unknown:
                return u;
            case VoiceReceivingOnly:
                return r;
            }
            Q_ASSERT("Wrong index");
            return u; // never reached
        }

        bool CVoiceCapabilities::operator ==(const CVoiceCapabilities &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CVoiceCapabilities>::toTuple(*this) == TupleConverter<CVoiceCapabilities>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CVoiceCapabilities::operator !=(const CVoiceCapabilities &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CVoiceCapabilities::getValueHash() const
        {
            return qHash(TupleConverter<CVoiceCapabilities>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CVoiceCapabilities::toJson() const
        {
            return BlackMisc::serializeJson(CVoiceCapabilities::jsonMembers(), TupleConverter<CVoiceCapabilities>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CVoiceCapabilities::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CVoiceCapabilities::jsonMembers(), TupleConverter<CVoiceCapabilities>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CVoiceCapabilities::jsonMembers()
        {
            return TupleConverter<CVoiceCapabilities>::jsonMembers();
        }

        /*
         * Register metadata
         */
        void CVoiceCapabilities::registerMetadata()
        {
            qRegisterMetaType<CVoiceCapabilities>();
            qDBusRegisterMetaType<CVoiceCapabilities>();
        }

    } // namespace
} // namespace
