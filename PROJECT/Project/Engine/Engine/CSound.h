#pragma once
#include "CRes.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif

class CSound final
	: public CRes
{
public:
	static FMOD::System* g_pFMOD;

public:
	CSound();
	virtual ~CSound();

public:
	int Load(const wstring& filePath) override;

public:
	// 0 (무한반복) 0 ~ 1(Volume)
	//Overlap : 채널 내에서 중첩되서 사운드가 발생할지 여부 지정
	int  Play(int loopCount, float volume = 1.f, bool isOverlap = false);
	void Stop();
	// 0 ~ 1
	void SetVolume(float volume, int channelIndex);

private:
	void               RemoveChannel(FMOD::Channel* pTargetChannel);
	friend FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              pChannelControl
	                                  , FMOD_CHANNELCONTROL_TYPE          controlType
	                                  , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
	                                  , void*                             pCommandData_1
	                                  , void*                             pCommandData_2);

private:
	FMOD::Sound*         m_pSound;
	list<FMOD::Channel*> m_listChannel;

};
