#ifndef BLACKGUI_COCKPITV1COMPONENT_H
#define BLACKGUI_COCKPITV1COMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include <QWidget>
#include <QPushButton>

namespace Ui { class CCockpitV1Component; }


namespace BlackGui
{

    /*!
     * User componenet (users, clients)
     */
    class CCockpitV1Component : public QWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitV1Component(QWidget *parent = nullptr);

        //! Destructor
        ~CCockpitV1Component();

        //! Set external push buttons
        void setExternalIdentButton(QPushButton *cockpitIdent);

        //! Volume 0..100 for COM1
        int getCom1Volume() const;

        //! Volume 0..100 for COM1
        int getCom2Volume() const;

        //! Pixmap for voice status
        void setCockpitVoiceStatusPixmap(const QPixmap &pixmap);

        //! Object one of the volume widgets?
        bool isCockpitVolumeWidget(const QObject *sender) const;

        //! Originator for signals
        static const QString &cockpitOriginator();

    public slots:
        //! Update cockpit from context
        void updateCockpitFromContext(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator);

        //! set SELCAL code
        QString getSelcalCode() const;

        //! Volume 0..100 for COM1
        void setCom1Volume(int volume);

        //! Volume 0..100 for COM2
        void setCom2Volume(int volume);

    signals:
        //! Audio volume changed
        void audioVolumeChanged();

    protected:
        //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
        virtual void runtimeHasBeenSet() override;

    private slots:
        //! Test SELCAL
        void testSelcal();

        //! Cockpit values changed from GUI
        void cockpitValuesChanged();

        //! Update voice rooms from list
        void updateAudioVoiceRoomsFromObject(const BlackMisc::Audio::CVoiceRoomList &selectedVoiceRooms);

        //! Update the voice room members
        void updateVoiceRoomMembers();

    private:
        Ui::CCockpitV1Component *ui;
        QPushButton *m_externalCockpitIdentButton; //!< External ident button
        QTimer *m_voiceRoomMembersTimer;

        //! Own aircraft object
        BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        //! Cockpit from aircraft object
        void updateCockpitFromObject(const BlackMisc::Aviation::CAircraft &ownAircraft);

        //! COM frequencies displays
        void updateComFrequencyDisplaysFromObjects(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

        //! Cockpit updates
        bool sendCockpitUpdates(const BlackMisc::Aviation::CAircraft &ownAircraft);

        //! Set audio voice rooms
        void setAudioVoiceRoomUrls();

        //! cockpit values to object, including back annotation
        BlackMisc::Aviation::CAircraft cockpitValuesToObject();

    };
}

#endif // guard
