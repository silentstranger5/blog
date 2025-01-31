+++
date = '2025-01-25T14:38:08+03:00'
draft = false
title = 'Writing an Algorithm Part 2'
+++

In the previous post, we wrote a SHA256 implementation. More specifically, we prepared all of the things needed to use an algorithm, such as constants, functions, and data structures. In this part, we are going to implement an algorithm itself. [Here](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf) is a document describing the algorithm specification.

After all of the groundwork, let's take a look at the algorithm:

```
6.2 SHA-256

SHA-256 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2 ^ 64. The 
algorithm uses 1) a message schedule of sixty-four 32-bit words, 2) eight working variables of 32 
bits each, and 3) a hash value of eight 32-bit words. The final result of SHA-256 is a 256-bit 
message digest.

The words of the message schedule are labeled W0, W1,…, W63. The eight working variables are 
labeled a, b, c, d, e, f, g, and h. The words of the hash value are labeled H0(i), H1(i), ..., H7(i), 
which will hold the initial hash value, H(0), replaced by each successive intermediate hash value
(after each message block is processed), H(i), and ending with the final hash value, H(N). SHA-256 
also uses two temporary words, T1 and T2.

```
From the start, let's declare all necessary variables that are going to be used in our algorithm:

```c
uint32_t W[64] = {0};                       // message schedule
uint32_t a, b, c, d, e, f, g, h, T1, T2;    // working variables
ctx->msg = msg;                             // put message in context
```

Let's take a look at preprocessing:

```
6.2.1 SHA-256 Preprocessing

1. Set the initial hash value, H(0), as specified in Sec. 5.3.3.
2. The message is padded and parsed as specified in Section 5.

```

We assume here that `ctx` is a pointer to the context, and `msg` is an input string from the user. Note that we already initiated hash values in the `context_init` function earlier. This leaves us with the second step: message preprocessing. As you recall, message preprocessing consists of two stages: padding and parsing. We already implemented functions that take care of that, so we can just use them:

```c
sha256_padding(ctx);
sha256_parse(ctx);
```

Finally, let's take a look at the algorithm itself:

```
6.2.2 SHA-256 Hash Computation

The SHA-256 hash computation uses functions and constants previously defined in Sec. 4.1.2 
and Sec. 4.2.2, respectively. Addition (+) is performed modulo 232.
Each message block, M(1), M(2), …, M(N), is processed in order, using the following steps:
For i=1 to N:

1. Prepare the message schedule, {Wt}:

        | Mt(i)                                     0 <= t <= 15
Wt =    |
        | s1{256}(Wt-2)+Wt-7+s0{256}(Wt-15)+Wt-16   16 <= t <= 63

2. Initialize the eight working variables, a, b, c, d, e, f, g, and h, with the (i-1)st hash 
value:

a = H0(i-1)
b = H1(i-1)
c = H2(i-1)
d = H3(i-1)
e = H4(i-1)
f = H5(i-1)
g = H6(i-1)
h = H7(i-1)

3. For t=0 to 63:

T1 = h + S1{256}(e) + Ch(e, f, g) + Kt{256} + Wt
T2 = S0{256}(a) + Maj(a, b, c)
h = g
g = f
f = e
e = d + T1
d = c
c = b
b = a
a = T1 + T2

4. Compute the ith intermediate hash value H(i):

H0(i) = a + H0(i-1)
H1(i) = b + H1(i-1)
H2(i) = c + H2(i-1)
H3(i) = d + H3(i-1)
H4(i) = e + H4(i-1)
H5(i) = f + H5(i-1)
H6(i) = g + H6(i-1)
H7(i) = h + H7(i-1)

After repeating steps one through four a total of N times (i.e., after processing M
(N)), the resulting 256-bit message digest of the message, M, is

H0(N) || H1(N) || H2(N) || H3(N) || H4(N) || H5(N) || H6(N) || H7(N)
```

It looks like a bit of work, but don't worry. We are going to cover it step by step.

As you can see from the algorithm, all further steps are happening inside of the loop:

```c
for (int i = 0; i < ctx->N; i++) {
    ...     // all code below goes there
}
```

We start by initializing the message schedule. For `0 <= t <= 16` this is pretty easy because we already parsed our message in advance:

```c
for (int t = 0; t < 16; t++) {
    W[t] = ctx->mblocks[i][t];      // Mt(i)
}
```

The next part is pretty simple as well. It uses functions `s0` and `s1` that we already defined as macros:

