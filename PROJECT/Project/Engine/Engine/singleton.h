#pragma once

template <typename T>
class CSingleton
{
	using DESTORY = void(*)(void);

private:
	static T* m_Inst;

public:
	static T* GetInst()
	{
		if (nullptr == m_Inst)
		{
			m_Inst = new T;
		}

		return m_Inst;
	}

	static void Destroy()
	{
		if (nullptr != m_Inst)
		{
			delete m_Inst;
			m_Inst = nullptr;
		}
	}

public:
	CSingleton()
	{
		atexit(Destroy);
	}

	virtual ~CSingleton() { }
};

template <typename T>
T* CSingleton<T>::m_Inst = nullptr;
