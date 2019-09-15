#pragma once

#include "ReferencedObject.h"
#include <string>

#include <al/include/al.h>

class AudioFile : public ReferencedObject
{
public:
	AudioFile() = default;
	virtual ~AudioFile()
	{
		alDeleteBuffers(1, &m_uiBuffer);
		alDeleteSources(1, &m_uiSource);
	};

	const std::string& GetPath() const
	{
		return m_sPath;
	}

	virtual void Load(const std::string& a_sFilePath) = 0;

	void Play() const
	{
		alSourcePlay(m_uiSource);
	}
  
	void Pause() const
	{
		alSourcePause(m_uiSource);
	}
  
	void Stop() const
	{
		alSourceStop(m_uiSource);
	}

	void SetVolume(float a_fVolume) const
	{
		alSourcef(m_uiSource, AL_GAIN, a_fVolume);
	}
	
	void SetPitch(float a_fPitch) const
	{
		alSourcef(m_uiSource, AL_PITCH, a_fPitch);
	}

	void SetLooping(bool a_bLooping) const
	{
		alSourcei(m_uiSource, AL_LOOPING, a_bLooping);
	}

	float GetVolume() const
	{
		float volume;
		alGetSourcef(m_uiSource, AL_GAIN, &volume);
		return volume;
	}

	ALuint GetSource() const
	{
		return m_uiSource;
	}

	ALuint GetBuffer() const
	{
		return m_uiBuffer;
	}

	void Bind() const
	{
		alSourcei(m_uiSource, AL_BUFFER, m_uiBuffer);
	}

	ALint GetState() const
	{
		ALint iState;
		alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);

		return iState;
	}

protected:
	std::string m_sPath;

	ALuint m_uiSource;
	ALuint m_uiBuffer;
};
