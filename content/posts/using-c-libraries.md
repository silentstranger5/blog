+++
date = '2025-01-22T14:01:17+03:00'
draft = false
title = 'Using C Libraries'
+++

In the previous post, I discussed C language and briefly introduced how to use it. To be honest, if you have programming experience already, chances are that you can use most of the C without issues. There may be some problems with pointers though. Maybe I'll cover it the next time.

When I described C, I said that it is a simple language in itself, although it can be tricky to use because there is not much included in the language. Indeed, if you compare it to modern languages, there are many features lacking. I'm not even talking about various containers and algorithms like lists, maps, or sets.

Take a look at Python or Golang, for example. They include many features for wide variety of purposes, like HTML parsing, SQL interface (or SQLite engine), JSON parsing, image processing, XML parsing, cryptographic libraries, HTTP servers... This list can go on and on. This definitely makes those languages convenient to use.

C language lacks most of those features out of the box providing only very basic functionality like string processing conversions between strings and numbers, math functions, memory management, and some OS interfaces (for working with files for example). This choice is deliberate: C is very minimalistic language and it is often expected that you write anything that's missing.

However, C does support the usage of libraries. And there is a surprisingly wide variety of said libraries for many purposes. Today I want to discuss how to use libraries in C language. It is certainly trickier compared to languages like Python or Go that have package managers like Pip. However, since C is a pretty old language, chances are that you probably would be able to find quite a few libraries for this language.

In fact, many languages, especially those with lower performance (yes, Python, I'm talking about you) often provide interfaces to libraries that are actually written in C or C++ (it includes many popular libraries, especially those working with data or artificial intelligence).

Let's try to use the [OpenSSL](https://openssl.org) library. We are going to implement a simple program that, given an input string, would provide a SHA256 hash digest of that string.

There are many ways to manage dependencies in C++:

- Build library from source and install it
- Build a library from the source and put it inside our project
- Use the build system to fetch source code and build it
- Use build system with package manager

The first three options allow for great flexibility, but they are quite difficult. You have to figure out many additional details like:

- How to obtain tools needed for building
- Is it possible to use portable versions of those tools
- Are there any prebuilt binaries that could be used instead
- How to install the package and how to configure this installation
- How to configure the build system to download and build source code
- If the library is installed, how to configure the build system to detect it
...

I can continue this list all day long, but I hope that you got the point. Managing dependencies is probably one of the worst aspects when it comes to C++. This is precisely the reason why tools called `package managers` were created.

I am going to cover details that are related to Windows (although this should work on other platforms as well). On UNIX systems, the general idea is to use a system package manager (like `apt`) to download and install libraries and then use the build system to detect those libraries. Maybe I'll cover it in more detail later.

To use the package manager we will need three things:

- [git](https://git-scm.com), which is hopefully already installed if you followed previous posts
- [CMake](https://cmake.org), which is installed with VS Build Tools by default
- [vcpkg](https://vcpkg.io), a package manager for C/C++

I am going to tell you how to use `vcpkg` with your projects. You can also read details [here](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started).

1. Clone the `vcpkg` repository in any place you want:

```pwsh
git clone https://github.com/microsoft/vcpkg.git
```

2. Run the bootstrap script:

```pwsh
cd vcpkg; .\bootstrap-vcpkg.bat
```

3. Configure environment variables:

You have to use PowerShell to do this step. This **will NOT** work for Command Prompt.

Before we proceed, let's save user and system `PATH` variables for recovery if something goes wrong:

```pwsh
$usrpath = (Get-ItemProperty HKCU:\Environment).PATH
$syspath = (Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment').PATH
echo `
"User PATH:
$usrpath

System PATH:
$syspath" > pathvar.txt
```

Now try to paste those commands. Replace `C:\path\to\vcpkg` with your vcpkg path.

```pwsh
setx VCPKG_ROOT C:\path\to\vcpkg
setx PATH "$usrpath;C:\path\to\vcpkg"
```

Display your `pathvar.txt` in PowerShell in case you'll need it:

```pwsh
cat pathvar.txt
```

Let's check if everything is fine:

```pwsh
sysdm.cpl
```

Go to `Advanced->Environment Variables->User Variables`. Click on `Path` and then click `Edit`. Check if everything is okay. You should see a list of directories from your User `PATH` and the directory you just added. If anything is wrong, recover User `PATH`. If everything is okay, then go to the next step, you are done here.

Here is how to recover your `PATH` variables. Copy text from file `pathvar.txt` displayed on your terminal under `User PATH:`, go back to the `PATH` edit window, click `Edit Text`, remove all text, and paste the text you just copied from a file (with CTRL+V). Click `OK`. Go to `System variables->Path` and check it as well. The recovery process for it is identical (use text under `System PATH` for this step).

4. Create a directory for your project

```pwsh
mkdir sha
cd sha
```

5. Create a `vcpkg` manifest file:

```pwsh
vcpkg new --application
```

6. Add `openssl` dependency:

```pwsh
vcpkg add port openssl
```

7. Create the project file called `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)

project(sha)

find_package(OpenSSL REQUIRED)

add_executable(sha sha.c)

target_link_libraries(sha PRIVATE OpenSSL::SSL)
target_link_libraries(sha PRIVATE OpenSSL::Crypto)
target_link_libraries(sha PRIVATE OpenSSL::applink)
```

8. Create a file called `CMakePresets.json`:

```json
{
  "version": 2,
  "configurePresets": [
  {
      "name": "vcpkg",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      }
    }
  ]
}
```

9. Create a file called `CMakeUserPresets.json`:

```json
{
    "version": 2,
    "configurePresets": [
    {
        "name": "default",
        "inherits": "vcpkg",
        "environment": {
          "VCPKG_ROOT": "<path to vcpkg>"
    }
  }
 ]
}
```

10. Create a source file called `sha.c`:

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
    printf("Input message: ");
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

11. Configure build with CMake:

```pwsh
cmake --preset=default
```

12. Build project:

```pwsh
cmake --build build
```

13. Run the application:

```pwsh
.\build\sha

Input message: hello world
0000 - b9 4d 27 b9 93 4d 3e 08-a5 2e 52 d7 da 7d ab fa   .M'..M>...R..}..
0010 - c4 84 ef e3 7a 53 80 ee-90 88 f7 ac e2 ef cd e9   ....zS..........
```