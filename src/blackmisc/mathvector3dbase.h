/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHVECTOR3DBASE_H
#define BLACKMISC_MATHVECTOR3DBASE_H

#include "blackmisc/valueobject.h"
#include "blackmisc/mathematics.h"
using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Math
    {
        class CMatrix3x1;

        /*!
         * \brief 3D vector base (x, y, z)
         */
        template <class ImplVector> class CVector3DBase : public CValueObject
        {
        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CVector3DBase)

            //! \brief Easy access to derived class (CRTP template parameter)
            ImplVector const *derived() const
            {
                return static_cast<ImplVector const *>(this);
            }

            //! \brief Easy access to derived class (CRTP template parameter)
            ImplVector *derived()
            {
                return static_cast<ImplVector *>(this);
            }

        protected:
            // using own value since Qt QVector3D stores internally as float
            double m_i; //!< Vector data i
            double m_j; //!< Vector data j
            double m_k; //!< Vector data k

            //! \brief Default constructor
            CVector3DBase() : m_i(0.0), m_j(0.0), m_k(0.0) {}

            //! \brief Constructor by values
            CVector3DBase(double i, double j, double k) : m_i(i), m_j(j), m_k(k) {}

            //! \brief Constructor by value
            explicit CVector3DBase(double value) : m_i(value), m_j(value), m_k(value) {}

            /*!
             * \brief Get element
             * \param row
             * \return Mutable reference
             */
            double &getElement(int row);

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

        public:
            // getter and setters are implemented in the derived classes
            // as they have different names (x, i, north)

            //! \brief Virtual destructor
            virtual ~CVector3DBase() {}

            //! \brief Set zeros
            void setZero();

            //! \brief Is zero?
            bool isZero() const
            {
                return this->m_i == 0 && this->m_j == 0 && this->m_k == 0;
            }

            //! \brief Is zero, epsilon considered.
            bool isZeroEpsilon() const
            {
                ImplVector v;
                v += *this;
                v.round();
                return v.isZero();
            }

            //! \brief Set all elements the same
            void fill(double value);

            //! \brief Get element
            double getElement(int row) const;

            //! \brief Set element
            void setElement(int row, double value);

            //! \brief Operator []
            double operator[](int row) const { return this->getElement(row); }

            //! \brief Operator [], mutable reference
            double &operator[](int row) { return this->getElement(row); }

            //! \brief Equal operator ==
            bool operator ==(const CVector3DBase &other) const
            {
                if (this == &other) return true;
                return
                    CMath::epsilonEqual(this->m_i, other.m_i, 1E-9) &&
                    CMath::epsilonEqual(this->m_j, other.m_j, 1E-9) &&
                    CMath::epsilonEqual(this->m_k, other.m_k, 1E-9);
            }

            //! \brief Unequal operator !=
            bool operator !=(const CVector3DBase &other) const
            {
                return !((*this) == other);
            }

            //! \brief Operator +=
            CVector3DBase &operator +=(const CVector3DBase &other)
            {
                this->m_i += other.m_i;
                this->m_j += other.m_j;
                this->m_k += other.m_k;
                return *this;
            }

            //! \brief Operator +
            ImplVector operator +(const ImplVector &other) const
            {
                ImplVector v = *derived();
                v += other;
                return v;
            }

            //! \brief Operator -=
            CVector3DBase &operator -=(const CVector3DBase &other)
            {
                this->m_i -= other.m_i;
                this->m_j -= other.m_j;
                this->m_k -= other.m_k;
                return *this;
            }

            //! \brief Operator -
            ImplVector operator -(const ImplVector &other) const
            {
                ImplVector v = *derived();
                v -= other;
                return v;
            }

            //! \brief Multiply with scalar
            CVector3DBase &operator *=(double factor)
            {
                this->m_i *= factor;
                this->m_j *= factor;
                this->m_k *= factor;
                return *this;
            }

            //! \brief Multiply with scalar
            ImplVector operator *(double factor) const
            {
                ImplVector v = *derived();
                v *= factor;
                return v;
            }

            //! \brief Operator to support commutative multiplication
            friend ImplVector operator *(double factor, const ImplVector &other)
            {
                return other * factor;
            }

            //! \brief Divide by scalar
            CVector3DBase &operator /=(double divisor)
            {
                this->m_i /= divisor;
                this->m_j /= divisor;
                this->m_k /= divisor;
                return *this;
            }

            //! \brief Divide by scalar
            ImplVector operator /(double divisor) const
            {
                ImplVector v = *derived();
                v /= divisor;
                return v;
            }

            //! \brief Dot product
            double dotProduct(const ImplVector &other) const;

            //! \brief Cross product
            ImplVector crossProduct(const ImplVector &other) const;

            //! \brief Reciprocal value
            ImplVector reciprocalValues() const
            {
                ImplVector v(1 / this->m_i, 1 / this->m_j, 1 / this->m_j);
                return v;
            }

            //! \brief Converted to matrix
            CMatrix3x1 toMatrix3x1() const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \brief length / magnitude
            double length() const
            {
                return sqrt(this->m_i * this->m_i + this->m_j * this->m_j + this->m_k * this->m_k);
            }

            //! \brief Round this vector
            void round()
            {
                const double epsilon = 1E-10;
                this->m_i = BlackMisc::Math::CMath::roundEpsilon(this->m_i, epsilon);
                this->m_j = BlackMisc::Math::CMath::roundEpsilon(this->m_j, epsilon);
                this->m_k = BlackMisc::Math::CMath::roundEpsilon(this->m_k, epsilon);
            }

            //! \brief Rounded vector
            ImplVector rounded() const
            {
                ImplVector v = *derived();
                v.round();
                return v;
            }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \brief Register metadata
            static void registerMetadata();
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(BlackMisc::Math::CVector3DBase, (o.m_i, o.m_j, o.m_k))

#endif // guard
