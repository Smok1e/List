#include "List.h"

//--------------------------------
								  
int main ()
{
	List <int, 8> list;
	list.insert_front (0x1);
	list.insert_front (0x2);
	list.insert_front (0x3);
	list.insert_back  (0x4);
	list.insert_after (1, 0x5);
	list.remove       (1);

	list.sort      ();
	list.dumpGraph ();

	return 0;
}

//--------------------------------