#include "pch.h"
#include "CSound.h"

#include "CResMgr.h"

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              pChannelControl
                           , FMOD_CHANNELCONTROL_TYPE          controlType
                           , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
                           , void*                             pCommandData_1
                           , void*                             pCommandData_2);

FMOD::System* CSound::g_pFMOD = nullptr;

CSound::CSound()
	:
	CRes(RES_TYPE::SOUND)
  , m_pSound(nullptr) {}

CSound::~CSound()
{
	if (nullptr != m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}

int CSound::Play(int loopCount, float volume, bool isOverlap)
{
	if (loopCount <= -1)
	{
		assert(nullptr);
	}

	// 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
	if (!isOverlap && !m_listChannel.empty())
	{
		return -1;
	}

	loopCount -= 1;

	FMOD::Channel* pChannel{};
	g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

	pChannel->setVolume(volume);
	pChannel->setCallback(CHANNEL_CALLBACK);
	pChannel->setUserData(this);
	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(loopCount);

	m_listChannel.push_back(pChannel);

	int index = -1;
	pChannel->getIndex(&index);

	return index;
}

void CSound::Stop()
{
	list<FMOD::Channel*>::iterator iter;

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();
	}
}

void CSound::SetVolume(float volume, int channelIndex)
{
	/*
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();

	int iIdx = -1;
	for (; iter != m_listChannel.end(); ++iter)
	{
		(*iter)->getIndex(&iIdx);
		if (channelIndex == iIdx)
		{
			(*iter)->setVolume(volume);
			return;
		}
	}*/

	auto iter = std::find_if(m_listChannel.begin()
	                       , m_listChannel.end()
	                       , [channelIndex](FMOD::Channel* pChannel)
		                         {
			                         int index{};
			                         pChannel->getIndex(&index);
			                         return index == channelIndex;
		                         });
	assert(iter!=m_listChannel.end());
	(*iter)->setVolume(volume);
}

void CSound::RemoveChannel(FMOD::Channel* pTargetChannel)
{
	/*list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
	for (; iter != m_listChannel.end(); ++iter)
	{
		if (*iter == pTargetChannel)
		{
			m_listChannel.erase(iter);
			return;
		}
	}*/
	auto iter = std::find_if(m_listChannel.begin()
	                       , m_listChannel.end()
	                       , [pTargetChannel](const FMOD::Channel* pChannel)
		                         {
			                         return pTargetChannel == pChannel;
		                         });
	assert(iter!=m_listChannel.end());
	m_listChannel.erase(iter);
}

int CSound::Load(const wstring& filePath)
{
	const std::string path = ToString(filePath);

	if (FMOD_OK != g_pFMOD->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(nullptr);
	}

	return S_OK;
}


// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              pChannelControl
                           , FMOD_CHANNELCONTROL_TYPE          controlType
                           , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
                           , void*                             pCommandData_1
                           , void*                             pCommandData_2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)pChannelControl;
	CSound*        pSound     = nullptr;

	switch (controlType)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
		{
			cppchannel->getUserData((void**)&pSound);
			pSound->RemoveChannel(cppchannel);
		}
		break;
	}

	return FMOD_OK;
}
