+++
date = '2025-01-22T14:01:17+03:00'
draft = false
title = 'Using C Libraries'
+++

In the previous post, I've discussed C language and briefly introduced how to used it. To be honest, if you have programming experience already, chances are that you can use most of the C without issues. There may be some problems with pointers though. Maybe I'll cover it in the next time.

When I described C, I said that it is simple language in itself, although it can be tricky to use because there is not much included in the language. Indeed, if you compare it to modern languages, there are many features lacking. I'm not even telling about various containers and algorithms like lists, maps or sets.

Take a look at Python or Golang, for example. They include many features for wide variety of purposes, like HTML parsing, SQL interface (or SQLite engine), JSON parsing, image processing, XML parsing, cryptographic libraries, HTTP servers... This list can go on and on. This definetly makes those languages convenient to use.

C language lacks most of those features out of the box providing only very basic functionality like string processing conversions between strings and numbers, math functions, memory management and some OS interfaces (for working with files for example). This choice is deliberate: C is very minimalistic language and it is often expected that you write anything that's missing.

However, C does support usage of libraries. And there is surprisingly wide variety of said libraries for many purposes. Today I want to discuss how to use libraries in C language. It is certainly trickier compared to languages like Python or Go that have their package managers like pip. However, since C is pretty old language, chances are that you probably would be able to find quite a few libraries for this language.

