+++
title = 'Computers'
date = 2024-08-17T09:01:43+03:00
draft = false
+++

Before diving into the world of development, it probably would be worth to take a brief look at what computer 
even is. Computers are incredibly complicated machines performing fairly complex tasks. There is no way that 
anyone could give a brief and comprehensive explanation of what this all is about. From a closer look, computing 
is a hell of complexity: design complexity, time complexity, computational complexity... Well, as most of 
readers are presumably familiar with the notion of computers, there is no point in overly brief and simple 
explanation. Of course, it would be impossible to cover even basics with just one post, because computer 
architecture is a very vast topic. Nevertheless, I don't think that it is the most important thing you should 
worry about: photographers have little to no idea about how camera works. But for the sake of general education, 
let's take a brief look on computers that we use every day.

Before I start to describe what computers are, I would like to share some resources on topic.

- The first one is a [Khan Academy course about computers and internet](https://www.khanacademy.org/computing/computers-and-internet)
- The second one is a [Crash Course on Computer Science](https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo)
- If you are a hopeless nerd beyond any help, check out book called Computer Systems: A Programmer's Perspective

## Basics of computation

I think that you've heard that a lot of times, so I would be brief. If I would describe what computers are, 
I would say that they are calculators on steroids (Actually, the opposite is technically more correct here: 
calculators are just really dumb computers). The base line is that computers are all about numbers. At the 
lowest level, nothing that you are familiar with, just does not exist: text you are reading, color of your 
wallpaper, size and look of font you are reading now, position of your cursor, speed of movement of this cursor, 
sound of notifications... I could continue for a long time, but the point is: *all* of that is just a bunch of 
numbers. We will uncover that mystery a little bit later, but for now, let's figure out how to represent numbers 
and what technology is at the core of computers.

## Numbers

People know about numbers since the dawn of times. After hunt or berry collection, you'd like to divide the food 
equally so that nobody starves to death. That requires counting and arithmetic. First idea you could come up 
with is to use some sort of unit to express the notion of 'one'. Your fingers, stones on the river bank, knots 
on the rope, pieces of wood... You got me. By the way, fingers are the most convenient, because they are always 
with you. After that, the next step would be to use something else to express a groups of multiple things, like 
5 or 10. Babylonians counted by 60 for some reason. That is quite weird from today's point of view, so let's 
just ignore this fact. Egyptians drew some funny pictures, but there is also not much interesting here. Greeks 
used letters to express numbers. That's a clever hack, but that's still not quite it. Romans used something that 
actually resembled numbers. Useless fact about five in roman system: it looks like a form of palm if you put all 
fingers together and then put your thumb aside. Now, make the same shape with your other palm, rotate it so that 
your fingers are directed down, and put your palms together. This funny gesture features all of your fingers 
(in a weird position), and it's shaped like X. Here is your ten. Romans are founders of the Europe civilization, 
so why does nobody use their system? Answer is quite simple: it's inconvenient to do arithmetic with them. 
Numbers themself are expressed in terms of arithmetic operations, so dealing with them is a pain in the butt. 
People from India decided to use their both hand at the same time, and to give each number a distinct digit. 
Another innovation was to use order of those digits to express a group of units. Thus, Indian digits 
(called Arabic by the name of importers) and decimal system were born.

## Computer Technology

Three centuries before computers, world already was set on scientific revolution: brightest minds of history 
solved complex physical problems about planetary motion, gravitation, body movement, heat, pressure, and many 
other things. Deep and complex mathematical tools were invented to assist in such task: functions, derivatives, 
integrals, limits, logarithms, vectors, coordinate space, and many more. But for computers to emerge, two pieces 
were still missing, and both appeared only at the nineteenth century: Boolean logic to process binary state, and 
electricity to express such state in the real world. Why we can only use binary state with electricity instead 
of familiar decimal? I think you know the answer: it's really hard to finely control voltage such that it's 
magnitude is clearly distinctive by ten states. Binary state, on contrary, is really simple: you've got power, 
or you don't. I don't want to waste time explaining how binary state can be used to express numbers. Think about 
it: we use ten as base of our decimal system (4 * 10 + 2 * 1), so we have ten digits whose positions are denoted 
by powers of ten (1, 10, 100, ...). Now just substitute 10 with 2 and you've got (101010 = 1 * 32 + 0 * 16 + 1 * 
8 + ...) where base is two, there are two digits (0 and 1) and positions are denoted by power of two (1, 2, 4, 
8...) Not really convenient for people to use, but now you can count numbers using a bunch of wires with 
(or without) electrical current.

