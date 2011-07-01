#ifndef _LIST_H_
#define _LIST_H_

#include <cstdlib>

template <typename T>
struct ListNode
{
	T item;
	ListNode<T> *next;
	ListNode<T> *prev;
};

template <typename T>
class List
{
public:
	List<T>(): m_iCount(0)
	{
		m_pHead = NULL;
		m_pCurrent = NULL;
	}

	void StartIterator()
	{
		m_pCurrent = m_pHead;
	}

	void MoveForward()
	{
		if(m_pCurrent != NULL)
			m_pCurrent = m_pCurrent->next;
	}

	void MoveBack()
	{
		if(m_pCurrent != NULL)
			m_pCurrent = m_pCurrent->prev;
	}

	T &GetCurrentItem()
	{
		return m_pCurrent->item;
	}

	ListNode<T> *GetIterator()
	{
		return m_pCurrent;
	}

	bool IsIteratorValid()
	{
		return (m_pCurrent != NULL);
	}

	void InsertItem(T item)
	{
		ListNode<T> *node;
		if(IsIteratorValid())
		{
			node = new ListNode<T>;
			node->item = item;
			node->next = m_pCurrent->next;
			node->prev = m_pCurrent;
			m_pCurrent->next = node;
			m_iCount++;
		}
		else
		{
			AppendItem(item);
		}
	}

	void RemoveCurrentItem()
	{
		ListNode<T> *prev;
		if(!m_pCurrent)
			return;
		if(m_pHead == m_pCurrent)
		{
			m_pCurrent = m_pHead->next;
			delete m_pHead;
			m_pHead = m_pCurrent;
		}
		else
		{
			prev = m_pCurrent->prev;
			m_pCurrent = m_pCurrent->next;
			delete prev->next;
			prev->next = m_pCurrent;
		}
		m_iCount--;
	}

	void AppendItem(T item)
	{
		ListNode<T> *node = new ListNode<T>;
		node->next = 0;
		node->item = item;
		ListNode<T> *temp = m_pHead;
		if(temp != NULL)
		{
			while(temp->next != NULL)
				temp = temp->next;
			temp->next = node;
			node->prev = temp;
		}
		else
		{
			m_pHead = node;
		}
		m_iCount++;
	}

	int GetCount()
	{
		return m_iCount;
	}

private:
	ListNode<T> *m_pHead;
	ListNode<T> *m_pCurrent;
	int m_iCount;
};

#endif