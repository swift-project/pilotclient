#ifndef BLACKMISC_AIRCRAFTMODEL_H
#define BLACKMISC_AIRCRAFTMODEL_H

#include "nwuser.h"
#include "valueobject.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Another pilot's aircraft model
         */
        class CAircraftModel : public BlackMisc::CValueObject
        {

        public:

            //! Indexes
            enum ColumnIndex : uint
            {
                IndexModelString = 100,
                IndexQueriedModelString
            };

            //! \brief Default constructor.
            CAircraftModel() {}

            //! \brief Constructor.
            CAircraftModel(const QString &model, bool queriedString) : m_modelString(model), m_queriedModelStringFlag(queriedString) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \brief Equal operator ==
            bool operator ==(const CAircraftModel &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAircraftModel &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::propertyByIndex(int)
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(const QVariant, int)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

            //! Queried model string
            const QString &getModelString() const { return this->m_modelString; }

            //! Set queried model string
            void setQueriedModelString(const QString &model) { this->m_modelString = model; }

            //! Queried model string?
            bool hasQueriedModelString() const { return this->m_queriedModelStringFlag && !this->m_modelString.isEmpty(); }

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! \brief Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

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
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftModel)
            QString m_modelString;
            bool m_queriedModelStringFlag; //!< model string is queried from network
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAircraftModel, (o.m_modelString, o.m_queriedModelStringFlag))
Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftModel)

#endif // guard
