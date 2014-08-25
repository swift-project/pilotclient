#ifndef BLACKSIM_SIMULATORINFO_H
#define BLACKSIM_SIMULATORINFO_H

#include "blackmisc/indexvariantmap.h"
#include "blackmisc/valueobject.h"

namespace BlackSim
{
    //! Describing a simulator
    class CSimulatorInfo : public BlackMisc::CValueObject
    {
    public:
        //! Default constructor
        CSimulatorInfo();

        //! Constructor
        CSimulatorInfo(const QString &shortname, const QString &fullname);

        //! Unspecified simulator
        bool isUnspecified() const { return this->m_shortName.isEmpty() || this->m_shortName.startsWith("Unspecified", Qt::CaseInsensitive); }

        //! Equal operator ==
        bool operator ==(const CSimulatorInfo &other) const;

        //! Unequal operator !=
        bool operator !=(const CSimulatorInfo &other) const;

        //! Single setting value
        QVariant getSimulatorSetupValue(int index) const;

        //! Single setting value
        QString getSimulatorSetupValueAsString(int index) const;

        //! Set single settings
        void setSimulatorSetup(const BlackMisc::CIndexVariantMap &setup);

        //! Short name
        const QString &getShortName() const { return m_shortName; }

        //! Short name
        const QString &getFullName() const { return m_fullName; }

        //! Compare on names only, ignore setup
        bool isSameSimulator(const CSimulatorInfo &otherSimulator) const;

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toQVariant()
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::convertFromJson
        virtual void convertFromJson(const QJsonObject &json) override;

        //! JSON member names
        static const QStringList &jsonMembers();

        //! Register the metatypes
        static void registerMetadata();

        //! Simulator is FS9 - Microsoft Flight Simulator 2004
        static const CSimulatorInfo &FS9()
        {
            static CSimulatorInfo sim("FS9", "Microsoft Flight Simulator 2004");
            return sim;
        }

        //! Simulator is FSX Microsoft Flight Simulator X (2006)
        static const CSimulatorInfo &FSX()
        {
            static CSimulatorInfo sim("FSX", "Microsoft Flight Simulator X (2006)");
            return sim;
        }

        //! Simulator is XPlane, unspecified version
        static const CSimulatorInfo &XP()
        {
            static CSimulatorInfo sim("XP", "X-Plane");
            return sim;
        }

        //! Simulator is unspecified
        static const CSimulatorInfo &UnspecifiedSim()
        {
            static CSimulatorInfo sim("Unspecified", "Unspecified");
            return sim;
        }

    protected:
        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &otherBase) const override;

        //! \copydoc CValueObject::marshallToDbus()
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CSimulatorInfo)
        QString m_fullName;
        QString m_shortName;
        BlackMisc::CIndexVariantMap m_simsetup; //!< allows to access simulator keys requried on remote side
    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::CSimulatorInfo, (o.m_fullName, o.m_shortName, o.m_simsetup))
Q_DECLARE_METATYPE(BlackSim::CSimulatorInfo)

#endif // guard
