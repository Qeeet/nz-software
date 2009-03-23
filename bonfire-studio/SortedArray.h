/*
Module : SortedArray.H
Purpose: Interface for an MFC template class which provides sorting and ordered insertion
         derived from the MFC class CArray
Created: PJN / 25-12-1999
History: PJN / 12-01-2000 Fixed a stack overflow in CSortedArray::Sort
         PJN / 21-02-2000 Fixed a number of problems in CSortedArray::Find
         PJN / 22-02-2000 Fixed a problem in CSortedArray::Find when there are no items in the array
         PJN / 29-02-2000 Fixed a problem in CSortedArray::Sort when there are no items in the array

Copyright (c) 1999 - 2000 by PJ Naughter.  All rights reserved.
*/

#if !defined(AFX_SORTEDARRAY_H__B18414EE_415E_4420_8E4C_CBD2618A7561__INCLUDED_)
#define AFX_SORTEDARRAY_H__B18414EE_415E_4420_8E4C_CBD2618A7561__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<class TYPE, class ARG_TYPE>
class CSortedArray : public CArray<TYPE, ARG_TYPE>
{
public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	CSortedArray()
	{
		m_lpfnCompareFunction = NULL;
	}

//Typedefs
	typedef int COMPARE_FUNCTION(ARG_TYPE element1, ARG_TYPE element2); 
	typedef COMPARE_FUNCTION* LPCOMPARE_FUNCTION;

//Methods
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	int  OrderedInsert(ARG_TYPE newElement, int nCount=1)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		int nLowIndex = 0;
		int nHighIndex = GetUpperBound();

		//if there are no elements in the array then just insert it at the begining
		if (nHighIndex == -1)
		{
			InsertAt(0, newElement, nCount);
			return 0;
		}

		//do a binary chop to find the location where the element should be inserted
		int nInsertIndex = -1;
		do
		{
			int nCompareLow = m_lpfnCompareFunction(newElement, ElementAt(nLowIndex));
			int nCompareHigh = m_lpfnCompareFunction(newElement, ElementAt(nHighIndex));  
			
			if (nCompareLow <= 0)
				nInsertIndex = nLowIndex;
			else if (nCompareHigh == 0)
				nInsertIndex = nHighIndex;
			else if (nCompareHigh == 1)
				nInsertIndex = nHighIndex+1;
			else
			{
			  ASSERT(nLowIndex < nHighIndex);

			  int nCompareIndex;
			  if (nHighIndex == (nLowIndex+2))
				nCompareIndex = nLowIndex+1;
			  else
				nCompareIndex = ((nHighIndex - nLowIndex)/2) + nLowIndex;

			  int nCompare = m_lpfnCompareFunction(newElement, ElementAt(nCompareIndex));
			  switch (nCompare)
			  {
				case -1:
				{
				  if ((nHighIndex - nLowIndex) == 1)
					nHighIndex = nLowIndex;
				  else
					nLowIndex = nCompareIndex;
				  break;
				}
				case 0:
				{
				  nInsertIndex = nCompareIndex;
				  break;
				}
				case 1:
				{
				  if ((nHighIndex - nLowIndex) == 1)
					nLowIndex = nHighIndex;
				  else
					nHighIndex = nCompareIndex;
				  break;
				}
				default:
				{
				  ASSERT(FALSE); //Your compare function has been coded incorrectly. It should
								 //return -1, 0 or 1 similiar to the way the C Runtime function
								 //"qsort" works
				  break;
				}
			  }
			}
		}
		while (nInsertIndex == -1);

