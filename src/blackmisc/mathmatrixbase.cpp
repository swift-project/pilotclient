/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathmatrix3x3.h"
#include "blackmisc/mathmatrix3x1.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <typeinfo>

namespace BlackMisc
{
    namespace Math
    {

        /*
         * Get element by column / row
         */
        template<class ImplMatrix, int Rows, int Columns> double CMatrixBase<ImplMatrix, Rows, Columns>::getElement(int row, int column) const
        {
            this->checkRange(row, column);
            return this->m_matrix(row, column);
        }

        /*
         * Set element by column / row
         */
        template<class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::setElement(int row, int column, double value)
        {
            this->checkRange(row, column);
            this->m_matrix(row, column) = value;
        }

        /*
         * Check range
         */
        template<class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::checkRange(int row, int column) const
        {
            bool valid = (row < Rows && column < Columns);
            Q_ASSERT_X(valid, "getElement()", "Row or column invalid");
            if (!valid) throw std::range_error("Row or column invalid");
        }

        /*
         * All values zero?
         */
        template<class ImplMatrix, int Rows, int Columns> bool CMatrixBase<ImplMatrix, Rows, Columns>::isZero() const
        {
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Columns; c++)
                {
                    if (this->m_matrix(r, c) != 0) return false;
                }
            }
            return true;
        }


        /*
         * Set cell index
         */
        template<class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::setCellIndex()
        {
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Columns; c++)
                {
                    this->m_matrix(r, c) = r + 0.1 * c;
                }
            }
        }

        /*
         * To list
         */
        template<class ImplMatrix, int Rows, int Columns> QList<double> CMatrixBase<ImplMatrix, Rows, Columns>::toList() const
        {
            QList<double> list;
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Columns; c++)
                {
                    list.append(this->m_matrix(r, c));
                }
            }
            return list;
        }

        /*
         * From list
         */
        template<class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::fromList(const QList<double> &list)
        {
            Q_ASSERT_X(Rows * Columns == list.count(), "fromList()", "Mismatch of elements in list");
            int ct = 0;
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Columns; c++)
                {
                    this->m_matrix(r, c) = list.at(ct++);
                }
            }
        }

        /*
         * Round all values
         */
        template<class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::round()
        {
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Columns; c++)
                {
                    this->m_matrix(r, c) = CMath::roundEpsilon(this->m_matrix(r, c), 1E-10);
                }
            }
        }

        /*
         * Convert to string
         */
        template <class ImplMatrix, int Rows, int Columns> QString CMatrixBase<ImplMatrix, Rows, Columns>::convertToQString(bool /* i18n */) const
        {
            QString s = "{";
            for (int r = 0; r < Rows; r++)
            {
                s = s.append("{");
                for (int c = 0; c < Columns; c++)
                {
                    QString n = QString::number(this->m_matrix(r, c), 'f', 2);
                    if (c > 0) s = s.append(",");
                    s = s.append(n);
                }
                s = s.append("}");
            }
            s = s.append("}");
            return s;
        }

        /*
         * metaTypeId
         */
        template <class ImplMatrix, int Rows, int Columns> int CMatrixBase<ImplMatrix, Rows, Columns>::getMetaTypeId() const
        {
            return qMetaTypeId<ImplMatrix>();
        }

        /*
         * is a
         */
        template <class ImplMatrix, int Rows, int Columns> bool CMatrixBase<ImplMatrix, Rows, Columns>::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<ImplMatrix>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        template <class ImplMatrix, int Rows, int Columns> int CMatrixBase<ImplMatrix, Rows, Columns>::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CMatrixBase &>(otherBase);

            for (int r = 0; r < Rows; ++r)
            {
                for (int c = 0; c < Columns; ++c)
                {
                    if (this->m_matrix(r, c) < other.m_matrix(r, c)) { return -1; }
                    if (this->m_matrix(r, c) > other.m_matrix(r, c)) { return 1; }
                }
            }
            return 0;
        }

        /*
         * Hash
         */
        template <class ImplMatrix, int Rows, int Columns> uint CMatrixBase<ImplMatrix, Rows, Columns>::getValueHash() const
        {
            const QList<double> l = this->toList();
            QList<uint> hashs;

            // there is an issue with the signature of QList, so I use
            // individual values
            foreach(double v, l)
            {
                hashs << qHash(static_cast<long>(v));
            }
            return BlackMisc::calculateHash(hashs, "CMatrixBase");
        }

        /*
         * To DBus
         */
        template <class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::marshallToDbus(QDBusArgument &argument) const
        {
            const QList<double> l = this->toList();

            // there is an issue with the signature of QList, so I use
            // individual values
            foreach(double v, l)
            {
                argument << v;
            }
        }

        /*
         * From DBus
         */
        template <class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            QList<double> list;
            double v;
            while (!argument.atEnd())
            {
                argument >> v;
                list.append(v);
            }
            this->fromList(list);
        }

        /*
             * Register metadata
             */
        template <class ImplMatrix, int Rows, int Columns> void CMatrixBase<ImplMatrix, Rows, Columns>::registerMetadata()
        {
            qRegisterMetaType<ImplMatrix>();
            qDBusRegisterMetaType<ImplMatrix>();
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CMatrixBase<CMatrix3x3, 3, 3>;
        template class CMatrixBase<CMatrix3x1, 3, 1>;
        template class CMatrixBase<CMatrix1x3, 1, 3>;

    } // namespace
} // namespace
