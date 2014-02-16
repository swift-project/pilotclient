#ifndef BLACKMISC_GEOLONGITUDE_H
#define BLACKMISC_GEOLONGITUDE_H
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
    namespace Geo
    {

        /*!
         * \brief Longitude
         */
        class CLongitude : public CEarthAngle<CLongitude>
        {
        protected:
            /*!
             * \brief Specific string representation
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const
            {
                QString s(CEarthAngle::convertToQString(i18n));
                if (!this->isZeroEpsilonConsidered())
                    s.append(this->isNegativeWithEpsilonConsidered() ? " W" : " E");
                return s;
            }

        public:
            /*!
             * \brief Default constructor
             */
            CLongitude() : CEarthAngle() {}

            /*!
             * \brief Constructor
             * \param angle
             */
            explicit CLongitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

            /*!
             * \brief Init by double value
             * \param value
             * \param unit
             */
            CLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Virtual destructor
             */
            virtual ~CLongitude() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLongitude)

#endif // guard
