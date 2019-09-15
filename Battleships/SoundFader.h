#pragma once
#include "BehaviorScript.h"

class AudioSource;
class SoundFader : public BehaviorScript
{
public:
	SoundFader(GameObject* a_pParent);

	void Transition(AudioSource* a_pSource, AudioFile* a_pAudioFile, float a_fSpeed = 1.f);

	void FadeOut(AudioSource* a_pSource, float a_fSpeed);
	void FadeIn(AudioSource* a_pSource, float a_fSpeed, float a_fTargetVolume);

private:
	float TransitionRoutine(AudioSource* a_pSource, AudioFile* a_pAudioFile, float a_fStartVolume, float a_fSpeed = 1.f) const;
	float FadeInRoutine(AudioSource* a_pSource, float a_fSpeed, float a_fTargetVolume) const;
	float FadeOutRoutine(AudioSource* a_pSource, float a_fSpeed) const;
};
