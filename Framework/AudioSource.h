#pragma once

#include "Component.h"
#include "Camera.h"

class AudioFile;
class AudioSource : public Component
{
public:
	AudioSource(GameObject* a_pGameObject, AudioFile* a_pAudioFile);
	~AudioSource();

	void SetAudioFile(AudioFile* a_pFile);

	void Play();
	void Pause(bool a_bTemp = false);
	void Stop() const;

	/* Used by audio system */
	void Suspend() const;

	bool IsPlaying() const { return m_bPlaying; };
	void SetLooping(bool a_bLooping);
	
	void SetVolume(float a_fVolume) const;
	float GetVolume() const { return m_pAudioFile->GetVolume(); }
	const AudioFile* GetAudioFile() const { return m_pAudioFile; }
	
	void Update() const;

private:
	GameObject* m_pGameObject;
	AudioFile* m_pAudioFile;

	bool m_bPlaying;
};
