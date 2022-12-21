#pragma once
#include "UI.h"


class MenuUI :
	public UI
{
public:
	MenuUI();
	virtual ~MenuUI();

public:
	void render() override;
	void render_update() override;

private:
	void Save();
	void Load();
	void Play();
	void Pause();
	void Stop();
private:
	bool m_bPackaging;
};