## Transistors 

All of that is nice, of course, but you can't do anything with it really. You can't just combine voltages or 
arrange wires like you arrange digits on the piece of paper. You have to use logic to do anything useful. This 
is where binary logical system, called Boolean by name of it's creator, George Bool, comes into place. Using 
this logic, you can perform all those operations we know about: disjunction, conjunction, negation, implication, 
etc. But how can you do it with just a bunch of wires? Well, for that, you need the last piece of technology 
called diode. To put it simple, diode has three points of contact: input, where current comes from; output, 
where current goes to; and last piece that I call switch: if you apply current here, current flows from input to 
output with no problem. Otherwise, it does not. First diodes were mechanical, slow, loud and hot. After them, 
there were a vacuum lamps. Finally, using some advanced physics magic, semiconductors were invented, and 
transistors emerged. Those are the simplest pieces used to build everything else in the computer.

## Logic gates

After we have transistors in place, we now can combine and arrange them in some peculiar fashion to create 
something else. Those things are called logical gates, and they carry out primitive logical operations described 
by Boolean logic. I won't go into details since it's hard to do without pictures and I'm too lazy for that. 
You can check out materials that I provided above for details. Those logic gates wrap combinations of 
transistors into some black box. We use that box to describe unit behaving according to the Boolean logic. 
We don't really care about what is inside of it, and use them to build something even more complex. 
This process of "wrapping" is called abstraction, and will be used many times yet. 

## Arithmetic

After we can perform logical operation on couple of inputs and get the result as an output, we can use that to 
build our final piece: arithmetic unit. Fundamental part of arithmetic unit is adder. Again, I'm not going deep 
into details, and you can check materials above if you are curious. Combining a bunch of logical gates, we have 
a half adder, then a full adder, and then we combine all that into ripple-carry adder. Based on adder we can do 
subtraction, which uses properties of binary number representation. Finally, using some more effort, we can 
perform multiplication algorithm, which is based on addition. Division, though, is not implemented so easily, 
so we won't mention it here. Again, you can check out all of the details on the internet if you are curious.

## Interpretation

In the beginning, I told you that everything you see on your computer is just a bunch of numbers. How does that 
work? Well, simple, really. Let's start with the text. That was the first thing people processed with computers 
after numbers. How text could be represented as numbers? Well, first, let's assign a number to each letter on 
the alphabet. Then, text would be just a sequence of numbers. The most widely used text encoding is called 
ASCII. Letter 'A' here is encoded as 65. It seems weird until you look at the binary: 1000001. First 64 numbers 
are assigned to some magical characters, like space, tab, backspace, enter, end of input and so on. Other 64 
numbers are assigned to things that you can actually type and see. Now, what about image? Well, if you look at 
your monitor really closely, you can find that a picture that you see consists of a lot of little dots called 
pixels. Each pixel has its own color represented with three basic colors: red, green, and blue. This system is 
called RGB, and each number can have value from 0 (black) to 255 (most bright color). Combination of those basic 
colors can produce pretty much any color you see on your monitor. Sound is also encoded as a sequence of 
numbers. More precisely, those numbers denote amplitude, or volume of the sound wave. Sound processing involves 
discretization because sound is analogous, and digital information is discrete. Again, I encourage you to check 
out materials provided above for the details.

## Conclusion

That's... about it? I'm not going to mention details of implementation of every thing that your processor can 
perform, but I suppose that you've got the gist of it. To perform bitwise logical operations, you apply such 
operation on combination of corresponding bits from each input number. This, combined with arithmetic, makes 
up pretty much majority of what there is to primitive and fundamental operations carried out by your processing 
unit. Of course, there are more details, like registers, program counter, cache, operating memory, and *a lot* 
of optimization magic. But provided information covers the basics.
