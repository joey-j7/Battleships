#pragma once

#include "ComponentSystem.h"

#include <al/include/al.h>
#include <al/include/alc.h>

class AudioSource;
class Camera;

class AudioSystem : public ComponentSystem
{
public:
	AudioSystem(Camera* a_pCamera);
	~AudioSystem();

	void Resume();
	void Suspend();
	
protected:
	bool CanProcessComponent(Component* a_pComponent) override;
	void ProcessComponents() override;
	void OnComponentDestroyed(Component* a_pComponent) override;
	void OnComponentAdded(Component* a_pComponent) override;
  
private:
	Camera* m_pCamera;
	std::vector<AudioSource*> m_vAudioSources;

	ALCdevice* m_pDevice = nullptr;
	ALCcontext* m_pContext = nullptr;
};
