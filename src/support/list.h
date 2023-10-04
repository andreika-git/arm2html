//////////////////////////////////////////////////////////////////////////
/**
 * support lib linear dynamic list template prototype
 *	\file		list.h
 *	\author		bombur
 *	\version	0.2
 *	\date		19.01.2002
 */
//////////////////////////////////////////////////////////////////////////

#ifndef RD_LIST_H
#define RD_LIST_H

#include <support/rdmemory.h>
#include <support/rddebug.h>

const int RD_LIST_BUFFER_SIZE = 50;

/// dynamic linear list template
///	\warning	only single-threaded version!!!
template <class T>
class RDList
{
public:

	/// comparison function prototype (see Sort())
	typedef int (*RDListCompareFunc)(T *e1, T *e2);

	/// Ctor. Creates empty list
	RDList ()
	{
		l = (T *)RDmalloc (sizeof(T) * RD_LIST_BUFFER_SIZE);
		//RDCHECKMSG(l != NULL, MEMERR);
		lastnum = num = 0;
	}

	/// Copy ctor.
	RDList (const RDList & list)
	{
		lastnum = num = list.num;
		l = (T *)RDmalloc ((lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
		//RDCHECKMSG(l != NULL, MEMERR);
		if (num > 0)
		{
			for (int i = 0; i < num; i++)
			{
				l[i] = list.l[i];	// call elements' copy ctors
			}
		}
	}

	RDList & operator = (const RDList & list)
	{
		lastnum = num = list.num;
		RDfree(l);
		l = (T *)RDmalloc ((lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
		//RDCHECKMSG(l != NULL, MEMERR);
		if (num > 0)
		{
			for (int i = 0; i < num; i++)
			{
				l[i] = list.l[i];	// call elements' copy ctors
			}
		}
		return (*this);
	}
	
	/// Dtor. Destroys only the list, not the elements.
	~RDList ()
	{
		RDfree (l);
		l = NULL;
		lastnum = num = 0;
	}

	/// Add item to the end of the list
	int Add (T item)
	{
		if (num + 1 >= lastnum + RD_LIST_BUFFER_SIZE)	// need to expand buffer
		{
			lastnum = (num << 1) + 1;
			l = (T *)RDrealloc ((void *)l, (lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
			if (l == NULL)
			{
				RDERRMES(MEMERR);
				return -1;
			}
		}
		l[num] = item;
		return num++;
	}

	/// Add one list to the end of another list.
	/// Returns the last element added.
	int Add (const RDList<T> & list)
	{
		int ret = -1;
		for (DWORD i = 0; i < list.GetN(); i++)
			ret = Add (list[i]);
		return ret;
	}

	/// Add unique item to the list, and return the existing or new index.
	int Merge (T item)
	{
		int g = Get (item);
		if (g == -1)
			return Add (item);
		return g;
	}
	
	/// Merge lists (avoid item duplicates)
	int Merge (const RDList<T> & list)
	{
		int ret = -1;
		for (DWORD i = 0; i < list.GetN(); i++)
		{
			T item = list[i];
			if (Get (item) == -1)
				ret = Add (item);
		}
		return ret;
	}

	/// Insert the 'item' element to the 'where' position in the list
	bool Insert (T item, int where)
	{
		int n = num;
		if (where < 0)
			return FALSE;
		if (where > (int)num)
			n = where;
		
		if (n + 1 >= lastnum + RD_LIST_BUFFER_SIZE)	// need to expand buffer
		{
			lastnum = n + 1;
			l = (T *)RDrealloc ((void *)l, (lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
			if (l == NULL)
			{
				RDERRMES(MEMERR);
				return FALSE;
			}
		}
		
		register int i;
		for (i = num; i > where; i--)
			l[i] = l[i - 1];
		/*for (i = num; i < n; i++)
			l[i] = 0;*/
		
		l[where] = item;
		num = n + 1;
		return TRUE;
	}

	/// Replace item with new one; expand list if needed.
	bool Put (T item, int where)
	{
		if (where < 0)
			return FALSE;
		if (where >= (int)num)
		{
			num = where + 1;
			if (num >= lastnum + RD_LIST_BUFFER_SIZE)	// need to expand buffer
			{
				lastnum = num;
				l = (T *)RDrealloc ((void *)l, (lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
				if (l == NULL)
				{
					RDERRMES(MEMERR);
					return FALSE;
				}
			}
		}
		l[where] = item;
		return TRUE;
	}

	/// Find item's index number
	int Get (T item)
	{
		for (int i = 0; i < num; i++)
			if (l[i] == item)
				return i;
		return -1;
	}

	/// Remove the item from the list
	bool Remove (int n)
	{
		if (n < 0 || n >= (int)num)
			return FALSE;
		for (int i = n; i < num - 1; i++)
			l[i] = l[i + 1];
		if (num >= 1)
		{
			if (num - 1 < lastnum)	// need to shrink buffer
			{
				l = (T *)RDrealloc ((void *)l, lastnum * sizeof(T));
				if (lastnum > RD_LIST_BUFFER_SIZE)
					lastnum -= RD_LIST_BUFFER_SIZE;
				else
					lastnum = 0;
				if (l == NULL)
				{
					RDERRMES(MEMERR);
					return FALSE;
				}
			}
			num--;
			return TRUE;
		}
		return FALSE;
	}

	/// Remove item from the list (search used)
	bool Remove (const T & item)
	{
		DWORD i;
		while ((i = Get (item)) != -1)
		{
			if (!Remove (i))
				return FALSE;
		}
		return TRUE;
	}

	/// Remove elements from 'this' list found in the 'list'.
	/// Returns the number of elements removed.
	DWORD Remove (const RDList<T> & list)
	{
		DWORD numdel = 0;
		for (DWORD i = 0; i < list.GetN(); i++)
		{
			int ret = Get (list[i]);
			if (ret != -1)
			{
				Remove (ret);
				numdel++;
			}
		}
		return numdel;
	}

	/// Remove all items from the list
	bool Clear ()
	{
		l = (T *)RDrealloc ((void *)l, sizeof(T) * RD_LIST_BUFFER_SIZE);
		if (l == NULL)
			return FALSE;
		lastnum = num = 0;
		return TRUE;
	}

	/// Moves item in the list (delete + insert)
	bool Move (int from, int to)
	{
		if (from < 0 || from >= (int)num)
			return FALSE;
		T item = l[from];
		if (Delete (from) == FALSE)
			return FALSE;
		if (Insert (item, to) == FALSE)
			return FALSE;
		return TRUE;
	}

	/// Delete all objects themself and clear the list
	bool DeleteObjects ()
	{
		for (int i = 0; i < num; i++)
			RDSafeDelete(l[i]);
		return Clear ();
	}

	/// The main item access operator
	__forceinline T & operator [] (int n) const
	{
		return l[n];
	}

	/// List concatenation and assign
	RDList<T> & operator += (const RDList<T> & list)
	{
		Add (list);
		return *this;
	}

	/// Boolean 'and' operation - returns elements present in both of the lists
	RDList<T> & operator &= (const RDList<T> & list)
	{
		for (int i = 0; i < num; i++)
		{
			if (list.Get(l[i]) == -1)
				Delete (i);
		}
		return *this;
	}

	/// Boolean 'or' operation - returns unique elements present in any of the lists
	RDList<T> & operator |= (const RDList<T> & list)
	{
		Merge(list);
		return *this;
	}

	/// Add one list to another and return the sum.
	/*RDList<T> operator + (const RDList<T> & list)
	{
		RDList<T> r;
		r.Add(*this);
		r.Add(list);
		return r;	
	}*/

	/// Return the elements of the first list not present in the second.
	RDList<T> operator - (const RDList<T> & list)
	{
		RDList<T> r;
		for (int i = 0; i < num; i++)
		{
			if (list.Get(l[i]) == -1)
				r.Add (l[i]);
		}
		return r;
	}

	/// Sort list using user-defined comparison function
	void Sort (RDListCompareFunc compare)
	{
		qsort((void *)l, num, sizeof(T), 
			(int (__cdecl *)(const void *, const void *))compare);
	}

	/// Get items number (count)
	__forceinline DWORD & GetN () const
	{
		return (DWORD &)num;
	}

	/// Set items count (reallocate the memory but don't change the data)
	BOOL SetN(DWORD n)
	{
	    num = (int)n;
	    if ((num >= lastnum + RD_LIST_BUFFER_SIZE) || // need to expand buffer
        	(num <= lastnum - RD_LIST_BUFFER_SIZE))	  // need to shrink buffer
	    {
	    	lastnum = num;
			l = (T *)RDrealloc ((void *)l, (lastnum + RD_LIST_BUFFER_SIZE) * sizeof(T));
			if (l == NULL)
			{
				RDERRMES(MEMERR);
				return FALSE;
			}
		}
		return TRUE;
	}

	/// List concatenation operator
	template <class T> friend RDList<T> operator + (const RDList<T> &, const RDList<T> &);
	/// Boolean 'or' operation - returns unique elements present in any of the lists
	template <class T> friend RDList<T> operator | (const RDList<T> &, const RDList<T> &);
	/// Boolean 'and' operation - returns elements present in both of the lists
	template <class T> friend RDList<T> operator & (const RDList<T> &, const RDList<T> &);

protected:
	/// Dynamic array, contains all data
	T *l;
	/// Number of stored items. Use GetN() for retrieving
	int num;
	int lastnum;	/// used for bufferized memory reallocs
};

/// List concatenation operator
template <class T> inline RDList<T> operator + (const RDList<T> &a, const RDList<T> &b)
{
	return (RDList <T>(a) += (b));
}

/// Boolean 'or' operation - returns unique elements present in any of the lists
template <class T> inline RDList<T> operator | (const RDList<T> &a, const RDList<T> &b)
{
	return (RDList<T>(a) |= b);
}

/// Boolean 'and' operation - returns elements present in both of the lists
template <class T> inline RDList<T> operator & (const RDList<T> &a, const RDList<T> &b)
{
	return (RDList<T>(a) &= b);
}

/// string list
typedef RDList<char *> RD_LIST_STR;
/// dword values list
typedef RDList<DWORD> RD_LIST_DWORD;

#endif // of RD_LIST_H
