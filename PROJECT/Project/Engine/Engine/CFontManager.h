#pragma once
#include <memory>

#include "singleton.h"

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
using namespace DirectX;

#ifdef _DEBUG
#pragma comment(lib,"DirectXTK/DirectXTK_debug")
#else
#pragma comment(lib,"DirectXTK/DirectXTK")
#endif

struct TextInfo
{
    std::wstring content;
    Vec3         position;
    Vec4         color{Colors::Magenta};
    float        rotationAngle{0.f};
    float        scale{1.f};
};

class CFontManager final
    : public CSingleton<CFontManager>
{
public:
    void Init();
    void Render();

public:
    void Insert(TextInfo *pText) { m_listText.push_back(pText); }

    //폰트 매니저에서 제거될 뿐 실제 메모리가 삭제되는 것은 아니다
    void Remove(const TextInfo* pText);
private:
    SINGLE(CFontManager);

private:
    std::unique_ptr<SpriteBatch> m_pSpriteBatch;
    std::unique_ptr<SpriteFont>  m_pSpriteFont;

    std::list<TextInfo *> m_listText;
};
