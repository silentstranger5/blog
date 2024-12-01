+++
title = 'Computers'
date = 2024-08-17T09:01:43+03:00
draft = false
+++

Before diving into the development, it probably would be worth contemplating what a computer truly is. Computers are incredibly complicated machines performing complex tasks. There is no way that 
anyone could give a brief and comprehensive explanation of what this all is about. From a closer look, computing 
is a hell of complexity: design and time complexity, computational complexity... As most  
readers are presumably familiar with the idea of computers, there is no point in overly brief and simple 
explanations. Of course, it would be impossible to cover even basics with just one blog post, because computer 
architecture is a vast topic. Nevertheless, I don't think it is the most important thing you should 
worry about: photographers have little to no idea how the camera works. But for the sake of general education, 
let's take a brief look at the computers that we use every day.

Before I begin describing what computers are, I would like to share some resources on the topic.

- The first one is a [Khan Academy course about computers and internet](https://www.khanacademy.org/computing/computers-and-internet)
- The second one is a [Crash Course on Computer Science](https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo)
- If you are a hopeless nerd beyond any help, check out a book called Computer Systems: A Programmer's Perspective

## Basics of computation

I believe you've heard this many times before, so I'll keep it brief. If I would describe what computers are, 
I would say that they are calculators on steroids (Actually, the opposite is technically more correct here: 
Calculators are just dumb computers). The baseline is that computers are all about numbers. At the 
lowest level, nothing that you are familiar with exists: the text you are reading, the color of your 
Wallpaper, the size and look of font you are reading now, the position of your cursor, the speed of movement of this cursor, 
the sound of notifications... I could continue for a long time, but the point is: *all* of that is just a bunch of 
numbers. We will uncover that mystery a little later. For now, let's figure out how to represent numbers 
and what technology is at the core of computers.

## Numbers

People have known about numbers since the dawn of time. After hunt or berry collection, you'd like to divide the food 
equally so that nobody starves to death. That requires counting and arithmetic. The first idea you could consider is using a unit to express the concept of 'one'. Your fingers, stones on the river bank, knots 
on the rope, pieces of wood... You got me. By the way, fingers are the most convenient, because they are always 
with you. After that, the next step would be to use something else to express a group of multiple things, like 
5 or 10. Babylonians counted by 60 for some reason. That is quite weird from today's point of view, so let's 
ignore this fact. Egyptians drew some funny pictures, but not much interesting here. Greeks 
used letters to express numbers. That's a clever hack, but that's still not quite it. Romans used something that 
resembled numbers. Useless fact about five in the Roman system: it looks like a form of palm if you put all 
your fingers together and then put your thumb aside. Now, make the same shape with your other palm, rotate it so 
your fingers are directed down, and put your palms together. This funny gesture features all of your fingers 
(in a weird position), and it's shaped like X. Here is your ten. Romans are the founders of the European civilization, 
so why does nobody use their system? The answer is quite simple: it's hard to do arithmetic with it. 
Numbers are expressed in arithmetic operations, so dealing with them is quite a pain. 
People from India decided to use both hands to give each number a distinct digit. 
Another innovation was to use the order of those digits to express a group of units. Thus, Indian digits 
(called Arabic by the name of importers) and the decimal system was born.

## Computer Technology

Three centuries before computers, the world was set on scientific revolution: the brightest minds of history 
solved complex physical problems about planetary motion, gravitation, body movement, heat, pressure, and many 
other things. Deep and complex mathematical tools were invented to assist in those tasks: functions, derivatives, 
integrals, limits, logarithms, vectors, coordinate space, and many more. But for computers to emerge, two pieces 
were still missing, and both appeared only in the nineteenth century: Boolean logic to process binary states, and 
electricity to express such states in the real world. Why can we only use binary states with electricity instead 
of familiar decimals? I think you know the answer: it's hard to finely control voltage such that its 
magnitude is distinctive by ten states. The binary state is quite simple: you've got power, 
or you don't. I don't want to waste time explaining how binary states can be used to express numbers. Think about 
it: we use ten as the base of our decimal system (4 * 10 + 2 * 1), so we have ten digits whose positions are denoted 
by powers of ten (1, 10, 100, ...). Now substitute 10 with 2 and you've got (101010 = 1 * 32 + 0 * 16 + 1 * 
8 + ...) where the base is two, there are two digits (0 and 1), and positions are denoted by the power of two (1, 2, 4, 
8...) Not convenient for people to use, but now you can count numbers using some wires with 
(or without) electrical current.

