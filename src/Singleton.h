/*
	Copyright 2011 Kacper Kasper

	This file is part of Tank.

    Tank is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tank is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tank.  If not, see <http://www.gnu.org/licenses/>.
*/

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