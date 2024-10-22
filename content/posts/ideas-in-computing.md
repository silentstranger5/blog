+++
title = 'Ideas in Computing'
date = 2024-08-17T11:00:49+03:00
draft = false
+++

If you got a glimpse of computer science field now, you can probably notice that it is incredibly huge tower of 
abstractions. There are a lot of various fields and many of them are built as abstractions on top of each other. 
It is not easy to study the whole domain in deep details. But I think that it is still worth to have at least a 
basic understanding. Today, after I discussed how computers work and some basics of development, let us discuss 
a little more advanced ideas. Your processor can do basic arithmetic and logical operations. On top of that, 
there is an assembly to represent those operations in a little more readable form. A bunch of ones and zeroes 
translate onto mnemonic instructions with register addresses and decimal numbers. Finally, there are some other 
programs that translate your human-readable instructions into assembly called compilers. Thus, your conditionals 
and loops become a series of primitive logical operations in assembly. This is a tower of *software* 
abstractions. As it turns out, writing programs is not an easy task. 

Now, imagine yourself somewhere in the eighties. You don't have to write assembly, fortunately enough, but all 
you have is a very basic programming language called C. It can not do anything advanced out of the box, but it 
is sufficient to express pretty much any idea that your computer is able to process. As your tasks become 
increasingly complex, you need to come up with good ideas on how to store and process data efficiently. This is 
where a field of Algorithms and Data Structures comes into place. Computers, at the end, are there to process 
data. That is, to do things like counting, sorting, computing, search, and so on. And the more data you have, 
the more difficult it is to do those things. Computers are ridiculously powerful today, but that was not always 
the case. Back in the day, you needed to apply really clever hacks to process your data fast. All of those ideas 
are still relevant today, and used to get the best performance in tasks where this is required. Things like 
operating systems, data bases, compilers, and tons of critical infrastructure rely on algorithms and data 
structures to be fast and efficient. 

The most simple way to store data is array. It is available in pretty much everything higher than assembly. 
It is as simple as it can be: you just allocate a block of memory, and then store your values next to each other. 
All that you need is just address of your first element, and then every other element can be accessed by offset 
of this address. After that, an important concept emerges. Values in your memory have address, and this address 
allows your processor to identify position of values in the memory. Now, what if your memory can store something 
other than numbers interpreted as integers, floating points, or characters? What if you can store address of 
another place in your memory? This allows you to "jump" from one place in memory into another. This thing is 
called pointer and is widely used to implement pretty much any data structure in low level. 

Now, let's apply this concept to implement the next simple data structure: a linked list. It is not very 
practically useful, but illustrates a use of pointers really nicely. Suppose that you group together two fields 
of data. First of them contains an integer value, and it is akin a cell of an array. The second field contains 
a pointer, that is, an address of the next record. You "link" your first group of fields to the second one using 
those pointers. This allows you to store your data freely in memory and jump from one place to the next one. 
The reason why I said that it's not a really useful structure is that you can only move forward, and lose your 
arbitrary access you had in array. This makes it quite slow, but allows you to add new record very easily. 
This is how a list node looks like in C:

```
struct list_node {
	int value;		/* integer value */
	struct list_node *next;	/* pointer to the next node */
};
```

The next type of data storage would be a binary search tree. You probably heard about a binary search: we start 
at the middle, and then cut our search range in half each time depending on our current value. Binary Search tree 
implements this idea in form of a data structure. We have three fields: first is an integer value. Two other 
fields are pointers to left and right tree node. Left pointer contains value lesser than a current one, and 
right pointer contains value larger than a current one. Since you cut your search range in half each time you 
hop to the next pointer, you now have a logarithmic performance instead of a linear one. This is how a binary 
tree node looks like in C:

```
struct tree_node {
	int value;			/* integer value */
	struct tree_node *left;		/* left node - less than value */
	struct tree_node *right;	/* right node - more than value */
};
```

The last thing I'd like to discuss is hash map. The key here is a thing called hash function. Given any value, 
hash function computes an integer called hash, which denotes a value position at the array. This allows to 
access our values by their hash function. Hash functions are generally really fast, so this allows us to use 
almost instantaneous lookup.
