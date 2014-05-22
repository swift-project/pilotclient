#ifndef BLACKGUI_COCKPITV1COMPONENT_H
#define BLACKGUI_COCKPITV1COMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include <QWidget>
#include <QPushButton>

namespace Ui { class CCockpitV1Component; }


namespace BlackGui
{

    //! User componenet (users, clients)
    class CCockpitV1Component : public QWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitV1Component(QWidget *parent = nullptr);

        //! Destructor
        ~CCockpitV1Component();

        //! Set external push buttons
        void setExternalButtons(QPushButton *cockpitIdent, QPushButton *cockpitSelected);

        //! Originator for signals
        static const QString &cockpitOriginator();

    public slots:
        //! Update cockpit from context
        void updateCockpitFromContext(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator);

        //! set SELCAL code
        QString getSelcalCode() const;

    private slots:
        //! Test SELCAL
        void testSelcal();

        //! Code values changed
        void cockpitValuesChanged();

        //! Cockpit updates
        void sendCockpitUpdates();

    private:
        Ui::CCockpitV1Component *ui;
        QPushButton *pb_ExternalCockpitIdent;
        QPushButton *pb_ExternalCockpitSelected;

        BlackMisc::Aviation::CAircraft m_ownAircraft; //!< DBus fail safe copy of last state, not used as long as DBus is working / not used with local context as well

        //! Own aircraft object, normally via context (in case of failure, backup object)
        BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        //! COM frequencies displays
        void updateComFrequencyDisplays(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

        //! Reset transponder
        void resetTransponderMode();

        //! Set audio voice rooms
        void setAudioVoiceRooms();

    };
}

#endif // guard
