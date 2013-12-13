#ifndef BLACKMISC_STATUSMESSAGES_H
#define BLACKMISC_STATUSMESSAGES_H

#include "valueobject.h"
#include "statusmessage.h"

namespace BlackMisc
{

    /*!
     * \brief Status messages, e.g. from Core -> GUI
     */
    class CStatusMessages : public CValueObject
    {
    public:

    private:
        QList<CStatusMessage> m_messages;

    public:
        /*!
         * \brief StatusMessage
         */
        CStatusMessages() {}

        /*!
         * \brief Find by type
         * \param type
         * \return
         */
        CStatusMessages findByType(CStatusMessage::StatusType type) const;

        /*!
         * \brief Find by type
         * \param type
         * \return
         */
        CStatusMessages findBySeverity(CStatusMessage::StatusSeverity severity) const;

        /*!
         * \brief Size
         * \return
         */
        int size() const;

        /*!
         * \brief Append
         * \param message
         */
        void append(const CStatusMessage &message);

        /*!
         * \brief Is empty
         * \return
         */
        bool isEmpty() const
        {
            return this->m_messages.isEmpty();
        }

        /*!
         * \brief Get messages
         * \return
         */
        const QList<CStatusMessage> &getMessages() const
        {
            return this->m_messages;
        }

        /*!
         * \brief Message at
         * \param index
         * \return
         */
        const CStatusMessage &at(int index) const
        {
            return this->m_messages.at(index);
        }

        /*!
         * \brief Value hash
         * \return
         */
        virtual uint getValueHash() const
        {
            return qHash(this);
        }

        /*!
         * \brief As QVariant
         * \return
         */
        virtual QVariant toQVariant() const
        {
            return QVariant::fromValue(*this);
        }

        /*!
         * \brief Register metadata of unit and quantity
         */
        static void registerMetadata();

    protected:
        /*!
         * \brief Marshall to DBus
         * \param argument
         */
        virtual void marshallToDbus(QDBusArgument &arg) const;

        /*!
         * \brief Unmarshall from DBus
         * \param argument
         */
        virtual void unmarshallFromDbus(const QDBusArgument &arg);

        /*!
         * \brief Convert to String
         * \param i18n
         * \return
         */
        virtual QString convertToQString(bool i18n = false) const;
    };

}

Q_DECLARE_METATYPE(BlackMisc::CStatusMessages)

#endif // guard
