/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MATHVECTOR3DBASE_H
#define BLACKMISC_MATHVECTOR3DBASE_H

#include "blackmisc/valueobject.h"
#include "blackmisc/mathematics.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Math
    {
        class CMatrix3x1;

        //! 3D vector base (x, y, z)
        template <class ImplVector> class CVector3DBase : public CValueObject
        {
        public:
            // getter and setters are implemented in the derived classes
            // as they have different names (x, i, north)

            //! Virtual destructor
            virtual ~CVector3DBase() {}

            //! Set zeros
            void setZero();

            //! Is zero?
            bool isZero() const
            {
                return this->m_i == 0 && this->m_j == 0 && this->m_k == 0;
            }

            //! Is zero, epsilon considered.
            bool isZeroEpsilon() const
            {
                ImplVector v;
                v += *this;
                v.round();
                return v.isZero();
            }

            //! Set all elements the same
            void fill(double value);

            //! Get element
            double getElement(int row) const;

            //! Set element
            void setElement(int row, double value);

            //! Operator []
            double operator[](int row) const { return this->getElement(row); }

            //! Operator [], mutable reference
            double &operator[](int row) { return this->getElement(row); }

            //! Equal operator ==
            bool operator ==(const CVector3DBase &other) const
            {
                if (this == &other) return true;
                return
                    CMath::epsilonEqual(this->m_i, other.m_i, 1E-9) &&
                    CMath::epsilonEqual(this->m_j, other.m_j, 1E-9) &&
                    CMath::epsilonEqual(this->m_k, other.m_k, 1E-9);
            }

            //! Unequal operator !=
            bool operator !=(const CVector3DBase &other) const
            {
                return !((*this) == other);
            }

            //! Operator +=
            CVector3DBase &operator +=(const CVector3DBase &other)
            {
                this->m_i += other.m_i;
                this->m_j += other.m_j;
                this->m_k += other.m_k;
                return *this;
            }

            //! Operator +
            ImplVector operator +(const ImplVector &other) const
            {
                ImplVector v = *derived();
                v += other;
                return v;
            }

            //! Operator -=
            CVector3DBase &operator -=(const CVector3DBase &other)
            {
                this->m_i -= other.m_i;
                this->m_j -= other.m_j;
                this->m_k -= other.m_k;
                return *this;
            }

            //! Operator -
            ImplVector operator -(const ImplVector &other) const
            {
                ImplVector v = *derived();
                v -= other;
                return v;
            }

            //! Multiply with scalar
            CVector3DBase &operator *=(double factor)
            {
                this->m_i *= factor;
                this->m_j *= factor;
                this->m_k *= factor;
                return *this;
            }

            //! Multiply with scalar
            ImplVector operator *(double factor) const
            {
                ImplVector v = *derived();
                v *= factor;
                return v;
            }

            //! Operator to support commutative multiplication
            friend ImplVector operator *(double factor, const ImplVector &other)
            {
                return other * factor;
            }

            //! Divide by scalar
            CVector3DBase &operator /=(double divisor)
            {
                this->m_i /= divisor;
                this->m_j /= divisor;
                this->m_k /= divisor;
                return *this;
            }

            //! Divide by scalar
            ImplVector operator /(double divisor) const
            {
                ImplVector v = *derived();
                v /= divisor;
                return v;
            }

            //! Dot product
            double dotProduct(const ImplVector &other) const;

            //! Cross product
            ImplVector crossProduct(const ImplVector &other) const;

            //! Reciprocal value
            ImplVector reciprocalValues() const
            {
                ImplVector v(1 / this->m_i, 1 / this->m_j, 1 / this->m_j);
                return v;
            }

            //! Converted to matrix
            CMatrix3x1 toMatrix3x1() const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! length / magnitude
            double length() const
            {
                return sqrt(this->m_i * this->m_i + this->m_j * this->m_j + this->m_k * this->m_k);
            }

            //! Round this vector
            void round()
            {
                const double epsilon = 1E-10;
                this->m_i = BlackMisc::Math::CMath::roundEpsilon(this->m_i, epsilon);
                this->m_j = BlackMisc::Math::CMath::roundEpsilon(this->m_j, epsilon);
                this->m_k = BlackMisc::Math::CMath::roundEpsilon(this->m_k, epsilon);
            }

            //! Rounded vector
            ImplVector rounded() const
            {
                ImplVector v = *derived();
                v.round();
                return v;
            }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*derived()); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant<ImplVector>(derived(), variant); }

            //! Register metadata
            static void registerMetadata();

        protected:
            // using own value since Qt QVector3D stores internally as float
            double m_i; //!< Vector data i
            double m_j; //!< Vector data j
            double m_k; //!< Vector data k

            //! Default constructor
            CVector3DBase() : m_i(0.0), m_j(0.0), m_k(0.0) {}

            //! Constructor by values
            CVector3DBase(double i, double j, double k) : m_i(i), m_j(j), m_k(k) {}

            //! Constructor by value
            explicit CVector3DBase(double value) : m_i(value), m_j(value), m_k(value) {}

            //! Get mutable element
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

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CVector3DBase)

            //! Easy access to derived class (CRTP template parameter)
            ImplVector const *derived() const
            {
                return static_cast<ImplVector const *>(this);
            }

            //! Easy access to derived class (CRTP template parameter)
            ImplVector *derived()
            {
                return static_cast<ImplVector *>(this);
            }

        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(BlackMisc::Math::CVector3DBase, (o.m_i, o.m_j, o.m_k))

#endif // guard
