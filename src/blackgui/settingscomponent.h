#ifndef BLACKGUI_SETTINGSCOMPONENT_H
#define BLACKGUI_SETTINGSCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include <QTabWidget>
#include <QModelIndex>
#include <QTimer>


namespace Ui { class CSettingsComponent; }

namespace BlackGui
{
    //! Settings component
    class CSettingsComponent : public QTabWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsComponent();

        //! ICAO data from GUI
        void setOwnAircraftIcaoDataFromGui(BlackMisc::Aviation::CAircraftIcao &icao) const;

        //! GUI Opacity 0-100%
        void setGuiOpacity(double value);

        //! Login as observer
        bool loginAsObserver() const;

        //! Login as observer
        bool loginStealth() const;

        //! Play notification sounds (at all)
        bool playNotificationSounds() const;

        //! ATC refresh time
        int getAtcUpdateIntervalSeconds() const;

        //! Aircraft refresh time
        int getAircraftUpdateIntervalSeconds() const;

        //! Aircraft refresh time
        int getUsersUpdateIntervalSeconds() const;

        //! Own callsign
        QString getOwnCallsignFromGui() const;

    signals:
        //! Change the windows opacity 0..100
        void changedWindowsOpacity(int opacity);

        //! Update interval changed (ATC)
        void changedAtcStationsUpdateInterval(int seconds);

        //! Update interval changed (aircrafts)
        void changedAircraftsUpdateInterval(int seconds);

        //! Update interval changed (users)
        void changedUsersUpdateInterval(int seconds);

    public slots:
        //! Reload settings
        void reloadSettings();

    protected:
        //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
        virtual void runtimeHasBeenSet() override;

    private slots:

        //! Network server selected
        void networkServerSelected(QModelIndex index);

        //! Alter traffic server
        void alterTrafficServer();

        /*!
         * \brief Update the selected server textboxes
         * \param server to be displayed
         */
        void updateGuiSelectedServerTextboxes(const BlackMisc::Network::CServer &server);

        //! Selected server from textboxes
        BlackMisc::Network::CServer selectedServerFromTextboxes() const;

        //! Settings have been changed
        void changedSettings(uint typeValue);

        //! Save the Hotkeys
        void saveHotkeys();

        //! Clear single hotkey
        void clearHotkey();

        //! start the MIC tests (Squelch)
        void startAudioTest();

        //! Audio test updates (timer) for progressbar and fetching results
        void audioTestUpdate();

        /*!
         * \brief Audio device selected
         * \param index audio device index (COM1, COM2)
         */
        void audioDeviceSelected(int index);

    private:
        //! Audio test modes
        enum AudioTest
        {
            NoAudioTest,
            SquelchTest,
            MicrophoneTest
        };

        Ui::CSettingsComponent *ui;
        QTimer *m_timerAudioTests; //!< audio tests: progress bar, disable/enable buttons
        AudioTest m_audioTestRunning;

        //! Audio device lists from settings
        void initAudioDeviceLists();
    };
} // namespace

#endif // guard
