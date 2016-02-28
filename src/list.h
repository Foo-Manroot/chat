/* list.h */
#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

using namespace std;

/*******************************/
/* Definition of custom errors */
/*******************************/

/* List related errors */
#define LIST_ERROR(x)                   ((x) < 0)       /* "true" if it's a list error  */
#define LIST_NOERROR                    0               /* No errors */

/* Error specifiers */
#define LIST_ERR_INDEX          -1      /* The specified index doesn't exists on this list */
#define LIST_ERR_NOT_FOUND      -2      /* The searched item is not in the list */

/* Generic errors */
#define UNKNOWN_ERR             -10     /* An error which cause is unknown */


/*
 *	 This header file declares and implements all the
 *	methods of a simple linked list.
 *
 *	 Becuase it's a template, the preprocessor needs
 *	to know the exact implementation of the list.
 */

template <class Elem>
class List
{
	public:
		/* Constructor */
		List();
		/* Destructor */
		~List();

	/* --- Methods to manipulate the list --- */

	       /*
		* Creates a node with value "element" and adds it
		* at the end of the list.
		*/
		void add (Elem element);

	       /*
		* Creates a node with value "element" and adds it on "index" position (if exists).
		*
		*      Return value:
		*       	0 on success
		*		error code if the specified index doesn't exists
		*
		*      Errors:
		*              LIST_ERR_INDEX
		*/
		int add_index (Elem element, int index);

	       /*
		* Deletes, if found, the first node with value "element".
		*
		*      Return value:
		*              Index of the deleted item on success
		*              error code if "element" isn't found
		*
		*      Errors:
		*              LIST_ERR_NOT_FOUND
		*/
		int del (Elem element);

	       /*
		* Deletes, if found, all the nodes with value "element".
		*
		*      Return value:
		*              Number of elements deleted on success
		*              error code if there's not any node with value "element"
		*
		*      Errors:
		*              LIST_ERR_NOT_FOUND
		*/
		int del_all (Elem element);

	       /*
		* Deletes, if exists, the node on index "index"
		*
		*      Return value:
		*              LIST_NOERROR on success
		*              error code if the specified index doesn't exists
		*
		*      Errors:
		*              LIST_ERR_INDEX
		*/
		int del_index (int index);


	/* --- Methods to control the current state of the list --- */

	       /*
		* Returns "true" if the list is empty.
		*
		*      Return value:
		*              true if the list is empty.
		*              false if the list has at least one node.
		*/
		bool is_empty ();

	       /*
		* Returns the number of elements on the list.
		*
		*      Return value:
		*              Number of elements.
		*/
		int size ();

	       /*
		* Returns the number of elements on the list (same as size()).
		*
		*      Return value:
		*              Number of elements.
		*/
		int num_elem ();

	       /*
		* Returns the number of occurrencies (nodes) which value is "element".
		*
		*      Return value:
		*              -Number of ocurencies of the element (from 0 to "elements").
		*/
		int find_occur (Elem element);

	       /*
		* Returns the index of the first node with value "element",
		* or error if it's not on the list.
		*
		*      Return value:
		*              -Index of the searched element on success.
		*              -LIST_ERR code on error.
		*
		*      Errors:
		*              -LIST_ERR_NOT_FOUND
		*/
		int find_index (Elem element);

	       /*
		* Returns the element on index "index", if exists.
		*
		*      Return value:
		*              -Index of the required element on success.
		*              -NULL if the index doesn't exists.
		*/
		Elem get_elem (int index);

	       /*
		* Prints all the information about the list (number of
		* nodes, first element and last element).
		*
		*      Return value:
		*              -String with all the info.
		*/
		string get_info ();

	       /*
		* Prints the entire content of the list.
		*
		*      Return value:
		*              -Number of elements printed on success.
		*              -Generic error code only if the number of
		*			elements printed != "elements".
		*
		*      Errors:
		*              -UNKNOWN_ERR
		*/
		int print ();

/*****************************/
/** --- Private section --- **/
/*****************************/
	private:
		/* Struct with the definition of the node from the list */
		struct Node
		{
			Elem value;
			Node *next;
		};

		/* Atributes of the list */
		Node *first;	/* Pointer to the first element on the list */
		Node *last;	/* Pointer to the last element on the list */
		int elements;	/* Current number of elements in the list */
};

/* ------------------------------------------------ */

/***********************************/
/** Implementation of the methods **/
/***********************************/

/* ------------------------------------------------ */


/******************************************/
/** -- Methods to manipulate the list -- **/
/******************************************/

/*
 * Creates a node with value "element" and adds it
 * at the end of the list.
 */
