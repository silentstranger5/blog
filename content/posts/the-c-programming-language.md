+++
date = '2025-01-21T13:18:53+03:00'
draft = false
title = 'The C Programming Language'
+++

### C Language

For some unknown reason, I often find myself drawn to C. It's hard for me to tell exactly why... Above all, I think that the most important reason is the simplicity of this language. It is fairly small and can be learned relatively fast. It is also very close to assembly with very close mapping between C code and assembly instructions.

Do not be fooled by this simplicity, though, reader. Even if the language and standard library are pretty minimal and even if they are close enough to primitive instructions, this language is not simple to use.

Is it not a contradiction? The language itself is simple, but this simplicity means everything must be built from scratch. The language does not provide abstractions or convenient containers with algorithms, like lists, maps, or sets. Object-oriented mechanisms can not be found here either.

Perhaps, the most important of all, memory can be managed indirectly using *pointers* that represent addresses with memory. This enables functions to modify values out of their scope. Your typical variables and arrays are allocated on a memory region called *stack*, whereas another type of memory can be dynamically allocated using *heap*.

### Hello World

I am not going to dive into those topics, at least not for now. "The C Programming Language" by B. Kernighan and D. Richie is the legendary book on the topic, perhaps the most famous book on programming in history. What I intend on doing today can be expressed using a quote from this book:

> The only way to learn a new programming language is by writing programs in it. The first 
program to write is the same for all languages: 
*Print the words*
`hello, world` 
This is a big hurdle; to leap over it you have to be able to create the program text somewhere, 
compile it successfully, load it, run it, and find out where your output went. With these 
mechanical details mastered, everything else is comparatively easy. 

Let us now do just that. The source code of the C language is stored in files with the `.c` extension. Create this file somewhere:

```bash
mkdir hello
cd hello
touch hello.c
```

Now use your text editor to paste this code into `hello.c`:

```c
#include <stdio.h>

int main(void) 
{
    printf("hello, world\n");
    return 0;
} 
```

### About compiler

Here comes the trickiest part. To create the program, you need a software called *compiler*. The *source code* of the program you see above is designed to be *human-readable*. Yes, it's tricky to read this cryptic language, but it's still possible.

However, computers can't understand this source code. To execute the code, it would need something more like that:

```asm
.TEXT:
    .string "hello world"
main:
    push bp
    mov rbp, rsp
    mov edi, OFFSET FLAT:.TEXT
    mov eax, 0
    call printf
    mov eax, 0
    pop rbp
    ret
```

This is *assembly*, low-level code representation, where each line maps roughly to one instruction of CPU. Computer actually can't understand this language either, but it's much closer to what a computer is doing. This form is still designed to be human-readable with each instruction having a mnemonic name (like `mov` for "move") instead of binary numbers. I admit though that reading assembly is harder compared to more *high-level* source code.

What computer understands is something like this:

```bin
55 
48 89 e5 
bf 00 00 00 00
b8 00 00 00 00
e8 00 00 00 00
b8 00 00 00 00
5d
c3
```

This is a *binary file* that contains famous ones and zeros. Note that the actual file does not contain newlines after each command, this is formatting for convenience. You see much more than just ones and zeroes because this file is represented using convenient *hexadecimal notation* where each digit can take any value from 0 to 15. Data here is indeed just a bunch of ones and zeros that the CPU knows how to deal with.

### Source code

Let's cover source code line by line.

To understand what the first line does, we need to take a detour. We will come back to it a bit later. Let's move further.

```c
int main(void)
```

This line tells us that we are about to define a function called `main`. This function accepts no arguments and returns an integer.

The function `main` is special: it must be present in any C program. Execution of a program starts from this function. More specifically, there is a hidden *environment* that initializes some state behind the scenes and then calls `main()`. This environment, called *C runtime* is linked to every C program.

Why this function returns an integer? There is a certain convention, especially prevalent in UNIX systems, where each program returns a value upon completion. This value indicates whether the program terminated normally, or there was some error. We'll discuss it in more detail later.

At the next line you can see a curly brace `{`. This brace indicates that we are now entering a `block`, which means that all statements inside curly braces shall be executed one after another.

The next line calls a function called `printf`. This function performs *formatted printing*. It is sufficient to note that the symbol `\n` denotes a newline character. If it is omitted, your program will not transfer to the next line after it is done printing.

Notice that we do not define `printf` ourselves. Instead, this function is defined inside a *standard library*, which is a collection of useful functions and definitions that may be useful in your programs. The standard library is usually supplied with a compiler. It is implicitly linked to all of your programs.

