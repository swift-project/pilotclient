/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUECACHE_PRIVATE_H
#define BLACKMISC_VALUECACHE_PRIVATE_H

#include "blackmisc/variantmap.h"
#include "blackmisc/statusmessage.h"
#include <QObject>
#include <QMutex>
#include <QMap>

namespace BlackMisc
{
    class CIdentifier;
    class CValueCache;
    class CValueCachePacket;

    namespace Private
    {

        /*!
         * \private Trait for obtaining the class type of a pointer to member type, or QObject if T is not a pointer to member.
         */
        template <typename T>
        struct TClassOfPointerToMember
        {
            using type = QObject;
        };
        //! \cond
        template <typename T, typename M>
        struct TClassOfPointerToMember<M T::*>
        {
            using type = T;
        };
        //! \endcond

        /*!
         * \private Destroy an object and reconstruct it with the given constructor arguments.
         */
        template <typename T, typename... Args>
        void reconstruct(T *object, Args &&...args)
        {
            object->~T();
            new (object) T(std::forward<Args>(args)...);
        }

        /*!
         * \private QObject subclass used by CCached<T> class template for signal/slot communication with CValueCache.
         * An instance of this class is shared between all CCached<T> referring to the same CValueCache and owned by the same QObject,
         * with the latter QObject becoming parent of this instance.
         */
        class BLACKMISC_EXPORT CValuePage : public QObject
        {
            Q_OBJECT

        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Returns a CValuePage owned by the parent and connected to the cache, creating one if it doesn't exist.
            static CValuePage &getPageFor(QObject *parent, CValueCache *cache);

            //! Opaque type holding the state of one value in the page.
            struct Element;

            //! Functor used for validating values.
            using Validator = std::function<bool(const CVariant &, QString &)>;

            //! Functor used to notify parent of changes.
            using NotifySlot = std::pair<std::function<void(QObject *)>, void (QObject::*)()>;

            //! Returns a new instance of the opaque Element type for use by CCached<T> to interact with CValuePage.
            //! \param key The key string of the value in the cache.
            //! \param name Human-readable name corresponding to the key.
            //! \param metaType The Qt metatype ID of the value object's expected type.
            //! \param validator Optional functor which returns true if the value is valid.
            //! \param defaultValue Optional value which is used in case the value is invalid.
            Element &createElement(const QString &key, const QString &name, int metaType, const Validator &validator, const CVariant &defaultValue);

            //! Set the functor to call to notify that the value corresponding to the element's key was modified.
            void setNotifySlot(Element &element, const NotifySlot &slot);

            //! True if the currently paged value corresponds to a valid key.
            bool isInitialized(const Element &element) const;

            //! True if the currently paged value is a valid instance of the given type.
            //! \threadsafe
            bool isValid(const Element &element, int typeId) const;

            //! Read the currently paged value corresponding to the element's key.
            const CVariant &getValue(const Element &element) const;

            //! Read the currently paged value corresponding to the element's key.
            //! \threadsafe
            CVariant getValueCopy(const Element &element) const;

            //! Write the value corresponding to the element's key and begin synchronizing it to any other pages.
            CStatusMessage setValue(Element &element, CVariant value, qint64 timestamp, bool save = false);

            //! Get the key string corresponding to the element.
            const QString &getKey(const Element &element) const;

            //! Get the timestamp corresponding to the element.
            qint64 getTimestamp(const Element &element) const;

            //! Get whether this element is already saved to disk.
            bool isSaved(const Element &element) const;

            //! Get whether this element is currently being saved to disk.
            bool isSaving(const Element &element) const;

            //! Synchronize with a change caused by another page.
            //! Connected to signal CValueCache::valuesChanged.
            //! \param values The new values.
            //! \param changedBy Pointer to the CValuePage which caused the change. Null if it was changed by another process.
            void setValuesFromCache(const BlackMisc::CValueCachePacket &values, QObject *changedBy);

            //! Put this page into batching mode.
            void beginBatch();

            //! Take this page out of batching mode and discard changes.
            void abandonBatch();

            //! Take this page out of batching mode and apply changes.
            void endBatch();

        signals:
            //! Synchronize this page's changes to other pages.
            //! Connected to slot CValueCache::changeValues.
            void valuesWantToCache(const BlackMisc::CValueCachePacket &values);

        private:
            using ElementPtr = QSharedPointer<Element>; // QMap doesn't support move-only types

            CValueCache *m_cache = nullptr;
            QMap<QString, ElementPtr> m_elements;
            int m_batchMode = 0;
            CVariantMap m_batchedValues;

            CValuePage(QObject *parent, CValueCache *cache);
            CStatusMessage validate(const Element &element, const CVariant &value, CStatusMessage::StatusSeverity invalidSeverity) const;
        };

    } // namespace

} // namespace

#endif
