#include "WAV.h"
#include "Debug.h"

#include <fstream>
#include "Application.h"
#include "Utils.h"

void WAV::Load(const std::string& a_sFilePath)
{
	m_sPath = a_sFilePath;

#ifdef ANDROID
	/* Cache WAV */
	std::string path = CacheFileToPath(a_sFilePath.c_str());
	std::string folderPath = GetFolderPath(path);

	std::ifstream ifStream;
	ifStream.open(path.c_str(), std::ios::in | std::ios::binary);
#else
	std::ifstream ifStream;
	ifStream.open(a_sFilePath.c_str(), std::ios::in | std::ios::binary);
#endif

	if (ifStream.fail())
	{
		Debug::Log("WAV failed to load: no data detected");	
		delete this;
		return;
	}

	/* Read RIFF Chunk */
	RIFF sRIFFHeader;
	ifStream.read((char*)&sRIFFHeader, sizeof(RIFF));
	
	const std::string sRIFFID(sRIFFHeader.ID, 0, 4);
	const std::string sWAVEID(sRIFFHeader.Format, 0, 4);

	if (sRIFFID != RIFF_MAGIC || sWAVEID != WAVE_MAGIC)
	{
	  Debug::Log("WAV failed to load: bad WAV file detected");
	  delete this;
	  return;	
	}

	const int iStart = ifStream.tellg();

	/* Read Next Chunk */
	int iOffset = 0;

	for (int i = 0; i < 2; i++)
	{
	  char cType[4];
	  ifStream.read((char*)&cType, 4);
	  std::string sType(cType, 0, 4);

		/* Check if it's FMT, otherwise keep looking at new offset */
		if (sType == FMT_MAGIC)
		{
		  break;
		}
		else
		{
			int iSize;
			ifStream.read((char*)&iSize, 4);
			iOffset += iSize + 8;

			ifStream.seekg(iStart + iOffset);
		}
	}

	ifStream.seekg(iStart + iOffset);
  
	/* Read FMT */
	FMT sFMTHeader;
	ifStream.read((char*)&sFMTHeader, sizeof(FMT));
  
	/* Read Data */
	Data sDataHeader;
	ifStream.read((char*)&sDataHeader, sizeof(Data));

	const std::string sFMTID(sFMTHeader.ID, 0, 4);

	if (sFMTID != FMT_MAGIC)
	{
		Debug::Log("WAV failed to load: bad WAV file detected");
		delete this;
		return;	
	}

	char* pBuffer = new char[sDataHeader.Size];
	ifStream.read((char*)pBuffer, sDataHeader.Size);
	
	ifStream.close();

	/* Setup OpenAL stuff */
	ALuint uiFrequency = sFMTHeader.SampleRate;
	ALenum eFormat;
  
	if (sFMTHeader.BitsPerSample == 8)
		eFormat = (sFMTHeader.Channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
	else if (sFMTHeader.BitsPerSample == 16)
		eFormat = (sFMTHeader.Channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	else
	{
		Debug::Log("WAV failed to load: invalid bits per sample rate detected!");
		delete this;
		return;
	}

	alGenBuffers(1, &m_uiBuffer);
	alGenSources(1, &m_uiSource);

	alBufferData(m_uiBuffer, eFormat, pBuffer, sDataHeader.Size, uiFrequency);

	Bind();

	/* Temp vars */
	SetPitch(1.0f);
	SetVolume(1.0f);
	SetLooping(true);

	delete[] pBuffer;
	m_bIsLoaded = true;
}