## Transistors 

It's all nice, of course, but you can't do anything with it. You can't just combine voltages or 
arrange wires like you arrange digits on a piece of paper. You have to use logic to do anything useful. This 
is where the binary logical system, called Boolean after its creator, George Bool, comes into place. Using 
this logic, you can perform all those operations we know about disjunction, conjunction, negation, implication, 
etc. But how can you do it with just a bunch of wires? For that, you need the last piece of technology 
called a diode. The diode has three points of contact: input, where current comes from; output, 
where current goes to; and the last piece that I call switch: if you apply current here, current flows from input to 
output with no problem. Otherwise, it does not. The first diodes were mechanical, slow, loud, and hot. After them, 
there were vacuum lamps. Finally, using some advanced physics magic, semiconductors were invented, and 
transistors emerged. Those are the basic pieces used to build everything else in the computer.

## Logic gates

After we have transistors, we can combine and arrange them in some peculiar fashion to create 
something else. Those things are called logical gates, and they carry out primitive logical operations described 
by Boolean logic. I won't go into details since it's hard to do without pictures and I'm too lazy for that. 
You can check out the materials that I provided above for details. Those logic gates wrap combinations of 
transistors into some black box. We use that box to describe units behaving according to the Boolean logic. 
We don't care about what is inside such boxes, and use them to build something even more complex. 
This process of "wrapping" is called abstraction, and will be used many times. 

## Arithmetic

After we can perform logical operations on a couple of inputs and get the result as an output, we can use that to 
build our final piece: the arithmetic unit. A fundamental part of the arithmetic unit is the adder. Again, I'm not going deep 
into details, and you can check the materials above if you are curious. Combining a bunch of logical gates, we have 
a half adder, then a full adder, and then we combine all that into a ripple-carry adder. Based on adder we can do 
subtraction, which uses properties of binary number representation. Finally, using more effort, we can 
perform a multiplication algorithm, which is based on addition. The division is not implemented so easily, 
so we won't mention it here. Again, you can check out all the details on the internet if you are curious.

## Interpretation

In the beginning, I told you that everything you see on your computer is just a bunch of numbers. How does that 
work? Well, simple. Let's start with the text. That was the first thing people processed with computers 
after numbers. How can text be represented as numbers? First, let's assign a number to each letter in 
the alphabet. Then, the text would be just a sequence of numbers. The most widely used text encoding is called 
ASCII. The letter 'A' here is encoded as 65. It seems weird until you look at the binary: 1000001. The first 64 numbers 
are assigned to some magical characters, like space, tab, backspace, enter, end of input, etc. Other 64 
Numbers are assigned to things that you can type and see. Now, what about images? Well, if you look at 
your monitor closely, you can see that the picture you are looking at consists of many little dots called 
pixels. Each pixel has its color represented by three simple colors: red, green, and blue. This system is 
called RGB, and each number can have a value from 0 (black) to 255 (most bright colors). A combination of those simple
colors can produce pretty much any color you see on your monitor. Sound is also encoded as a sequence of 
numbers. More precisely, those numbers denote the amplitude or volume of the sound wave. Sound processing involves 
discretization because sound is analogous, and digital information is discrete. Again, I encourage you to check 
out the materials above for the details.

## Conclusion

That's... about it? I'm not going to mention details of the implementation of everything your processor can 
perform, but I suppose you've got the gist of it. To perform bitwise logical operations, you apply such 
operation on a combination of corresponding bits from each input number. Combined with arithmetic, this makes 
up the majority of primitive and fundamental operations carried out by your processing 
unit. Of course, there are more details, like registers, program counter, cache, operating memory, and *a lot* 
of optimization magic. But provided information covers the basics.