		//Do the actual insert
		InsertAt(nInsertIndex, newElement, nCount);
		return nInsertIndex;
	};
	
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	void Sort(int nLowIndex=0, int nHighIndex=-1)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		// If there are no items in the array, then return immediately
		if (GetSize() == 0)
			return;

		int left = nLowIndex;
		int right = nHighIndex;
		if (right == -1)
			right = GetUpperBound();

		if (right-left <= 0) //Do nothing if fewer than 2 elements are to be sorted
			return;

		// (not really needed, just to optimize)
		if (right-left == 1)  //Do a simple comparison if only 2 elements
		{
			if (m_lpfnCompareFunction(ElementAt(right), ElementAt(left)) == -1)
				swap(ElementAt(left), ElementAt(right));
			return;
		}

		// move partition element to begining 
		swap(ElementAt(left), ElementAt((left+right)/2));
		int last = left;

		for (int i=left+1; i<=right; i++) //Partition
		{
			if (m_lpfnCompareFunction(ElementAt(i), ElementAt(left)) == -1)
				swap(ElementAt(++last), ElementAt(i));
		}

		swap(ElementAt(left), ElementAt(last)); //Restore partition element
		Sort(left, last-1);
		Sort(last+1, right);  
	};

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	int Find(ARG_TYPE element, int nLowIndex=0, int nHighIndex=-1)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		//If there are no items in the array, then return immediately
		if (GetSize() == 0)
			return -1;

		int left = nLowIndex;
		int right = nHighIndex;
		if (right == -1)
			right = GetUpperBound();
		ASSERT(left <= right);

		if (left == right) //Straight comparision fewer than 2 elements to search
		{
			BOOL bFound = (m_lpfnCompareFunction(ElementAt(left), element) == 0);
			if (bFound)
				return left;
			else
				return -1;
		}

		//do a binary chop to find the location where the element should be inserted
		int nFoundIndex = -1;
		while (nFoundIndex == -1 && left != right)
		{
			int nCompareIndex;
			if (right == (left+2))
			  nCompareIndex = left+1;
			else
			  nCompareIndex = ((right - left)/2) + left;

			int nCompare = m_lpfnCompareFunction(ElementAt(nCompareIndex), element);
			switch (nCompare)
			{
				case -1:
				{
					if ((right - left) == 1)
					{
						if (m_lpfnCompareFunction(ElementAt(right), element) == 0)
							nFoundIndex = right;
						else if (m_lpfnCompareFunction(ElementAt(left), element) == 0)
							nFoundIndex = left;
						else
							left = right;
					}
					else
						left = nCompareIndex;
					break;
				}
				case 0:
				{
					nFoundIndex = nCompareIndex;
					break;
				}
				case 1:
				{
					if ((right - left) == 1)
					{
						if (m_lpfnCompareFunction(ElementAt(right), element) == 0)
							nFoundIndex = right;
						else if (m_lpfnCompareFunction(ElementAt(left), element) == 0)
							nFoundIndex = left;
						else
							right = left;
					}
					else
						right = nCompareIndex;
					break;
				}
				default:
				{
					//Your compare function has been coded incorrectly. It should
					//return -1, 0 or 1 similiar to the way the C Runtime function "qsort" works
					ASSERT(FALSE); 
					break;
				}
			}
		}

		return nFoundIndex;
	};

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	void SetCompareFunction(LPCOMPARE_FUNCTION lpfnCompareFunction) 
	{
		m_lpfnCompareFunction = lpfnCompareFunction; 
	};

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	LPCOMPARE_FUNCTION GetCompareFunction() const 
	{ 
		return m_lpfnCompareFunction; 
	};

protected:
	LPCOMPARE_FUNCTION m_lpfnCompareFunction;

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	/*LPCOMPARE_FUNCTION m_lpfnDefaultCompareFunction(ARG_TYPE element1, ARG_TYPE element2)
	{
		if (element1 > element2)
			return 1;
		if (element1 == element2)
			return 0;
		if (element1 < element2)
			return -1;
	};*/

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	void swap(ARG_TYPE element1, ARG_TYPE element2)
	{
		TYPE temp = element1;
		element1 = element2;
		element2 = temp;
	};
};

#endif // !defined(AFX_SORTEDARRAY_H__B18414EE_415E_4420_8E4C_CBD2618A7561__INCLUDED_)