The last line before closing the curly brace tells that function `main` returns 0. As I mentioned above there is a convention, especially in UNIX systems where programs return value indicating whether the program succeeded or failed. Zero usually means that there is no problem. Any non-zero value means that our program failed during completion and terminated.

### Header files

Now let's go back to the first line. To put it very simply, it tells our compiler that `printf` is a function that accepts a string along with an arbitrary number of additional arguments and returns nothing. Look at this line: `#include <stdio.h>`. To understand what it means, let's consider a couple more examples. Let's say that you have a file like this called `main.c`:

```c
int main(void) {
    foo();
}
```

If you try to compile something like that, you will likely get an error. This error may look somewhat like this:

```
unresolved external symbol foo referenced in function main
```

What is happening here? Well, let's try to think about it. We are trying to call some function called `foo`. But if you look around, this function is nowhere to be found. Compiler (more specifically, linker) complains that it does not understand what `foo` means. Now let's say that we have another file called `foo.c`:

```c
#include <stdio.h>
void foo() {
    printf("foo\n");
}
```

You can now in theory make this program work if you compile both of those files and link them together. I am going to cover compiler mechanics in just a minute. Let's pretend that you know how to do it. If you try to do it... it does indeed compile. But there is a certain subtlety to it.

When you compile `main.c` without linking, you'll see a warning like this:

```
warning: implicit declaration of function ‘foo’
```

Well, that's weird. We haven't told the compiler anything about `foo`. How exactly can it make an implicit declaration? Here is another quote from the book:

>If a name that has not been previously declared occurs in an expression and is followed by a 
left parenthesis, it is declared by context to be a function name, the function is assumed to 
return an int, and nothing is assumed about its arguments. Furthermore, if a function 
declaration does not include arguments, as in 
`double atof();` 
that too is taken to mean that nothing is to be assumed about the arguments of `atof`; all 
parameter checking is turned off. This special meaning of the empty argument list is intended 
to permit older C programs to compile with new compilers. But it's a bad idea to use it with 
new C programs. If the function takes arguments, declare them; if it takes no arguments, use 
`void`.

Well, what is this *declaration* and what does it look like? The declaration is a statement that tells the compiler the type of variable (or type of return value and arguments in a function). Here is how our file looks with a declaration:

```c
void foo(void);

int main(void) {
    foo();
}
```

Notice the first line. It tells to compiler that function `foo` does not accept any arguments, neither does it return any value. If you compile this file without linking now, all errors go away.

As you can see, we use declarations to specify *interfaces* - to specify compiler types of variables and functions. Often one file may contain hundreds of functions you may want to use. Typing a hundred function declarations line by line when you want to use functions from your file in your program is not a great idea.

This is the reason why *header files* were created. A typical header file has a `.h` extension and consists of various declarations and definitions. It may look somewhat like this:

```c
void foo(void);
int bar(int, int);
float baz(float);
...
```

Let's say that we have a header like this, called `foo.h`. How do we use it in our source code? We type this:

```c
...
#include "foo.h"

void foo()
...
```

Notice that this line starts with a hash sign `#`. Lines that start with a hash sign are handled by *preprocessor*, part of the compiler that performs source code text manipulation before compilation.

More specifically, `#include` means that the preprocessor has to copy all contents of another file into our source code. After the preprocessor is done, our source code will look like this:

```c
...
void foo(void);
int bar(int, int);
float baz(float);
...

void foo(void)
...
```

This code won't compile because functions `bar` and `baz` are not defined. But I am just showing it here to make a point.

### How to compile

Maybe it's weird that I've put the most important part at the end, but I wanted to pay attention to the source code before compiling it. Unfortunately, obtaining a C compiler is trickier compared to something like Python or Go which you can just download and it works.

Your compiler generally depends on your operating system. 

### UNIX notes

If you are using UNIX-like systems (like Linux or MacOS), you'll need `gcc` - GNU Compiler Collection or any other UNIX compiler.

Most UNIX-like systems have package managers. On Debian-like systems, for example, you can install a compiler like this:

```bash
apt install gcc
```

Although, it's probably best to install all useful tools for building like this:

```bash
apt install build-essential
```

It's fairly similar on Mac:

```bash
brew install gcc
```

Compiling on UNIX-like systems is easy:

```bash
gcc hello.c
```

Some systems have an alias for compiler called `cc` which stands for `C Compiler`. This alias is from the original UNIX systems, where it was the name of the compiler program. It may or may not be available on your system, but I intend to use it hereafter for the sake of brevity.

