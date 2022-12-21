#pragma once

template <typename T>
class Ptr
{
private:
	T* m_pRes;

public:
	T* Get() { return m_pRes; }
	T* Get() const { return m_pRes; }

	T** GetAddressOf() { return &m_pRes; }
	T** GetAddressOf() const { return &m_pRes; }

public:
	Ptr& operator =(T* _ptr)
	{
		if (nullptr != m_pRes)
			m_pRes->SubRef();

		m_pRes = _ptr;

		if (nullptr != m_pRes)
			m_pRes->AddRef();

		return *this;
	}

	Ptr& operator =(const Ptr<T>& _otherPtr)
	{
		if (nullptr != m_pRes)
			m_pRes->SubRef();

		m_pRes = _otherPtr.m_pRes;

		if (nullptr != _otherPtr.m_pRes)
			m_pRes->AddRef();

		return *this;
	}

	T*   operator ->() { return m_pRes; }
	bool operator ==(T* _pRes) { return m_pRes == _pRes; }
	bool operator !=(T* _pRes) { return !(*this == _pRes); }
	bool operator ==(const Ptr<T>& _OtherPtr) { return m_pRes == _OtherPtr.m_pRes; }
	bool operator !=(const Ptr<T>& _OtherPtr) { return !(*this == _OtherPtr); }

public:
	Ptr()
		:
		m_pRes(nullptr) {}

	Ptr(T* _pRes)
		:
		m_pRes(_pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	Ptr(const Ptr<T>& _otherPtr)
		:
		m_pRes(_otherPtr.m_pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	~Ptr()
	{
		if (nullptr != m_pRes)
		{
			m_pRes->SubRef();
		}
	}
};


template <typename T>
bool operator ==(void* _pRes, const Ptr<T>& _ptr)
{
	return _pRes == _ptr.Get();
}

template <typename T>
bool operator !=(void* _pRes, const Ptr<T>& _ptr)
{
	return !(_pRes == _ptr.Get());
}
