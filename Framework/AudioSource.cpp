#include "AudioSource.h"
#include "Application.h"

#include <al/include/alc.h>

#include "Debug.h"

AudioSource::AudioSource(GameObject* a_pGameObject, AudioFile* a_pAudioFile) : Component(a_pGameObject)
{
	m_pGameObject = a_pGameObject;
	m_pAudioFile = a_pAudioFile;
  
	SetType(E_Audio);
}

AudioSource::~AudioSource()
{
	Application::GetInstance()->GetLoadInterface()->DeleteAudio(m_pAudioFile->GetPath());
}

void AudioSource::SetAudioFile(AudioFile* a_pFile)
{
	float volume = GetVolume();
	m_pAudioFile->Stop();
	Application::GetInstance()->GetLoadInterface()->DeleteAudio(m_pAudioFile->GetPath());
	m_pAudioFile = a_pFile;
	m_pAudioFile->SetVolume(volume);
}

void AudioSource::Play()
{
	m_bPlaying = true;
	m_pAudioFile->Play();
}

void AudioSource::Pause(bool a_bTemp)
{
	if (!a_bTemp) m_bPlaying = false;
	m_pAudioFile->Pause();
}

void AudioSource::Stop() const
{
	m_pAudioFile->Stop();
}

void AudioSource::Suspend() const
{
	m_pAudioFile->Pause();
}

void AudioSource::SetLooping(bool a_bLooping)
{
	m_pAudioFile->SetLooping(a_bLooping);
}

void AudioSource::SetVolume(float a_fVolume) const
{
	if (a_fVolume < 0)
		a_fVolume = 0;
	if (a_fVolume > 1)
		a_fVolume = 1;
	m_pAudioFile->SetVolume(a_fVolume);
}

void AudioSource::Update() const
{
	Transform* pTransform = m_pGameObject->GetTransform();
	const ALuint uiBuffer = m_pAudioFile->GetBuffer();
	const ALuint uiSource = m_pAudioFile->GetSource();

	glm::vec3 v3Position = pTransform->GetPosition();
	glm::vec3 v3Up = pTransform->GetUp();
  
	alSourcefv(uiSource, AL_POSITION, (ALfloat*)&v3Position);
	alSourcefv(uiSource, AL_VELOCITY, (ALfloat*)&v3Up);

  #ifdef DEBUG
	const ALCenum error = alGetError();
	if (error != AL_NO_ERROR)
		Debug::Log("Error while playing audio: " + to_string(error));
  #endif
}
