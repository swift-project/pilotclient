#ifndef BLACKGUI_TRANSPONDERMODESELECTOR_H
#define BLACKGUI_TRANSPONDERMODESELECTOR_H

#include "blackmisc/aviotransponder.h"
#include "blackmisc/aviotransponder.h"
#include <QTimer>
#include <QComboBox>
#include <QStringList>


namespace BlackGui
{

    /*!
     * \brief Selector for the transponder mode
     */
    class CTransponderModeSelector : public QComboBox
    {
        Q_OBJECT

    private:
        BlackMisc::Aviation::CTransponder::TransponderMode m_currentMode;
        BlackMisc::Aviation::CTransponder::TransponderMode m_resetMode;
        QTimer m_resetTimer;

    public:
        //! \brief Constructor
        explicit CTransponderModeSelector(QWidget *parent = nullptr);

        //! \brief Standby string
        static const QString &transponderStateStandby();

        //! \brief Ident string
        static const QString &transponderStateIdent();

        //! \brief Mode C string
        static const QString &transponderModeC();

        //! \brief All relevant modes for GUI
        static const QStringList &modes();

        //! \brief Selected transponder mode
        BlackMisc::Aviation::CTransponder::TransponderMode getSelectedTransponderMode() const;

        //! \brief Selected transponder mode
        void setSelectedTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode);

        //!\ brief Set to ident
        void setSelectedTransponderModeStateIdent()
        {
            this->setSelectedTransponderMode(BlackMisc::Aviation::CTransponder::StateIdent);
        }

        //! \brief Ident selected
        bool isIdentSelected() const
        {
            return this->getSelectedTransponderMode() == BlackMisc::Aviation::CTransponder::StateIdent;
        }

    public slots:
        //! \brief reset to last mode (unequal ident)
        void resetTransponderMode();

        //! \brief Selected transponder mode
        void setSelectedTransponderModeAsString(const QString &mode);

    signals:
        //! \brief Ident ended
        void identEnded();
    };
} // namespace

#endif // guard
