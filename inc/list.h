#ifndef __LIST_H__
#define __LIST_H__

template <class T> class list
{
	struct link
	{
		T *item;
		link *next;
	};

	link *head;
	link *current;

public:

	list();
	~list();
	void insert(T *);
	T *first();
	T *next();
	T *search(int (*comp)(const void *));
};

#endif //__LIST_H__