template <class Elem>
void List<Elem>::add (Elem element)
{
	/* Declares and initializes the new node */
	Node *new_node = new Node;

	new_node->value = element;
	new_node->next = 0;

	/* If the list is empty, the value of "first"
	and "last" has to be changed */
	if (is_empty ())
	{
		first = new_node;
		last = new_node;
		/* Increments the number of elements */
		elements++;
	}
	else
	{
		/* Adds the node and moves the pointer "last" */
		last->next = new_node;
		last = new_node;
		/* Increments the number of elements */
		elements++;
	}
}


/*
 * Creates a node with value "element" and adds it on "index" position (if exists).
 *
 *	Return value:
 *		0 on success
 *		error code if the specified index doesn't exists
 *
 *	Errors:
 *		LIST_ERR_INDEX
 */
template <class Elem>
int List<Elem>::add_index (Elem element, int index)
{
	Node *aux = first,
	     *new_node = new Node;
	int i = 0;

	new_node->next = 0;
	new_node->value = element;

	/* If the requested index is not on the limits, returns an error code */
	if ( (index < 0) || (index > elements) )
		return LIST_ERR_INDEX;

	/* If the list is empty, simply adds it to the first position */
	if (is_empty ())
	{
		first = new_node;
		last = new_node;

		elements++;
		return LIST_NOERROR;
	}

	/* Different treatment if index == 0 */
	if (index == 0)
	{
		new_node->next = first;
		first = new_node;

		elements++;
		return LIST_NOERROR;
	}

	/* This loop stops when reaches the element before the desired index */
	while ( (i < (index - 1)) && (aux != NULL) )
	{
		aux = aux->next;
		i++;
	}

	/* Once it finds the requested index, adds the new element */
	new_node->next = aux->next;
	aux->next = new_node;

	/* If the requested index is the last position, moves the pointer "last" */
	if (index == elements)
		last = new_node;

	/* Adds an element to the list's element counter */
	elements++;

	return LIST_NOERROR;
}


/*
 * Deletes, if found, the first node with value "element".
 *
 *	Return value:
 *		Index of the deleted item on success
 *		error code if "element" isn't found
 *
 *	Errors:
 *		LIST_ERR_NOT_FOUND
 */
template <class Elem>
int List<Elem>::del (Elem element)
{
	Node *aux = first,
	     *remove = new Node;
	int index = 0;

	/* Checks the first node */
	if (aux->value == element)
	{
		/* Moves the pointer */
		first = first->next;

		/* Deletes this element */
		aux->next = 0;
		free (aux);

		/* Decrease the number of elements */
		elements--;

		return index;
	}

	while (aux->next != NULL)
	{
		if (aux->next->value == element)
		{
			remove = aux->next;

			if (remove == last)
				/* If it's the las element, moves back the pointer "last" */
				last = aux;

			/* Deletes the selected node  */
			aux->next = remove->next;
			free (remove);

			/* Decrease the number of elements */
			elements--;

			return (index + 1);
		}

		aux = aux->next;
		index++;
	}

	return LIST_ERR_NOT_FOUND;
}


/*
 * Deletes, if found, all the nodes with value "element".
 *
 *	Return value:
 *		Number of elements deleted on success
 *		error code if there's not any node with value "element"
 *
 *	Errors:
 *		LIST_ERR_NOT_FOUND
 */
template <class Elem>
int List<Elem>::del_all (Elem element)
{
	int value = 0; /* Value that has to be returned to the caller */

	/* This loop calls the method "del" until no more elements
	with the desired value are left */
	while ( !LIST_ERROR(del (element)) )
		value++;

	if (value == 0)
		return LIST_ERR_NOT_FOUND;

	return value;
}


/*
 * Deletes, if exists, the node on index "index"
 *
 *	Return value:
 *		LIST_NOERROR on success
 *		error code if the specified index doesn't exists
 *
 *	Errors:
 *		LIST_ERR_INDEX
 */
template <class Elem>
int List<Elem>::del_index (int index)
{
	Node *aux = first,
	     *remove = new Node;
	int i = 0;

	/* If the specified index doesn't exists, returns error */
	if ( (index < 0) || (index > elements) )
		return LIST_ERR_INDEX;

	/* Checks if it's the first element */
	if (index == 0)
	{
		/* Moves the pointer */
		first = first->next;

		/* Deletes this element */
		aux->next = 0;
		free (aux);

		/* Decrease the number of elements */
		elements--;

		return LIST_NOERROR;
	}

	/* Travels around the list until it reaches the desired index */
	while (i < (index - 1))
	{
		aux = aux->next;
		i++;
	}

	/* Deletes the requested node */
	remove = aux->next;

	aux->next = remove->next;
	remove->next = 0;
	elements--;

	free (remove);

	return LIST_NOERROR;
}


