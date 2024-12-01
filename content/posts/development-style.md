+++
title = 'Development Style'
date = 2024-09-24T18:02:18+03:00
draft = false
+++

There are generally two approaches to development which contrast each other. 
Imperative style describes how to do things in an orderly fashion. The declarative style describes what needs to be done. 
The low-level style is called the procedural development style, and the high-level style is called the object-oriented paradigm. 
Procedural style is essentially constrained by structures and procedures (hence the name). 
The procedure is just another term used for function. The object-oriented paradigm is centered around objects. 
Objects couple structures and functions operating on them together. This bundle is called an object. 
I will discuss both of those styles in more detail. But before I do that, let us touch on the functional style.

## Functional style

A functional style is centered around functions. But that does not mean your everyday 
staple function. If that were the case, this style would be no different than the procedural style. 
In this context of functions, I mean a mathematical function, which is a relation between inputs and outputs. 
Other than that, functions should be "pure": their value must be deterministic, and 
they should only rely on function inputs. This means you have to supply all inputs as arguments, 
or to construct deep hierarchies of peculiar functions. Functions in this style are considered 
"first-class citizens". This means that you can do function anything that you can do with data: 
Pass it as an argument, assign it to a variable, and return it as a function value.

The functional style originates from lisp. This peculiar "list processing language" differs from most other languages. Anything you could think of is achieved with two fundamental 
concepts: recursion and lists. We use recursion for iteration and lists to store data. Recursion allows to call the same function with modified arguments 
which is often used for iteration. A list is a chain of pairs. 
The list node consists of two elements: a node value, and a pointer to the remaining list. It is surprising 
how much it is possible to do using just that. The most canonical example is a Fibonacci sequence:

```
(define (fib n)
  (fib-iter 0 1 0 n))

(define (fib-iter a b c d)
  (if (= c d) a
    (fib-iter b (+ a b) (+ c 1) d)))
```

Another example is a factorial.

```
(define (fact n)
  (fact-iter 1 1 n))

(define (fact-iter a b c)
  (if (= b c) a
    (fact-iter (* a b) (+ b 1) c)))
```

This style differs quite much compared to the imperative approach. Even though it is not widely accepted, 
supporters of this style argue that it encourages decomposition and simplifies testing and debugging.

## Procedural style

Probably the most notorious example of procedural style is the C programming language. It is a thin and 
simple abstraction over assembly language. Nevertheless, it provides enough balance between convenient 
abstractions and performance. It also exposes such unique mechanics as pointers and runtime memory allocation. 
I find this approach very appealing, even though difficult 
sometimes. For me, it is not hard to convert 
between procedural and object-oriented styles:

```
/* object-oriented style */
foo_object.method(argument);

/* procedural style */
foo_method(struct foo* foo_ptr, argument);
```

Some of you may argue that the replacement of an underscore with a dot is not what this style is about. Indeed, 
one of the selling points of the object-oriented approach is so-called information hiding, that is, access 
restriction for some of the variables that represent sensitive states. Surprisingly, my take on that 
originates from the Python approach to information hiding. In Python, there is no private field 
enforcement from the language itself. Instead, it is based purely on the convention: private fields are 
stylistically denoted with leading underscore. What about inheritance? Well, you can easily embed 
structures in one another:

```
struct foo { ... };

struct bar {
    struct foo* foo_ptr;
    ...
};
```

The same is true for methods:

```
bar_method(struct bar* bar_ptr, args...) {
    foo_method(bar.foo_ptr, args...);
    ...
}
```

While those hierarchies are not built out of the box, I think it solves the problem. 
What about abstract classes? Well, let's try something like this.

```
enum {FOO_FIRST, FOO_SECOND};

typedef struct {
    int type;
    ...
} foo_first;

typedef struct {
    int type;
    ...
} foo_second;

foo_first_method(foo_first* foo_ptr);
foo_second_method(foo_second* foo_ptr);

foo_method(void* foo_ptr) {
    int type = *(int *)(foo_ptr);
    switch type {
    case FOO_FIRST:
        foo_first_method((foo_first *)(foo_ptr));
        ...
    case FOO_SECOND:
        foo_second_method((foo_second *)(foo_ptr));
        ...
    }
}

int main(void) {
    foo_first first = {
        FOO_FIRST, ...
    };
    foo_second second = {
        FOO_SECOND, ...
    };
    foo_method(&first);
    foo_method(&second);
    ...
}

```

Well, it works. But it is cumbersome and a little silly. Generally, I don't see an idea of 
virtual function being used widely in C. Much more often I encounter specific methods for corresponding 
types (for example, `duck. quack()` instead of `animal. speak(duck)`).

C is often accused of having too small a standard library. And while it is true to a certain extent, some may 
argue that nothing prevents you from writing these yourself. If you need something, you can always find libraries online for any purpose you can think of. Not only low-level stuff, 
but also things like graphics API, image processing, or GUI stuff. But of course, the primary area of C is 
low-level things like drivers, operating systems, databases, servers, and so on. Flexibility and low-level access 
is crucial in those areas.

## Object Oriented Paradigm

Probably, it is one of the most widely used styles in development at the moment. Some of the most popular 
programming languages use it as the only paradigm available (like Java or C#) and many more support it.
There is certainly a convenience in binding data and functions operating on it together. Object hierarchy 
can arise naturally from many areas, like Graphical User Interfaces, Games, and many instances of complex 
or 'object-oriented' software in general. Sometimes this certainly feels redundant, though. A lot of software 
can be perfectly written in a procedural style. Besides, probably the most notorious critique of object-oriented 
approach is that it introduces a coupling and restriction on a fundamental level. Class hierarchies are very 
brittle to change. Also, much of object-oriented code (say in Java) feels verbose and mouthful.
Nevertheless, the Object-Oriented approach (despite being heavily criticized by a fair number of respected computer 
scientists), remains widely popular today.
