#pragma once

#include "AudioFile.h"

#define RIFF_MAGIC "RIFF"
#define WAVE_MAGIC "WAVE"
#define JUNK_MAGIC "JUNK"
#define FAKE_MAGIC "Fake"
#define FMT_MAGIC "fmt "

class WAV : public AudioFile
{
public:
	struct RIFF
	{
		char ID[4];
		int Size;
		char Format[4];
	};

	struct FMT
	{
		char ID[4];
		int Size;
		short Format;
		short Channels;
		int SampleRate;
		int ByteRate;
		short BlockAlign;
		short BitsPerSample;
	};

	struct Data
	{
		char ID[4];
		int Size;
	};

	void Load(const std::string& a_sFilePath) override;
};