In fact, many languages, especially those with lower performance (yes, Python, I'm talking about you) often provide interfaces to libraries that are actually written in C or C++ (it includes many popular libraries especially those working with data or artificial intelligence).

Let's try to use [OpenSSL](https://openssl.org) library. We are going to implement a simple program that, given an input string, would provide SHA256 hash digest of that string.

I intend to build the library from its source code and copy library files to our program. I going through this process to show how library can be built from source and how it can be used. Before I proceed, I want to note that there are couple shortcuts that can reduce your time dramaticly and possibly simplify the whole process. I am not going to explain those options in detail right now, but you can always find this information on your own.

- Prebuilt binaries. Many libraries provide prebuilt binaries that you can use without spending time to built them.
- Package managers. This allows not only skip building stage (or at least automate it), but also you don't have to worry about distribution files. For example, you may consider using [vcpkg](https://vcpkg.io).

Let's get back on track. Download the library and unpack it at any appropriate path. Take a look at the contents:

```bash
ACKNOWLEDGEMENTS.md  demos             NOTES-DJGPP.md     README.md
apps                 doc               NOTES-NONSTOP.md   README-PROVIDERS.md
AUTHORS.md           engines           NOTES-PERL.md      README-QUIC.md
build.info           exporters         NOTES-POSIX.md     ssl
CHANGES.md           external          NOTES-UNIX.md      SUPPORT.md
cloudflare-quiche    fuzz              NOTES-VALGRIND.md  test
CODE-OF-CONDUCT.md   HACKING.md        NOTES-VMS.md       tlsfuzzer
config               include           NOTES-WINDOWS.md   tlslite-ng
config.com           INSTALL.md        oqs-provider       tools
configdata.pm.in     LICENSE.txt       os-dep             util
Configurations       ms                providers          VERSION.dat
Configure            NEWS.md           python-ecdsa       VMS
CONTRIBUTING.md      NOTES-ANDROID.md  README-ENGINES.md  wycheproof
crypto               NOTES-ANSI.md     README-FIPS.md
```

Well, looks like there is a lot of stuff here. It's usually a good idea to start with `README.md`. There is a lot of useful information here. But we are interested in building it. Here is the contents of the `Build and Install` section from `README`:

```md
Build and Install
=================

After obtaining the Source, have a look at the [INSTALL](INSTALL.md) file for
detailed instructions about building and installing OpenSSL. For some
platforms, the installation instructions are amended by a platform specific
document.

 * [Notes for UNIX-like platforms](NOTES-UNIX.md)
 * [Notes for Android platforms](NOTES-ANDROID.md)
 * [Notes for Windows platforms](NOTES-WINDOWS.md)
 * [Notes for the DOS platform with DJGPP](NOTES-DJGPP.md)
 * [Notes for the OpenVMS platform](NOTES-VMS.md)
 * [Notes on Perl](NOTES-PERL.md)
 * [Notes on Valgrind](NOTES-VALGRIND.md)

Specific notes on upgrading to OpenSSL 3.x from previous versions can be found
in the [ossl-guide-migration(7ossl)] manual page.
```

As you can see, information on building can be found in `INSTALL.md`. That's often the case for many libraries (such file is also often called `BUILD.md` or something like that). Take a look at it. Many platform specific documents have form of `NOTES-PLATFORM.md`. If you are on UNIX-like machine, take a look at `NOTES-UNIX.md`. I'm going to cover Windows installation here.

As you can read from `NOTES-WINDOWS.md`, there are basically three prerequisites for us:

- MSVC compiler. Hopefully you installed it if you followed my previous post.
- [Strawberry Perl](https://strawberryperl.com/) is a perl environment for windows
- [NASM](https://nasm.us/) is a popular assembler

It is recommended to install those programs since installer handles most things automatically. However, I am not going to use this software for anything besides this build. Installers on Windows (and maybe on UNIX systems too) are not always written properly and sometimes leave a trace of files or registry entries / configuration files behind them. If that's not an issue for you, feel free to use installer since it makes things easier. I however intend to download standalone binaries, temporarily add them to `PATH` variable, build OpenSSL, and remove those files after I'm done. After all, I find this approach more convenient for me.

Now I'm going to open to open `Developer Power Shell` since I intend to use MSVC build tools to build the library.

Here is what I need to add to `PATH` variable:

```
/path/to/nasm/
/path/to/perl/perl/bin
/path/to/perl/perl/site/bin
/path/to/perl/c/bin
```

Those directories might differ depending on your platform. Read documentation provided with portable / zip version of software to learn what to add to the `PATH`. It may look somewhat like this:

```pwsh
$env:path += ";C:/path/to/nasm;C:/path/to/perl/perl/bin;C:/path/to/perl/site/bin;C:/path/to/perl/c/bin"
```

Note that this bash syntax is invalid if you use cmd. In order to set up your environment variables properly, please refer to syntax of the shell you are using.

After I'm done with setting up `PATH`, let's head to openssl directory. Now, according to the docs, I need to use perl to configure the library:

```pwsh
perl Configure VC-WIN64A
```

If you are not on x64 architecture, select the appropriate option for configuration. Read details in the docs.

Let's build it:

```pwsh
nmake
```

Actual building may take some time, depending on your hardware.

If you feel like it, you may test library as well:

```pwsh
nmake test
```

From here onwards, you have two choices:

- Install the library
- Put it to your project

To install library, type

```pwsh
nmake install
```

Admin priveledges might be required for installation.

If you are going to use library just in one project, it's probably better to just put it here. If that's not the case, it's better to install it.

I am going to show you how to put library in your project and use it this way.

Let's create directories for our project. Final structure should look like this:

```
sha
|-- bin
|-- include
|-- lib
|-- src
```

`bin` directory will contain our executables and dynamic libraries, `include` will contain header files, `lib` will contain static library files, and `src` will contain source code.

Note. Find file `applink.c` and put it into `src` directory. It will be needed for compilation.

Now, we need to copy files from library to our project. Copy all `.dll` files to `bin`, all files inside `include` to `include`, and all `.lib` files to `lib`:

```bash
cp /path/to/openssl/*.dll /path/to/sha/bin
cp /path/to/openssl/include/* /path/to/sha/include
cp /path/to/openssl/*.lib /path/to/sha/lib
```

Depending on your editor, configure it to not complain about libraries. Explore documentation of your editor to find details.

Finally, let's create a source file called `sha.c`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#define SIZE 512

int main(void) {
    EVP_MD_CTX *ctx = NULL;
    EVP_MD *sha256 = NULL;
    unsigned char *msg = malloc(SIZE);
    unsigned int len = 0;
    unsigned char *digest = NULL;
    int ret = 1;

    /* Read string */
    fgets(msg, SIZE, stdin);
    *strchr(msg, '\n') = 0;

    /* Create a context for the digest operation */
    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        goto err;

    /* Fetch the SHA256 algorithm implementation for doing the digest */
    sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
    if (sha256 == NULL)
        goto err;

    /* Initialize the digest operation */
    if (!EVP_DigestInit_ex(ctx, sha256, NULL))
        goto err;

    /* Pass the message to be digested */
    if (!EVP_DigestUpdate(ctx, msg, strlen(msg)))
        goto err;

    /* Allocate the output buffer */
    digest = OPENSSL_malloc(EVP_MD_get_size(sha256));
    if (digest == NULL)
        goto err;

    /* Now calculate the digest itself */
    if (!EVP_DigestFinal_ex(ctx, digest, &len))
        goto err;

    /* Print out the digest result */
    BIO_dump_fp(stdout, digest, len);

    ret = 0;

err:
    /* Clean up all the resources we allocated */
    free(msg);
    OPENSSL_free(digest);
    EVP_MD_free(sha256);
    EVP_MD_CTX_free(ctx);
    if (ret != 0)
        ERR_print_errors_fp(stderr);
    return ret;
}
```

Now we need to tell the compiler four things:

- use `include` as include directory
- put executables and object files in `bin` directory
- link `src/applink.c`
- link `lib/libcrypto.lib` (or `lib/libcrypto.a`)

Specifics depend on your operating system. Read compiler documentation for details. For example, this is how to do that on MSVC (assuming you are at your project directory):

```pwsh
cl /Fe:./bin/ /Fo:./bin/ /I ./include/ ./src/sha.c ./src/applink.c ./lib/libcrypto.lib
```

On UNIX, it *probably* looks like this (not sure though):

```bash
cc -o ./bin/sha -I ./include ./src/sha.c ./src/applink.c ./lib/libcrypto.a
```

If you did not make any mistakes, results probably look like this:

```pwsh
shell> ./bin/main
hello world
0000 - b9 4d 27 b9 93 4d 3e 08-a5 2e 52 d7 da 7d ab fa   .M'..M>...R..}..
0010 - c4 84 ef e3 7a 53 80 ee-90 88 f7 ac e2 ef cd e9   ....zS..........
```