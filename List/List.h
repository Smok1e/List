#pragma once

//--------------------------------

template <typename elem_t, size_t Capacity>
class List
{
public:
	List ();

	void dump      ();
	void dumpGraph ();

	elem_t* get (int index);

	int head ();
	int tail ();

	int next (int index);
	int prev (int index);

	void insert_front  (const elem_t& elem);
	void insert_back   (const elem_t& elem);
	void insert_before (int index, const elem_t& elem);
	void insert_after  (int index, const elem_t& elem);

	void remove (int index);
	void clear  ();
	
	int physical (int logical );
	int logical  (int physical);

	constexpr size_t capacity () const { return Capacity; }
	          size_t size     () const;

	int find (const elem_t& value);

	void sort ();

	      elem_t& operator [] (int physical_index);
	const elem_t& operator [] (int physical_index) const;

private:
	struct element
	{
		int    prev;
		int    next;
		elem_t data;
	};

	element m_data[Capacity];

	int m_head;
	int m_tail;
	int m_free_begin;

	constexpr element* begin () { return m_data;            };
	constexpr element* end   () { return m_data + Capacity; };

	int  _alloc  ();
	bool isFree  (int index);
	bool hasFree ();

};

//--------------------------------

#include "List.hpp"

//--------------------------------