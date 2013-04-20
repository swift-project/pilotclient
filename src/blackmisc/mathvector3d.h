#ifndef BLACKMISC_MATHVECTOR3D_H
#define BLACKMISC_MATHVECTOR3D_H

#include "blackmisc/mathvector3dbase.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief Concrete vector implementation
 */
class CVector3D : public CVector3DBase<CVector3D>
{
public:
    /*!
     * \brief Default constructor
     */
    CVector3D() : CVector3DBase() {}

    /*!
     * \brief Constructor by value
     * \param i
     * \param j
     * \param k
     */
    CVector3D(double i, double j, double k) : CVector3DBase(i, j, k) {}

    /*!
     * \brief Constructor by value
     * \param value
     */
    CVector3D(double value) : CVector3DBase(value) {}

    /*!
     * \brief Copy constructor
     * \param otherVector
     */
    CVector3D(const CVector3D &otherVector) : CVector3DBase(otherVector) {}

    /*!
     * \brief i
     * \return
     */
    double i() const
    {
        return this->m_i;
    }

    /*!
     * \brief j
     * \return
     */
    double j() const
    {
        return this->m_j;
    }

    /*!
     * \brief k
     * \return
     */
    double k() const
    {
        return this->m_k;
    }

    /*!
     * \brief Set i
     * \param i
     */
    void setI(double i)
    {
        this->m_i = i;
    }

    /*!
     * \brief Set j
     * \param j
     */
    void setJ(double j)
    {
        this->m_j = j;
    }

    /*!
     * \brief Set k
     * \param k
     */
    void setK(double k)
    {
        this->m_k = k;
    }

};

} // namespace
} // namespace
#endif // guard
