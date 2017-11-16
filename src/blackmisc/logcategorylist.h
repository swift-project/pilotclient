/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOGCATEGORYLIST_H
#define BLACKMISC_LOGCATEGORYLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/sequence.h"
#include "blackmisc/typetraits.h"

#include <QJsonObject>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QThreadStorage>
#include <QtGlobal>
#include <initializer_list>
#include <tuple>
#include <type_traits>

namespace BlackMisc
{
    /*!
     * A sequence of log categories.
     */
    class BLACKMISC_EXPORT CLogCategoryList : public CSequence<CLogCategory>
    {
    public:
        //! Empty constructor.
        CLogCategoryList() = default;

        //! By single element
        CLogCategoryList(const CLogCategory &category) { this->push_back(category); }

        //! Copy constructor.
        CLogCategoryList(const CLogCategoryList &) = default;

        //! Copy construct from superclass instance.
        CLogCategoryList(const CSequence<CLogCategory> &other) : CSequence<CLogCategory>(other) {}

        //! Initializer list constructor.
        CLogCategoryList(std::initializer_list<CLogCategory> il) : CSequence<CLogCategory>(il) {}

        //! Prevent accidental use of the initializer list constructor.
        CLogCategoryList(std::nullptr_t) = delete;

        //! Copy assignment.
        CLogCategoryList &operator =(const CLogCategoryList &) = default;

        //! Copy assign from superclass instance.
        CLogCategoryList &operator =(const CSequence<CLogCategory> &other) { CSequence<CLogCategory>::operator =(other); return *this; }

        /*!
         * Construct by extracting categories from a class T.
         *
         * If T has a member function getLogCategories, then this will be called.
         * If T inherits from QObject then we get the name of the class and all its superclasses.
         * If T is registered with QMetaType then we get the name of the class.
         * If more than one of the above cases is true, then all are combined into the list.
         * If none of the above cases is true, then the constructed list will be the uncategorized category.
         *
         * This constructor will be disabled if T is not a class type.
         *
         * \param pointer The value of pointer is unimportant. Only the static type T is considered.
         *                It is legal to pass static_cast<T>(nullptr), but in member functions passing the <tt>this</tt> pointer is easier.
         */
        template <typename T, typename = std::enable_if_t<std::is_class<T>::value>>
        CLogCategoryList(const T *pointer) : CLogCategoryList(fromClass<T>()) { Q_UNUSED(pointer); }

        //! Convert each of the categories to a QString and return the result as a QStringList.
        QStringList toQStringList() const;

        //! \copydoc BlackMisc::CValueObject::toQString
        QString toQString(bool i18n = false) const;

        //! Convert a string list, such as that returned by toQStringList(), into a CLogCategoryList.
        static CLogCategoryList fromQStringList(const QStringList &stringList);

        //! Convert a string, such as that returned by toQString(), into a CLogCategoryList.
        static CLogCategoryList fromQString(const QString &string);

        //! Returns true if any of the categories in the list start with the given prefix.
        bool anyStartWith(const QString &prefix) const;

        //! Returns true if any of the categories in the list end with the given suffix.
        bool anyEndWith(const QString &suffix) const;

        //! Register metadata
        static void registerMetadata();

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        /*
         * Templates used by the constructor template:
         */
        template <typename T>
        struct tag {};

        template <typename T>
        static const CLogCategoryList &fromClass()
        {
            static_assert(sizeof(T) > 0, "T must be a complete type, not forward declared");
            static QThreadStorage<CLogCategoryList> list;
            if (! list.hasLocalData())
            {
                list.localData().appendCategoriesFromMemberFunction(tag<T>(), THasGetLogCategories<T>());
                list.localData().appendCategoriesFromMetaType(tag<T>(), std::integral_constant<bool, QMetaTypeId<T>::Defined>());
                list.localData().appendCategoriesFromMetaObject(tag<T>(), std::is_base_of<QObject, T>());
                if (list.localData().isEmpty()) { list.localData().push_back(CLogCategory::uncategorized()); }
            }
            return list.localData();
        }

        template <typename T>
        void appendCategoriesFromMemberFunction(tag<T>, std::true_type) { push_back(T::getLogCategories()); }
        void appendCategoriesFromMemberFunction(...) {}

        template <typename T>
        void appendCategoriesFromMetaType(tag<T>, std::true_type) { push_back(QMetaType::typeName(qMetaTypeId<T>())); }
        void appendCategoriesFromMetaType(...) {}

        template <typename T>
        void appendCategoriesFromMetaObject(tag<T>, std::true_type) { appendCategoriesFromMetaObject(T::staticMetaObject); }
        void appendCategoriesFromMetaObject(...) {}

        void appendCategoriesFromMetaObject(const QMetaObject &);
    };
}

Q_DECLARE_METATYPE(BlackMisc::CLogCategoryList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CLogCategory>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CLogCategory>)

#endif
