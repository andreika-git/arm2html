//////////////////////////////////////////////////////////////////////////
/**
 * support lib hash list (search) template prototype
 *	\file		hashlist.h
 *	\author		bombur
 *	\version	0.2
 *	\date		17.02.2004 19.01.2002
 */
//////////////////////////////////////////////////////////////////////////

#ifndef RD_HASHLIST_H
#define RD_HASHLIST_H

#include <support/rdmemory.h>
#include <support/rddebug.h>
#include <support/list.h>
#include <support/dllist.h>


/// Hash-table list (search-optimised) template.
/// Use SetN() first, then Add() or Merge() to add elements;
/// Get() searches for elements, and GetN() returns the actual number stored.
///	\warning	only single-threaded version!!!
template <class T>
class RDHashList : public RDList<RDDLinkedList<T> *>
{
public:
	///////////////////////////////////////////////////////////////
	/// Hash function - declare it in derived class.
	/// The return value should be in the range of [0..num-1]
	virtual DWORD HashFunc(const T & item) = 0;
	/*{
		return (DWORD)item % ((num > 0) ? num : 1);
	}*/

    /// Comparision function - declare it in derived class
    virtual BOOL Compare(const T & item1, const T & item2) = 0;
    /*{
    	return (item1 == item2);
    }*/
	///////////////////////////////////////////////////////////////

	/// Ctor. Creates  list with N hash-size
	RDHashList (int N = 1) : RDList<RDDLinkedList<T> *>()
	{
		SetN(N);
	}

	/// Copy ctor.
	RDHashList (const RDHashList & list) : RDList<RDDLinkedList<T> *>(list)
	{
	}

	/// Dtor. Destroys only the list, not the elements.
	~RDHashList ()
	{
		DeleteObjects();
		for (int i = 0; i < num; i++)
		{
			delete l[i];
		}
	}

	/// Add item to the list
	BOOL Add (const T & item)
	{
		if (num == 0)
			return FALSE;
		DWORD f = HashFunc(item);
		DLElement<T> *elem = new DLElement<T>(item);
		return l[f]->Add(elem);
	}

    /// Add unique item to the list, and return the existing or new one.
	T *Merge (const T & item)
	{
		DWORD f = HashFunc(item);
		DLElement<T> *cur = l[f]->GetFirst();
		while (cur != NULL)
		{
			if (Compare(*cur, item))
				return &(*cur).item;
			cur = cur->next;
		}
        DLElement<T> *elem = new DLElement<T>(item);
		if (l[f]->Add(elem) == FALSE)
			return NULL;
		return &(*elem).item;
	}

	/// Find item (return pointer to existing item or NULL)
	T *Get (const T & item)
	{
		DWORD f = HashFunc(item);
        DLElement<T> *cur = l[f]->GetFirst();
		while (cur != NULL)
		{
			if (Compare(*cur, item))
				return &(cur->item);
			cur = cur->next;
		}
		return NULL;
	}

	/// Find the next item of the specified one (or the first, if called with 'NULL')
	T *GetNext(T *item)
	{
		DWORD f;
		if (item == NULL)
		{
    		if (num == 0)
    			return NULL;
    		for (f = 0; f < (DWORD)num; f++)
    		{
    			if (l[f]->GetFirst() != NULL)
    				return &l[f]->GetFirst()->item;
    		}
    		return NULL;
   		}

		f = HashFunc(*item);
        DLElement<T> *cur = l[f]->GetFirst();
		while (cur != NULL)
		{
			if (Compare(*cur, *item))
			{
				if (cur->next == NULL) // get from the next row
				{
					while (++f < (DWORD)num)
					{
						if (l[f]->GetFirst() != NULL)
							return &l[f]->GetFirst()->item;
					}
					return NULL;
				} else
					return &(cur->next->item);
			}
			cur = cur->next;
		}
		return NULL; // not in this list
	}

	/// Delete item from the list (but don't delete the item itself!)
	BOOL Delete (const T & item)
	{
		DWORD f = HashFunc(item);
        DLElement<T> *cur = l[f]->GetFirst();
		while (cur != NULL)
		{
			if (Compare(*cur, item))
				return l[f]->Delete(cur);
			cur = cur->next;
		}
		return FALSE;
	}

	/// Remove all items from the list
	BOOL Clear ()
	{
		for (int i = 0; i < num; i++)
		{
			l[i]->Delete();
		}
        return TRUE;
	}

	/// Delete all objects themself and clear the list
	BOOL DeleteObjects ()
	{
		for (int i = 0; i < num; i++)
		{
			if (l[i]->Delete() == FALSE)
				return FALSE;
		}
		return TRUE;
	}

	/// Get items number (count)
	inline DWORD GetN ()
	{
		DWORD n = 0;
		for (int i = 0; i < num; i++)
			n += l[i]->GetN();
		return n;
	}

	/// Set items count (reallocate the memory but don't change the data)
	BOOL SetN(DWORD n)
	{
		if (num > 0)
		{
			for (int i = 0; i < num; i++)
			{
				delete l[i];
			}
		}
		if (RDList<RDDLinkedList<T> *>::SetN(n) == FALSE)
			return FALSE;
		for (int i = 0; i < num; i++)
			l[i] = new RDDLinkedList<T>;
		return TRUE;
	}
};


/// Integer hash table implementation.
/// \warning For testing purposes only.
class RDIntHashList : public RDHashList<int>
{
public:
	/// ctor
	RDIntHashList()
	{
	}
	
	/// dtor
	~RDIntHashList()
	{
	}

    /// Hash function - declare it in derived class.
	/// The return value should be in the range of [0..num-1]
	DWORD HashFunc(int const & item)	// \todo	Get better function
	{
		return (DWORD)item % num;
	}

    /// Comparision function - declare it in derived class
    BOOL Compare(int const & item1, int const & item2)
    {
    	return item1 == item2;
    }
};

/// String hash table implementation.
/// \warning For testing purposes only.
class RDStringHashList : public RDHashList<char *>
{
public:
	/// ctor
	RDStringHashList()
	{
	}
	
	/// dtor
	~RDStringHashList()
	{
	}

    /// Hash function - declare it in derived class.
	/// The return value should be in the range of [0..num-1]
	DWORD HashFunc(char* const & item)	// \todo	Get better function
	{
		if (item == NULL)
			return 0;
		DWORD h = 0;
		char *it = (char *)item;
		for (int i = 0; *it; i++)
			h += ((DWORD)*it++) ^ (i % num);
		return h % num;
	}

    /// Comparision function - declare it in derived class
    BOOL Compare(char* const & item1, char* const & item2)
    {
    	return strcmp(item1, item2) == 0;
    }
};

#endif // of RD_HASHLIST_H
