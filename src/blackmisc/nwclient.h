#ifndef BLACKMISC_CLIENT_H
#define BLACKMISC_CLIENT_H

#include "nwuser.h"
#include "nwaircraftmodel.h"
#include "nwvoicecapabilities.h"
#include "valueobject.h"
#include "indexvariantmap.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Another client software.
         */
        class CClient : public BlackMisc::CValueObject
        {

        public:
            /*!
             * \brief Properties by index
             */
            enum ColumnIndex : uint
            {
                // user
                IndexEmail = 0,
                IndexId,
                IndexPassword,
                IndexRealName,
                IndexCallsign,
                IndexCallsignIcon,
                // model
                IndexQueriedModelString = 100,
                // own indexes
                IndexCapabilities = 1000,
                IndexCapabilitiesString,
                IndexModel,
                IndexHost,
                IndexVoiceCapabilities,
                IndexVoiceCapabilitiesString,
                IndexVoiceCapabilitiesIcon
            };

            /*!
             * \brief The Capabilities enum
             */
            enum Capabilities
            {
                FsdWithInterimPositions,
                FsdWithModelDescription,
                FsdAtisCanBeReceived
            };

            //! Default constructor.
            CClient() {}

            //! Construct by callsign
            CClient(const BlackMisc::Aviation::CCallsign &callsign) : m_user(CUser(callsign)) {}

            //! Constructor.
            CClient(const CUser &user) : m_user(user) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Equal operator ==
            bool operator ==(const CClient &other) const;

            //! Unequal operator !=
            bool operator !=(const CClient &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! Callsign used with other client
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_user.getCallsign(); }

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! Get capabilities
            CIndexVariantMap getCapabilities() const { return this->m_capabilities; }

            //! Set capability
            void setCapability(bool hasCapability, Capabilities capability);

            //! Set capabilities
            void setCapabilities(const CIndexVariantMap &capabilities);

            //! Get capabilities
            QString getCapabilitiesAsString() const;

            //! Has capability?
            bool hasCapability(Capabilities capability) const;

            //! Get voice capabilities
            const CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities;}

            //! Set voice capabilities
            void setVoiceCapabilities(const CVoiceCapabilities &voiceCapabilities) { m_voiceCapabilities = voiceCapabilities;}

            //! Set voice capabilities
            void setVoiceCapabilities(const QString &flightPlanRemarks) { m_voiceCapabilities = CVoiceCapabilities(flightPlanRemarks);}

            //! Host
            const QString &getHost() const { return this->m_host; }

            //! Host
            void setHost(const QString &host) { this->m_host = host;}

            //! Model
            const CAircraftModel &getAircraftModel() const { return this->m_model; }

            //! Set model
            void setAircraftModel(const CAircraftModel &model) { this->m_model = model; }

            //! \copydoc CValueObject::toIcon()
            virtual const QPixmap &toIcon() const override { return this->m_user.toIcon(); }

            //! Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

            //! \copydoc CValueObject::propertyByIndex(int)
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(const QVariant, int)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CClient)
            CUser m_user;
            CAircraftModel m_model;
            CIndexVariantMap m_capabilities;
            QString m_host;
            CVoiceCapabilities m_voiceCapabilities;

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CClient, (o.m_user, o.m_model, o.m_capabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CClient)

#endif // guard
