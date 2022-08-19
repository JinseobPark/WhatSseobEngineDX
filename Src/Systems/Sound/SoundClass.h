#pragma once
#include "Systems/System.h"

class SoundClass : public System
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	void Initialize()override;
	void Update(float) override;
	void Shutdown();

	bool PlayWaveFile();
private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool LoadWaveFile(const char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);


private:
	IDirectSound8* m_DirectSound = nullptr;
	IDirectSoundBuffer* m_primaryBuffer = nullptr;
	IDirectSoundBuffer8* m_secondaryBuffer1 = nullptr;

	IDirectSound3DListener8* m_listener = nullptr;
	IDirectSound3DBuffer8* m_secondary3DBuffer1 = nullptr;

	bool isOneChannel;
};
extern SoundClass* mSOUND;
