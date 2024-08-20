+++
title = 'About Tools'
date = 2024-08-19T18:06:20+03:00
draft = false
+++

Development presents us with a significant tradeoff which deeply affects both our tools and nature of our code. 
That is a tradeoff between a speed of development and speed of execution. Fastest programs are the closest to 
the machine, and most convenient programs require a number of abstractions, indirections and execution overhead. 
The most known low-level language (besides assembly, which is just a thin wrapper) is C. There are some more 
new languages in this field, like Rust or Zig. I do not mention C++, because it accomplishes an amazing 
achievement of being both human-unreadable and machine-unreadable. Most people enter the world of development 
from high-level dynamically-typed languages, that take care of almost anything you could think of. It makes 
those languages, like Python or JavaScript, very easy to learn. One of tradeoffs here is that those languages 
have poor execution speed due to the fact that they provide a lot of convenience like dynamic typing and 
automatic memory management. Another drawback is that those languages often scale poorly due to their dynamic 
typing nature, because it can be hard to work with data when you are not sure about it's type.

My opinion on this matter is that I prefer simplicity and flexibility. I don't like C++ because it's really 
hard to work with, and I don't like Rust because it both differs significantly from anything else, and again, 
it's not easy to work with. That could make me a mediocre developer, because a lot of awesome stuff is written 
on those languages. I also do not like restricting languages, say Java. OOP is a nice paradigm, but enforcing 
me to work in a specific paradigm, (which is not *always* the best tool for the job, and has a lot of complexity 
just to overcome it's limitations), is certainly not giving me a good impression. Again, a lot of people work 
in this language, and it's really easy to find a job with them, and I am quite familiar with OOP at least at the 
basic level, but I think that I can live without classes *everywhere*. 

From time to time, I like to code something in C. I think that the reason is that C provides just enough of what 
you need. It makes development fun, because you have to figure out how to provide necessary data structures with 
required functionality, and you have to think about what you are doing as well. There are some annoying things 
related to heap management, but other than that, it's not bad. On the other hand, it's also convenient enough 
not to think about registers, stack management, and grind all of those boring details. It's nice spot in the 
middle. But, of course, I think that many things depend on where in the abstraction level exists the thing you 
are trying to develop. 
I probably would not implement advanced high-level thing like browser or videogame in something like C, because 
at this 
point, it's minimalism becomes a significant flaw. You *can* do it, but it just takes quite effort and time.

My path looks like this: I started with high-level Python. It is easy to learn, but that may pose a problem 
because it might be hard to deal with statically-typed languages afterwards. After that, I learned C. And I 
think that C is 
quite nice language to learn. I wouldn't think that you will use it on the daily basis, but it has couple 
benefits. First one is that C is a lingua franca of development. Pretty much any highly-used language today is 
very similar to C. It is akin to the Latin language: it's not spoken anywhere today, but by learning it, you 
would probably understand any 
language from Roman family much better. The second benefit of learning that language is that it provides you with 
better understanding of what is actually going on, and how thing are taken care of under the hood.

So, we have a high-level languages, like Python and JavaScript, and low-level languages like C. Is there 
something in the middle? As it turns out, yes, there is. A language called Go combines static typing and 
relatively fast execution speed with high readability and usability like Python. It's not as fast as low-level 
stuff because it manages memory automatically, and it's not as simple as Python because it's statically typed, 
but it hits this spot in the middle, which was not covered for quite some time. I often use Golang on my 
personal projects, and it's fun to work with. 

Many people say that tools do not matter, and a lot of it is about syntax. There is a certain truth to it, but 
it is still somewhat wrong. Human languages convey roughly the same information, no matter which language in 
particular you use. But devil is often hidden in the details. Many studies found out that using different 
language often impacts our way of thinking, because different language cover different details and use different 
way of expressing information. It is also certainly true with programming languages. Depending on which language 
you use, you think about different things and different ways of expressing and implementing those things. 
Some languages are close to each other, other languages could not be more different.