You can see that your directory now contains a file called `a.out`. This is the default executable name that we have for historical reasons. It means "assembly output". If you want to change the name of your executable, specify it with the `-o` flag like this:

```bash
cc -o hello hello.c
```

Now you can execute your program like this:

```bash
./hello
hello, world
```

A dot with a slash in the beginning tells your shell that you intend to execute the file from your current directory (`.`) instead of searching for it in the default path. Many shells do not include the current directory in the executable search path by default.

If you want to check the status of your program (whether it was completed without issues or failed), type this:

```bash
echo $?
```

If everything is okay, it should print `0`.

You can also check the [GCC manual](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/) for additional information.

### Windows notes

On Windows systems, the best choice for the compiler is probably `MSVC` which is a part of `Visual Studio`. You can download Visual Studio [here](https://visualstudio.microsoft.com/downloads/). You can use Visual Studio IDE if you like, but I prefer to use Visual Studio Build Tools. It is a standalone package that contains all tools for building. You can then use any text editor of your choice, be it `neovim`, `Visual Studio Code`, or something else. If your editor does not contain a debugger, you may use `WinDbg` - Windows Debugger.

To be able to use the compiler, you have to start `developer powershell` or `developer command line`. This program performs the necessary configuration of the environment before starting the shell. You shouldn't use the MSVC compiler outside of it.

Here is how compilation on Windows looks like:

```pwsh
cl hello.c
```

You can execute your program like this:

```pwsh
./hello
hello, world
```

You can also check out the [MSVC compiler reference](https://learn.microsoft.com/en-us/cpp/build/reference/compiling-a-c-cpp-program?view=msvc-170) for additional information.

### CMake

As we discussed already, the compiler depends on your platform. I haven't heard about the cross-platform C compiler yet. However, even if that's the case, *building* projects can be cross-platform.

In our example, we use just one command to build our executable. However, it's not always this simple. Often you need to type many different commands to do that. To avoid typing all of those commands over and over again, *building systems* were created. Generally, the most popular building option is a `makefile`. It can look like this:

```makefile
main: main.o
 cc -o main main.o

main.o: main.c
 cc -c main.c
```

You can use it just by typing `make`.

This file is quite simple and you can probably read it without many problems. If you want to learn more, check out the [make utility manual](https://www.gnu.org/software/make/manual/html_node/index.html).

However, makefiles are platform-dependent as well. On Windows, makefiles are built using the `nmake` utility with the `msvc` compiler and have a specific flavor. The example above could look like this:

```makefile
main: main.obj
    cl -Fe:main main.obj

main.obj: main.c
    cl -Fo:main -c main.c
```

If you want to learn more about this tool, check out the [nmake reference](https://learn.microsoft.com/en-us/cpp/build/reference/nmake-reference?view=msvc-170).

This means that if you want to target both UNIX and Windows platforms, for example, you will need to provide makefiles in both flavors.

There is a popular cross-platform building system that supports both platforms. This means that you can provide just one `makefile` and it can be used to build your source code on any platform (assuming that your source code can be compiled on any platform which is usually the case unless you use anything platform-specific). This building system is called `CMake`.

If you want to use CMake, you will need to [download](https://cmake.org) and install it. If you use Visual Studio, it is installed by default.

To use CMake, create a new file called `CMakeLists.txt` in your directory:

```cmake
cmake_minimum_required(VERSION 3.10)

project(hello C)

add_executable(hello main.c)
```

Now, configure your project:

```bash
cmake -B build -S .
```

Here, the `-B` flag denotes a building directory (it is used to build your project), and `-S` denotes a source code directory. Note that at this point, the directory `build` does not exist yet. It will be automatically created after this command. Recall that `.` denotes the current directory. It is generally a good idea to specify a separate building directory since `CMake` generates a lot of files and you probably don't want to clutter your project with building files.

Finally, let's build the project:

```bash
cmake --build build
```

The project is usually built in a *debug* configuration. It is used to provide additional information necessary for debugging. If you want to build the project faster, you can specify *release* configuration:

```bash
cmake --build build --config release
```

After you are done, you can execute the program like this:

```bash
build/debug/hello
hello, world
```

If you specified the *release* configuration, change `debug` directory to `release`.

If you want to learn more about this tool, you can check out the [CMake documentation](https://cmake.org/cmake/help/latest/index.html). You can also try a [CMake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) to learn how to use the CMake in practice.