#include "SoundFader.h"
#include "AudioSource.h"
#include "GameTime.h"

SoundFader::SoundFader(GameObject * a_pParent) :
	BehaviorScript(a_pParent)
{
}

void SoundFader::Transition(AudioSource* a_pSource, AudioFile* a_pAudioFile, float a_fSpeed)
{
	std::string routineName = "Transition" + a_pAudioFile->GetPath();
	StartRoutine(std::bind(&SoundFader::TransitionRoutine, this, a_pSource, a_pAudioFile, a_pSource->GetVolume(), a_fSpeed), routineName.c_str());
}

void SoundFader::FadeOut(AudioSource* a_pSource, float a_fSpeed)
{
	std::string routineName = "FadeOut" + a_pSource->GetAudioFile()->GetPath();
	StartRoutine(std::bind(&SoundFader::FadeOutRoutine, this, a_pSource, a_fSpeed), routineName.c_str());
}

void SoundFader::FadeIn(AudioSource* a_pSource, float a_fSpeed, float a_fTargetVolume)
{
	std::string routineName = "FadeIn" + a_pSource->GetAudioFile()->GetPath();
	StartRoutine(std::bind(&SoundFader::FadeInRoutine, this, a_pSource, a_fSpeed, a_fTargetVolume), routineName.c_str());
}

float SoundFader::TransitionRoutine(AudioSource* a_pSource, AudioFile* a_pAudioFile, float a_fStartVolume, float a_fSpeed) const
{
	float volume = a_pSource->GetVolume();
	if (a_pSource->GetAudioFile() != a_pAudioFile)
	{
		if (volume > 0.01f)
		{
			volume -= GameTime::GetDeltaTime() * a_fSpeed * a_fStartVolume;
			a_pSource->SetVolume(volume);
			return 0.f;
		}
		a_pSource->SetAudioFile(a_pAudioFile);
		a_pSource->Play();
		return 0.f;
	}

	if (volume <= a_fStartVolume)
	{
		volume += GameTime::GetDeltaTime() * a_fSpeed * a_fStartVolume;
		a_pSource->SetVolume(volume);
		return 0.f;
	}

	return STOP_ROUTINE;
}

float SoundFader::FadeInRoutine(AudioSource* a_pSource, float a_fSpeed, float a_fTargetVolume) const
{
	float volume = a_pSource->GetVolume();
	if (volume >= a_fTargetVolume) return STOP_ROUTINE;

	volume += GameTime::GetDeltaTime() * a_fSpeed;
	a_pSource->SetVolume(volume);

	return 0.f;
}

float SoundFader::FadeOutRoutine(AudioSource* a_pSource, float a_fSpeed) const
{
	float volume = a_pSource->GetVolume();
	if (volume <= 0.01f) return STOP_ROUTINE;

	volume -= GameTime::GetDeltaTime() * a_fSpeed;
	a_pSource->SetVolume(volume);

	return 0.f;
}