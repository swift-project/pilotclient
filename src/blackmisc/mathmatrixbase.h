/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHMATRIXBASE_H
#define BLACKMISC_MATHMATRIXBASE_H

#include "blackmisc/valueobject.h"
#include "blackmisc/mathvector3dbase.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QGenericMatrix>

namespace BlackMisc
{
    namespace Math
    {

        /*!
         * \brief Base functionality of a matrix
         */
        template<class ImplMatrix, int Rows, int Columns> class CMatrixBase : public BlackMisc::CValueObject
        {
        private:
            /*!
             * \brief Easy access to derived class (CRTP template parameter)
             * \return
             */
            ImplMatrix const *derived() const
            {
                return static_cast<ImplMatrix const *>(this);
            }

            /*!
             * \brief Easy access to derived class (CRTP template parameter)
             * \return
             */
            ImplMatrix *derived()
            {
                return static_cast<ImplMatrix *>(this);
            }

        protected:
            // no bug, Qt expects columns rows
            QGenericMatrix<Columns, Rows, double> m_matrix; //!< backing data

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

        public:
            //! \brief Default constructor
            CMatrixBase() : m_matrix() {}

            /*!
             * \brief Fill with value
             * \param fillValue
             */
            explicit CMatrixBase(double fillValue) : m_matrix()
            {
                this->fill(fillValue);
            }

            //! \brief Virtual destructor
            virtual ~CMatrixBase() {}

            //! \brief List of values
            QList<double> toList() const;

            //! \brief List of values
            void fromList(const QList<double> &list);

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*derived()); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant<ImplMatrix>(derived(), variant); }

            //! \brief Equal operator ==
            bool operator ==(const ImplMatrix &other) const
            {
                if (this == &other) return true;
                return this->m_matrix == other.m_matrix;
            }

            //! \brief Unequal operator !=
            bool operator !=(const ImplMatrix &other) const
            {
                return !((*this) == other);
            }

            //! \brief Operator *=
            CMatrixBase &operator *=(double factor)
            {
                this->m_matrix *= factor;
                return *this;
            }

            //! \brief Operator *
            ImplMatrix operator *(double factor) const
            {
                ImplMatrix m = *derived();
                m *= factor;
                return m;
            }

            //! \brief Operator to support commutative scalar multiplication
            friend ImplMatrix operator *(double factor, const ImplMatrix &other)
            {
                return other * factor;
            }

            //! \brief Operator /=
            CMatrixBase &operator /=(double factor)
            {
                this->m_matrix /= factor;
                return *this;
            }

            //! \brief Operator /
            ImplMatrix operator /(double factor) const
            {
                ImplMatrix m = *derived();
                m /= factor;
                return m;
            }

            //! \brief Operator +=
            CMatrixBase &operator +=(const CMatrixBase &other)
            {
                this->m_matrix += other.m_matrix;
                return *this;
            }

            //! \brief Operator +
            ImplMatrix operator +(const ImplMatrix &other) const
            {
                ImplMatrix m = *derived();
                m += other;
                return m;
            }

            //! \brief Operator -=
            CMatrixBase &operator -=(const CMatrixBase &other)
            {
                this->m_matrix -= other.m_matrix;
                return *this;
            }

            //! \brief Operator -
            ImplMatrix operator -(const ImplMatrix &other) const
            {
                ImplMatrix m = *derived();
                m -= other;
                return m;
            }

            //! \brief Is identity matrix?
            bool isIdentity() const
            {
                return this->m_matrix.isIdentity();
            }

            //! \brief Is identity matrix? Epsilon considered.
            bool isIdentityEpsilon() const
            {
                ImplMatrix m = *derived();
                m.round();
                return m.isIdentity();
            }

            //! \brief Set as identity matrix
            void setToIdentity()
            {
                this->m_matrix.setToIdentity();
            }

            //! \brief All values to zero
            void setZero() { this->m_matrix.fill(0.0); }

            //! \brief Is zero?
            bool isZero() const;

            //! \brief Is zero matrix? Epsilon considered.
            bool isZeroEpsilon() const
            {
                ImplMatrix m = *derived();
                m.round();
                return m.isZero();
            }

            //! \brief Each cell gets a unique index (used primarily for testing)
            void setCellIndex();

            //! \brief Set all elements the same
            void fill(double value) { this->m_matrix.fill(value); }

            //! \brief Round all values
            void round();

            //! \brief Return a rounded matrix
            ImplMatrix roundedMatrix() const
            {
                ImplMatrix m = *derived();
                m.round();
                return m;
            }

            //! \brief Get element
            double getElement(int row, int column) const;

            //! \brief Set element
            void setElement(int row, int column, double value);

            //! \brief Get element by operator () modifying
            double &operator()(int row, int column)
            {
                this->checkRange(row, column);
                return this->m_matrix(row, column);
            }

            //! \brief Get element by operator () read only
            double operator()(int row, int column) const
            {
                return this->getElement(row, column);
            }

            //! \brief Register metadata
            static void registerMetadata();

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

        private:
            //! \brief Check range of row / column
            void checkRange(int row, int column) const;
        };

    } // namespace
} // namespace

#endif // guard
