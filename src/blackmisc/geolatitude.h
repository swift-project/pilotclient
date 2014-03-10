#ifndef BLACKMISC_GEOLATITUDE_H
#define BLACKMISC_GEOLATITUDE_H

#include <QtCore/qmath.h>
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
    namespace Geo
    {

        /*!
         * \brief Latitude
         */
        class CLatitude : public CEarthAngle<CLatitude>
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
                    s.append(this->isNegativeWithEpsilonConsidered() ? " S" : " N");
                return s;
            }

        public:
            /*!
             * \brief Default constructor
             */
            CLatitude() : CEarthAngle() {}

            /*!
             * \brief Constructor
             * \param angle
             */
            explicit CLatitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

            /*!
             * \brief Init by double value
             * \param value
             * \param unit
             */
            CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}

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
            virtual ~CLatitude() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLatitude)

#endif // guard