/************************************************************/
/** -- Methods to control the current state of the list -- **/
/************************************************************/

/*
 * Returns "true" if the list is empty.
 *
 *	Return value:
 *		true if the list is empty.
 *		false if the list has at least one node.
 */
template <class Elem>
bool List<Elem>::is_empty ()
{
	/* If its empty, both pointers ("first" and "last") should be NULL */
	if ((first == NULL) && (last == NULL))
		return true;
	else
		return false;
}


/*
 * Returns the number of elements on the list.
 *
 *	Return value:
 *		Number of elements.
 */
template <class Elem>
int List<Elem>::size ()
{
	return elements;
}


/*
 * Returns the number of elements on the list (same as size()).
 *
 *	Return value:
 *		Number of elements.
 */
template <class Elem>
int List<Elem>::num_elem ()
{
	return elements;
}


/*
 * Returns the number of occurrencies (nodes) which value is "element".
 *
 *	Return value:
 *		-Number of ocurencies of the element (from 0 to "elements").
 */
template <class Elem>
int List<Elem>::find_occur (Elem element)
{
	int occur = 0;		/* Number of occurrencies */
	Node *aux = first;	/* Pointer to travel around the list */

	while (aux != NULL)
	{
		if (aux->value == element)
			occur++;

		aux = aux->next;
	}

	return occur;
}


/*
 * Returns the index of the first node with value "element",
 * or error if it's not on the list.
 *
 *	Return value:
 *		-Index of the searched element on success.
 *		-LIST_ERR code on error.
 *
 *	Errors:
 *		-LIST_ERR_NOT_FOUND
 */
template <class Elem>
int List<Elem>::find_index (Elem element)
{
	int index = 0;
	Node *aux = first;	/* Pointer to travel around the list */

	while (aux != NULL)
	{
		if (aux->value == element)
			return index;

		aux = aux->next;
		index++;
	}

	return LIST_ERR_NOT_FOUND;
}


/*
 * Returns the element on index "index", if exists.
 *
 *	Return value:
 *		-Index of the required element on success.
 *		-NULL if the index doesn't exists.
 */
template <class Elem>
Elem List<Elem>::get_elem (int index)
{
	int i = 0;
	Node *aux = first;	/* Pointer to travel around the list */

	/* If the specified index doesn't exists,
	returns the proper error code */
	if ( (index >= elements) || (index < 0) )
	{
		Node *aux = NULL;
		return aux->value;
	}

	while (aux != NULL && i < index)
	{
		aux = aux->next;
		i++;
	}

	return aux->value;
}


/*
 * Prints all the information about the list (number of
 * nodes, first element and last element).
 *
 *	Return value:
 *		-String with all the info.
 */
template <class Elem>
string List<Elem>::get_info ()
{
	ostringstream info;

	/* If it's empty, it doesn't print the NULL pointers */
	if (is_empty ())
		/* Adds the requested info to the string */
		info << "\nNumber of elements: " << elements <<
			"\n\"First\" and \"last\" pointers are NULL. \n";
	else
		/* Adds the requested info to the string */
		info << "\nNumber of elements: " << elements <<
			"\n\tFirst element: " << first->value <<
			"\n\tLast element: " << last->value << "\n";

	return info.str ();
}


/*
 * Prints the entire content of the list.
 *
 *	Return value:
 * 		-Number of elements printed on success.
 *		-Generic error code only if the number of elements printed != "elements".
 *
 *	Errors:
 *		-UNKNOWN_ERR
 */
template <class Elem>
int List<Elem>::print ()
{
	/* Pointer that will travel around the list */
	Node *aux = first;
	int n = 0;	/* Number of elements shown (should be equals to "elements") */

	cout << "\n";
	/* In this loop, "aux" will print the value of all nodes, from "first" to "last" */
	while (aux != NULL)
	{
		cout << "\tElement " << n << ": " << aux->value << "\n";
		n++;
		aux = aux->next;
	}

	if (n != elements)
		return UNKNOWN_ERR;
	else
		return n;
}



/*********************************************/
/** ---- CONSTRUCTOR AND DESTRUCTOR ------- **/
/*********************************************/

/* Constructor */
template <class Elem>
List<Elem>::List()
{
	/* Initializes all the atributes */
	first = NULL;
	last = NULL;
	elements = 0;
}

/* Destructor */
template <class Elem>
List<Elem>::~List ()
{
}


#endif