```c
for (int t = 16; t < 64; t++) {
    W[t] = s1(W[t-2]) + W[t-7] + s0(W[t-15]) + W[t-16];
}
```

Now let's initialize working variables:

```c
a = ctx->H[0];
b = ctx->H[1];
c = ctx->H[2];
d = ctx->H[3];
e = ctx->H[4];
f = ctx->H[5];
g = ctx->H[6];
h = ctx->H[7];
```

The next part is the essence of the algorithm. It uses two temporary variables, called `T1` and `T2` that extensively use our predefined functions like `S1`, `CH`, and `MAJ`, as well as constants `K` and message schedule `W`. After that we shift working variables by one, gradually mixing values of temporary variables. Over 64 iterations, variables are mixed pretty well:

```c
for (int t = 0; t < 64; t++) {
    T1 = h + S1(e) + CH(e, f, g) + K[t] + W[t];
    T2 = S0(a) + MAJ(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
}
```

All that is left to do is to add values of our variables to the hash values:

```c
ctx->H[0] += a;
ctx->H[1] += b;
ctx->H[2] += c;
ctx->H[3] += d;
ctx->H[4] += e;
ctx->H[5] += f;
ctx->H[6] += g;
ctx->H[7] += h;
```

Congratulations, the most important part, the core of our program is now done. Here is what a complete function looks like:

```c
/* update context */
void sha256_update(sha256_ctx *ctx, uint8_t *msg) {
    uint32_t W[64] = {0};
    uint32_t a, b, c, d, e, f, g, h, T1, T2;
    ctx->msg = msg;
    sha256_padding(ctx);
    sha256_parse(ctx);
    for (int i = 0; i < ctx->N; i++) {
        for (int t = 0; t < 16; t++) {
            W[t] = ctx->mblocks[i][t];
        }
        for (int t = 16; t < 64; t++) {
            W[t] = s1(W[t-2]) + W[t-7] + s0(W[t-15]) + W[t-16];
        }
        a = ctx->H[0];
        b = ctx->H[1];
        c = ctx->H[2];
        d = ctx->H[3];
        e = ctx->H[4];
        f = ctx->H[5];
        g = ctx->H[6];
        h = ctx->H[7];
        for (int t = 0; t < 64; t++) {
            T1 = h + S1(e) + CH(e, f, g) + K[t] + W[t];
            T2 = S0(a) + MAJ(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }
        ctx->H[0] += a;
        ctx->H[1] += b;
        ctx->H[2] += c;
        ctx->H[3] += d;
        ctx->H[4] += e;
        ctx->H[5] += f;
        ctx->H[6] += g;
        ctx->H[7] += h;
    }
}
```

All that is left to do is to extract context hash values into the message digest. Here is a function to do that:

```c
/* copy hash value into digest buffer */
void sha256_final(sha256_ctx *ctx, uint8_t *digest) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            uint32_t n = ENDIAN ? ctx->H[i] : BSWAP32(ctx->H[i]);
            memcpy(digest + i * 4, &n, sizeof(uint32_t));
        }
    }
}
```

We assume here that the `digest` buffer is already initialized and has enough storage. Notice that hash values are stored as little-endian values on little-endian machines. This means that we have to swap the bytes of those words before we can treat them as a collection of bytes.

Before we go further, I want to note that we are not going to use our context anymore. This means that now is a perfect time to free all data from our context (or at least define a function that will be doing it):

```c
/* free data from context */
void sha256_free(sha256_ctx *ctx) {
    free(ctx->msg);
    free(ctx->mblocks);
}
```

Well, what is the point of our digest if we can't do anything with it? Let's print it:

```c
/* print digest value */
void digest_print(uint8_t *digest) {
    printf("Digest: ");
    int n = ds / 8;     // ds = 256
    for (int i = 0; i < n; i++) {
        if (i > 0 && i % 4 == 0) {
            putchar(' ');
        }
        printf("%02X", digest[i]);
    }
    putchar('\n');
}
```

Finally, let's wrap it up with the `main` function:

```c
int main(void) {
    uint8_t msg[m];
    uint8_t digest[ds / 8];
    sha256_ctx ctx = {0};
    printf("Input Message: ");
    getline(msg, m);
    putchar('\n');
    sha256_init(&ctx);
    sha256_update(&ctx, msg);
    sha256_final(&ctx, digest);
    sha256_free(&ctx);
    digest_print(digest);
    return 0;
}
```

Try to compile the program. Hopefully, it should work. You can also check out the [source code](../sha.c).