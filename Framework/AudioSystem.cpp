#include "AudioSystem.h"
#include "AudioSource.h"
#include "Debug.h"

AudioSystem::AudioSystem(Camera* a_pCamera)
{
	m_pCamera = a_pCamera;

	m_pDevice = alcOpenDevice(nullptr);
	if (!m_pDevice) {
		Debug::Log("Error when opening sound device!");
		return;
	}

	m_pContext = alcCreateContext(m_pDevice, nullptr);
	if (!m_pContext) {
		Debug::Log("Error when creating sound context!");
		return;
	}

	alcMakeContextCurrent(m_pContext);
}

AudioSystem::~AudioSystem()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_pContext);
	alcCloseDevice(m_pDevice);
}

void AudioSystem::Resume()
{
	for (AudioSource* pAudioSource : m_vAudioSources)
	{
		if (pAudioSource->IsPlaying())
			pAudioSource->Play();
	}
	
	Debug::Log("Resumed Audio System");
}

void AudioSystem::Suspend()
{
	for (AudioSource* pAudioSource : m_vAudioSources)
	{
		pAudioSource->Suspend();
	}

	Debug::Log("Paused Audio System");
}

bool AudioSystem::CanProcessComponent(Component* a_pComponent)
{
  return a_pComponent->GetType() == Component::E_Audio;
}

void AudioSystem::ProcessComponents()
{
	Transform* pCameraTransform = m_pCamera->GetTransform();
	glm::vec3 v3Orientation[2] = { pCameraTransform->GetForward(), pCameraTransform->GetUp() };

	alListenerfv(AL_POSITION, (ALfloat*)&pCameraTransform->GetPosition()[0]);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, (ALfloat*)&v3Orientation[0]);

	for (AudioSource* pAudioSource : m_vAudioSources)
	{
		pAudioSource->Update();
	}
}

void AudioSystem::OnComponentDestroyed(Component* a_pComponent)
{
	AudioSource* pAudioSource = static_cast<AudioSource*>(a_pComponent);
	const std::vector<AudioSource*>::iterator iter = std::find(m_vAudioSources.begin(), m_vAudioSources.end(), pAudioSource);
	
	if (iter != m_vAudioSources.end())
		m_vAudioSources.erase(iter);
}

void AudioSystem::OnComponentAdded(Component* a_pComponent)
{
	AudioSource* pAudioSource = static_cast<AudioSource*>(a_pComponent);
	const std::vector<AudioSource*>::iterator iter = std::find(m_vAudioSources.begin(), m_vAudioSources.end(), pAudioSource);
	
	if (iter == m_vAudioSources.end())
		m_vAudioSources.push_back(pAudioSource);
}
