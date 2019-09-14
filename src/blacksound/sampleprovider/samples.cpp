#include "samples.h"
#include "blackmisc/directoryutils.h"

Samples &Samples::instance()
{
    static Samples samples;
    return samples;
}

Samples::Samples() :
    m_crackle(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/Crackle_f32.wav"),
    m_click(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/Click_f32.wav"),
    m_whiteNoise(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/WhiteNoise_f32.wav")
{ }

ResourceSound Samples::click() const
{
    return m_click;
}

ResourceSound Samples::crackle() const
{
    return m_crackle;
}

ResourceSound Samples::whiteNoise() const
{
    return m_whiteNoise;
}
