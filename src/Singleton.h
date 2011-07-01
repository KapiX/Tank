#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<class T>
class Singleton
{
private:
	static T *m_pInstance;

public:
	virtual ~Singleton<T>()
    {
		if(m_pInstance)
			m_pInstance = 0;
	}

	static T *GetInstance()
    {
		if(!m_pInstance)
        {
			m_pInstance = new T;
        }
		return m_pInstance;
	}
};

template<class T> T *Singleton<T>::m_pInstance = 0;

#endif