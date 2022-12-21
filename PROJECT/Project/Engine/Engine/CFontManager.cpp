#include "pch.h"

#include "CFontManager.h"

namespace
{
    const wchar_t *pSzFontFilePath = L"font\\alagard.spritefont";
}

void CFontManager::Remove(const TextInfo* pText)
{
    auto iter = std::find(m_listText.begin(), m_listText.end(), pText);
    if (iter != m_listText.end())
    {
        m_listText.erase(iter);
    }
}

CFontManager::CFontManager()  = default;
CFontManager::~CFontManager() = default;

void CFontManager::Init()
{
    m_pSpriteBatch = std::make_unique<SpriteBatch>(CONTEXT);

    const std::wstring contentPath = CPathMgr::GetInst()->GetContentPath();
    const std::wstring fontPath    = contentPath + pSzFontFilePath;
    m_pSpriteFont                  = std::make_unique<SpriteFont>(DEVICE, fontPath.c_str());
}

void CFontManager::Render()
{
    m_pSpriteBatch->Begin(SpriteSortMode_Deferred
                        , nullptr
                        , CDevice::GetInst()->GetSampler(SAMPLER_TYPE::POINT).Get());
    for (const auto &textInfo : m_listText)
    {
        m_pSpriteFont->DrawString(m_pSpriteBatch.get()
                                , textInfo->content.c_str()
                                , textInfo->position
                                , textInfo->color
                                , textInfo->rotationAngle
                                , Vec2{0.f, 0.f}
                                , textInfo->scale);
    }
    m_pSpriteBatch->End();
}
