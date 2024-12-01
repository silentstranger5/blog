+++
title = 'Ideas in Computing'
date = 2024-08-17T11:00:49+03:00
draft = false
+++

If you get a glimpse of the computer science field now, you can probably notice that it is an incredibly high tower of 
abstractions. There are a lot of various fields, many of which are built as abstractions on top of each other. 
It is not easy to study the whole domain in deep detail. But I think that it is still worth to have at least a 
basic understanding. Today, after I discussed how computers work and some basics of development, let us discuss 
a few more advanced ideas. Your processor can do basic arithmetic and logical operations. On top of that, 
there is an assembly to represent those operations in a little more readable form. Some ones and zeroes 
translate onto mnemonic instructions with register addresses and decimal numbers. Finally, some other 
programs translate your human-readable instructions into assembly called compilers. Thus, your conditionals 
and loops become a series of primitive logical operations in assembly. This is a tower of *software* 
abstractions. As it turns out, writing programs is not an easy task. 

Now, imagine yourself somewhere in the eighties. You don't have to write assembly, fortunately enough, but all 
you have is a simple programming language called C. It can not do anything advanced out of the box, yet it's sufficient to express pretty much any idea that your computer can process. As your tasks become more complex, you need to develop effective ways to store and process data efficiently. This is 
where the field of Algorithms and Data Structures comes into place. Computers, in the end, are there to process 
data. That is, to do things like counting, sorting, computing, searching, etc. And the more data you have, 
the more difficult it is to do those things. Computers are very fast today, but that was not always 
the case. Back in the day, you needed to apply clever hacks to process your data fast. Those ideas 
are still relevant today and used to get the best performance in tasks where this is required. Operating systems, databases, compilers, and tons of critical infrastructure rely on algorithms and data 
structures to be fast and efficient. 

The simplest way to store data is an array. It is available in pretty much everything higher than assembly. 
It is as simple as it can be: you allocate a block of memory and then store your values next to each other. 
All you need is the address of your first element, and then every other element can be accessed by the offset 
of this address. After that, an important concept emerges. Values in your memory have an address, this allows your processor to identify the position of values in the memory. What if your memory can store something 
other than numbers interpreted as integers, floating points, or characters? What if you can store the address of 
another place in your memory? This allows you to "jump" from one place in memory into another. This thing is 
called a pointer and is widely used to implement pretty much any data structure at a low level. 

Let's apply this concept to implement another data structure: a linked list. It is not very 
practically useful, but it illustrates the use of pointers nicely. Suppose that you group two fields 
of data. The first of them contains an integer value, and it is akin to a cell of an array. The second field contains 
a pointer, that is, the address of the next record. You "link" your first group of fields to the second one with pointers. This allows you to store your data freely in memory and jump from one place to the next. 
A reason I stated that this structure is not useful is that you can only move forward, and lose the 
arbitrary access you had in array. This makes it slow but allows you to add new records very easily. 
This is how a list node looks like in C:

```
struct list_node {
	int value;		/* integer value */
	struct list_node *next;	/* pointer to the next node */
};
```

The next type of data storage would be a binary search tree. You probably heard about a binary search: we start 
at the middle, and then cut our search range in half each time depending on our current value. A Binary Search tree 
implements this idea in the form of a data structure. We have three fields: the first is an integer value. Two other 
fields are pointers to the left and right tree nodes. The left pointer contains a value lesser than the current one, and 
the right pointer contains a value larger than the current one. Since you cut your search range in half each time you 
hop to the next pointer, you now have a logarithmic performance instead of a linear one. This is how a binary 
tree node looks like in C:

```
struct tree_node {
	int value;			/* integer value */
	struct tree_node *left;		/* left node - less than value */
	struct tree_node *right;	/* right node - more than value */
};
```

The last thing I'd like to discuss is a hash map. The key here is a thing called hash function. Given any value, 
the hash function computes an integer called hash, which denotes a value position at the array. This allows us to 
access our values by their hash function. Hash functions are generally fast, so this allows us to use 
almost instantaneous lookups.
