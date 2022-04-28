#pragma once

//--------------------------------

#include "List.h"
#include "Graph.h"

#include <cstdio>
#include <typeinfo>
#include <cassert>
#include <algorithm>

//--------------------------------

template <typename elem_t, size_t Capacity>
List <elem_t, Capacity>::List ():
	m_data       {},
	m_head       (-2),
	m_tail       (-2),
	m_free_begin (0)
{
	clear ();
}

//--------------------------------

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::_alloc ()
{
	assert (m_free_begin >= 0);
	int index = m_free_begin;

	m_free_begin = m_data[index].next;
	return index;
}

template <typename elem_t, size_t Capacity>
bool List <elem_t, Capacity>::isFree (int index)
{
	assert (0 <= index && index < Capacity);
	return m_data[index].prev == -1;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::dump ()
{
	printf ("List <%s, %zu> dump:\n", typeid (elem_t).name (), Capacity);
	printf ("  head: %d\n",       m_head);
	printf ("  tail: %d\n",       m_tail);
	printf ("  free begin: %d\n", m_free_begin);

	int index = head ();
	if (index == -1)
	{
		printf ("  All data is free\n");
		return;
	}

	printf ("  Data:\n");
	for (index; index >= 0; index = m_data[index].next)
	{
		printf ("    index: %d    \n", index);
		printf ("    prev:  %d%s  \n", m_data[index].prev, m_data[index].prev != -1? "": " (free block)");
		printf ("    next:  %d    \n", m_data[index].next);
		printf ("    data:  0x%0*X\n", 2*sizeof (elem_t), m_data[index].data);
		printf ("                 \n");
	}
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::dumpGraph ()
{
	Graph graph ("list_graph_dump", false);
	graph.add ("dpi      = 200;");
	graph.add ("ordering = in;");
	graph.add ("rankdir  = LR;");
	graph.add ("");
	graph.add ("node [shape = record, style = filled, fontname = consolas];");
	graph.add ("");

	for (int index = 0; index < Capacity; index++)
	{
		static char logical_str[BUFFSIZE] = "";
		if (isFree (index)) sprintf_s (logical_str, "logical: free");
		else                sprintf_s (logical_str, "logical: %d", logical (index));

		graph.add 
		(
			"\"elem%d\" [label = \"{ <index> index: %d | <logical> %s } | { data: 0x%0*X } | { <prev> prev: %d%s | <next> next: %d%s }\", fillcolor = %s];", 
			index, 
			index,
			logical_str,
			2*sizeof (elem_t),
			*get (index),
			m_data[index].prev, m_data[index].prev == -2? " [END]": "",
			m_data[index].next, m_data[index].next == -2? " [END]": "",
			isFree (index)? Graph::Color::LightGreen: Graph::Color::LightBlue
		);
	}

	// Прозрачные стрелки для соблюдения физической последовательности
	graph.add ("");
	for (int index = 0; index < Capacity-1; index++)
		graph.add ("\"elem%d\" -> \"elem%d\" [style = invis, weight = 10, penwidth = 3];", index, index+1);

	// Связь блоков в логической последовательности
	graph.add ("");
	for (int index = head (); index >= 0; index = next (index))
		if (m_data[index].next >= 0)
			graph.add ("\"elem%d\":<next> -> \"elem%d\":<index>;", index, m_data[index].next);

	// Связь свободных блоков
	graph.add ("");
	for (int index = m_free_begin; index >= 0; index = m_data[index].next)
		if (m_data[index].next >= 0)
			graph.add ("\"elem%d\":<next> -> \"elem%d\":<index>;", index, m_data[index].next);

	int status = graph.render ();
	if (status == 0)
	{
		char cmd[BUFFSIZE] = "";
		sprintf_s (cmd, "start %s", graph.image.c_str ());
		system (cmd);
	}

	else printf ("Graphviz error: 0x%08X (%d)\n", status, status);
}

//--------------------------------

template <typename elem_t, size_t Capacity>
elem_t* List <elem_t, Capacity>::get (int index)
{
	assert (0 <= index && index < Capacity);
	return &(m_data[index].data);
}

//--------------------------------

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::head ()
{
	return m_head;
}

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::tail ()
{
	return m_tail;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::next (int index)
{
	if (index < 0 || m_data[index].next < 0) 
		return -1;

	return m_data[index].next;
}

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::prev (int index)
{
	if (index < 0 || m_data[index].prev < 0) 
		return -1;

	return m_data[index].prev;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::insert_front (const elem_t& elem)
{
	assert (hasFree ());
	int index = _alloc ();

	m_data[index].data = elem;
	m_data[index].prev = -2;
	m_data[index].next = m_head;

	if (m_head >= 0) m_data[m_head].prev = index;
	m_head = index;

	if (m_tail < 0) 
		m_tail = index;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::insert_back (const elem_t& elem)
{
	assert (hasFree ());
	int index = _alloc ();

	m_data[index].data = elem;
	m_data[index].prev = m_tail;
	m_data[index].next = -2;

	if (m_tail >= 0) m_data[m_tail].next = index;
	m_tail = index;

	if (m_head < 0)
		m_head = index;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::insert_before (int index, const elem_t& elem)
{
	assert (hasFree ());
	int new_index = _alloc ();

	m_data[new_index].data = elem;
	m_data[new_index].prev = prev (index);
	m_data[new_index].next = index;
	m_data[prev (index)].next = new_index;
	m_data[index].prev = new_index;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::insert_after (int index, const elem_t& elem)
{
	assert (hasFree ());
	int new_index = _alloc ();

	m_data[new_index].data = elem;
	m_data[new_index].prev = index;
	m_data[new_index].next = next (index);
	m_data[next (index)].prev = new_index;
	m_data[index].next = new_index;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::remove (int index)
{
	if (index == m_head) m_head = next (index);
	if (index == m_tail) m_tail = prev (index);

	m_data[next (index)].prev = prev (index);
	m_data[prev (index)].next = next (index);

	m_data[index].data = {0};
	m_data[index].prev = -1;
	m_data[index].next = m_free_begin;
	m_free_begin = index;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::clear ()
{
	for (size_t i = 0; i < Capacity; i++)
	{
		m_data[i].prev = -1;
		m_data[i].next = -1;
		m_data[i].data = {0};

		if (i > 0) m_data[i-1].next = i;
	}

	m_head = m_tail = -2;
	m_free_begin = 0;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::physical (int logical)
{
	for (int physical = head (), number = 0; physical >= 0; physical = next (physical), number++)
		if (number == logical) return physical;

	return -1;
}

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::logical (int physical)
{
	for (int index = m_head, logical = 0; index >= 0; index = next (index), logical++)
		if (index == physical) return logical;
		
	return -1;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
size_t List <elem_t, Capacity>::size ()	const
{
	size_t count = 0;
	for (int index = head (); index >= 0; index = next (index))
		count++;

	return count;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
int List <elem_t, Capacity>::find (const elem_t& value)
{
	for (int index = head (); index >= 0; index = next (index))
		if (m_data[index].data == value) return index;

	return -1;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
void List <elem_t, Capacity>::sort ()
{
	for (int index = head (); index >= 0; index = next (index))
		m_data[index].prev = logical (index);

	std::sort (begin (), end (),
		[](const element& lft, const element& rgt) 
		{ 
			if (lft.prev == -1) return false; 
			if (rgt.prev == -1) return true;
			return lft.prev < rgt.prev; 
		}
	);

	m_head = m_tail = 0;
	for (int index = 0; index < Capacity; index++)
	{
		if (isFree (index)) 
		{
			m_free_begin = index;
			break;
		}
	}

	for (int index = 0; index < m_free_begin; index++)
	{
		m_data[index].next = index < m_free_begin-1? index+1: -2;
		m_data[index].prev = index > 0?              index-1: -2;
	}

	for (int index = m_free_begin; index < Capacity; index++)
	{
		m_data[index].next = index < Capacity-1? index+1: -2;
		m_data[index].prev = -1;
	}
}

//--------------------------------

template <typename elem_t, size_t Capacity>
elem_t& List <elem_t, Capacity>::operator [] (int index)
{
	assert (index >= 0 && Capacity > index);
	return m_data[index].data;
}

template <typename elem_t, size_t Capacity>
const elem_t& List <elem_t, Capacity>::operator [] (int index) const
{
	assert (index >= 0 && Capacity > index);
	return m_data[index].data;
}

//--------------------------------

template <typename elem_t, size_t Capacity>
bool List <elem_t, Capacity>::hasFree ()
{
	return m_free_begin >= 0;
}

//--------------------------------