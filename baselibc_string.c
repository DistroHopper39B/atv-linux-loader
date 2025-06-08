/*
 * These functions are taken from Baselibc.
 * https://github.com/PetteriAimonen/Baselibc
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Copyright (c) 2012 Petteri Aimonen <jpa at blc.mail.kapsi.fi>
 * All rights reserved.
 */

#include <atvlib.h>


int isupper(int __c)
{
    return __c >= 'A' && __c <= 'Z';
}

int islower(int __c)
{
    return __c >= 'a' && __c <= 'z';
}

int isalpha(int __c)
{
    return islower(__c) || isupper(__c);
}

int isdigit(int __c)
{
    return ((unsigned)__c - '0') <= 9;
}

int isalnum(int __c)
{
    return isalpha(__c) || isdigit(__c);
}

int isascii(int __c)
{
    return !(__c & ~0x7f);
}

int isblank(int __c)
{
    return (__c == '\t') || (__c == ' ');
}

int iscntrl(int __c)
{
    return __c < 0x20;
}

int isspace(int __c)
{
    return __c == ' ' || __c == '\n' || __c == '\t' || __c == '\r';
}

int isxdigit(int __c)
{
    return isdigit(__c) || (__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F');
}

int toupper(int __c)
{
    return islower(__c) ? (__c & ~32) : __c;
}

int tolower(int __c)
{
    return isupper(__c) ? (__c | 32) : __c;
}

void *memccpy(void *dst, const void *src, int c, size_t n)
{
    char *q = dst;
    const char *p = src;
    char ch;

    while (n--) {
        *q++ = ch = *p++;
        if (ch == (char)c)
            return q;
    }

    return NULL;		/* No instance of "c" found */
}

void *memchr(const void *s, int c, size_t n)
{
    const unsigned char *sp = s;

    while (n--) {
        if (*sp == (unsigned char)c)
            return (void *)sp;
        sp++;
    }

    return NULL;
}

void *memrchr(const void *s, int c, size_t n)
{
    const unsigned char *sp = (const unsigned char *)s + n - 1;

    while (n--) {
        if (*sp == (unsigned char)c)
            return (void *)sp;
        sp--;
    }

    return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *c1 = s1, *c2 = s2;
    int d = 0;

    while (n--) {
        d = (int)*c1++ - (int)*c2++;
        if (d)
            break;
    }

    return d;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    const char *p = src;
    char *q = dst;
#if defined(__i386__)
    size_t nl = n >> 2;
    asm volatile ("cld ; rep ; movsl ; movl %3,%0 ; rep ; movsb":"+c" (nl),
    "+S"(p), "+D"(q)
            :"r"(n & 3));
#elif defined(__x86_64__)
    size_t nq = n >> 3;
	asm volatile ("cld ; rep ; movsq ; movl %3,%%ecx ; rep ; movsb":"+c"
		      (nq), "+S"(p), "+D"(q)
		      :"r"((uint32_t) (n & 7)));
#else
	while (n--) {
		*q++ = *p++;
	}
#endif

    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    const char *p = src;
    char *q = dst;
#if defined(__i386__) || defined(__x86_64__)
    if (q < p) {
        asm volatile("cld; rep; movsb"
                : "+c" (n), "+S"(p), "+D"(q));
    } else {
        p += (n - 1);
        q += (n - 1);
        asm volatile("std; rep; movsb; cld"
                : "+c" (n), "+S"(p), "+D"(q));
    }
#else
    if (q < p) {
		while (n--) {
			*q++ = *p++;
		}
	} else {
		p += n;
		q += n;
		while (n--) {
			*--q = *--p;
		}
	}
#endif

    return dst;
}

void *memset(void *dst, int c, size_t n)
{
    char *q = dst;

#if defined(__i386__)
    size_t nl = n >> 2;
    asm volatile ("cld ; rep ; stosl ; movl %3,%0 ; rep ; stosb"
            : "+c" (nl), "+D" (q)
            : "a" ((unsigned char)c * 0x01010101U), "r" (n & 3));
#elif defined(__x86_64__)
    size_t nq = n >> 3;
	asm volatile ("cld ; rep ; stosq ; movl %3,%%ecx ; rep ; stosb"
		      :"+c" (nq), "+D" (q)
		      : "a" ((unsigned char)c * 0x0101010101010101U),
			"r" ((uint32_t) n & 7));
#else
	while (n--) {
		*q++ = c;
	}
#endif

    return dst;
}

void *memmem(const void *haystack, size_t n, const void *needle, size_t m)
{
    const unsigned char *y = (const unsigned char *)haystack;
    const unsigned char *x = (const unsigned char *)needle;

    size_t j, k, l;

    if (m > n || !m || !n)
        return NULL;

    if (1 != m) {
        if (x[0] == x[1]) {
            k = 2;
            l = 1;
        } else {
            k = 1;
            l = 2;
        }

        j = 0;
        while (j <= n - m) {
            if (x[1] != y[j + 1]) {
                j += k;
            } else {
                if (!memcmp(x + 2, y + j + 2, m - 2)
                    && x[0] == y[j])
                    return (void *)&y[j];
                j += l;
            }
        }
    } else
        do {
            if (*y == *x)
                return (void *)y;
            y++;
        } while (--n);

    return NULL;
}

void memswap(void *m1, void *m2, size_t n)
{
    char *p = m1;
    char *q = m2;
    char tmp;

    while (n--) {
        tmp = *p;
        *p = *q;
        *q = tmp;

        p++;
        q++;
    }
}

void bzero(void *dst, size_t n)
{
    memset(dst, 0, n);
}

int strcasecmp(const char *s1, const char *s2)
{
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (1) {
        /* toupper() expects an unsigned char (implicitly cast to int)
           as input, and returns an int, which is exactly what we want. */
        d = toupper(ch = *c1++) - toupper(*c2++);
        if (d || !ch)
            break;
    }

    return d;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (n--) {
        /* toupper() expects an unsigned char (implicitly cast to int)
           as input, and returns an int, which is exactly what we want. */
        d = toupper(ch = *c1++) - toupper(*c2++);
        if (d || !ch)
            break;
    }

    return d;
}

char *strcat(char *dst, const char *src)
{
    strcpy(strchr(dst, '\0'), src);
    return dst;
}

char *strchr(const char *s, int c)
{
    while (*s != (char)c) {
        if (!*s)
            return NULL;
        s++;
    }

    return (char *)s;
}

char *strrchr(const char *s, int c)
{
    const char *found = NULL;

    while (*s) {
        if (*s == (char)c)
            found = s;
        s++;
    }

    return (char *)found;
}

int strcmp(const char *s1, const char *s2)
{
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (1) {
        d = (int)(ch = *c1++) - (int)*c2++;
        if (d || !ch)
            break;
    }

    return d;
}

char *strcpy(char *dst, const char *src)
{
    char *q = dst;
    const char *p = src;
    char ch;

    do {
        *q++ = ch = *p++;
    } while (ch);

    return dst;
}

size_t strcspn(const char *s1, const char *s2)
{
    const char *s = s1;
    const char *c;

    while (*s1)
    {
        for (c = s2; *c; c++)
        {
            if (*s1 == *c)
                break;
        }
        if (*c)
            break;
        s1++;
    }

    return s1 - s;
}

size_t strlen(const char *s)
{
    const char *ss = s;
    while (*ss)
        ss++;
    return ss - s;
}

size_t strnlen(const char *s, size_t maxlen)
{
    const char *ss = s;

    /* Important: the maxlen test must precede the reference through ss;
       since the byte beyond the maximum may segfault */
    while ((maxlen > 0) && *ss) {
        ss++;
        maxlen--;
    }
    return ss - s;
}

char *strncat(char *dst, const char *src, size_t n)
{
    char *q = strchr(dst, '\0');
    const char *p = src;
    char ch;

    while (n--) {
        *q++ = ch = *p++;
        if (!ch)
            return dst;
    }
    *q = '\0';

    return dst;
}

size_t strlcat(char *dst, const char *src, size_t size)
{
    size_t bytes = 0;
    char *q = dst;
    const char *p = src;
    char ch;

    while (bytes < size && *q) {
        q++;
        bytes++;
    }
    if (bytes == size)
        return (bytes + strlen(src));

    while ((ch = *p++)) {
        if (bytes + 1 < size)
            *q++ = ch;

        bytes++;
    }

    *q = '\0';
    return bytes;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (n--) {
        d = (int)(ch = *c1++) - (int)*c2++;
        if (d || !ch)
            break;
    }

    return d;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    char *q = dst;
    const char *p = src;
    char ch;

    while (n) {
        n--;
        *q++ = ch = *p++;
        if (!ch)
            break;
    }

    /* The specs say strncpy() fills the entire buffer with NUL.  Sigh. */
    memset(q, 0, n);

    return dst;
}

size_t strlcpy(char *dst, const char *src, size_t size)
{
    size_t bytes = 0;
    char *q = dst;
    const char *p = src;
    char ch;

    while ((ch = *p++)) {
        if (bytes + 1 < size)
            *q++ = ch;

        bytes++;
    }

    /* If size == 0 there is no space for a final null... */
    if (size)
        *q = '\0';

    return bytes;
}

char *strpbrk(const char *s1, const char *s2)
{
    const char *c = s2;
    if (!*s1)
        return (char *) NULL;

    while (*s1)
    {
        for (c = s2; *c; c++)
        {
            if (*s1 == *c)
                break;
        }
        if (*c)
            break;
        s1++;
    }

    if (*c == '\0')
        s1 = NULL;

    return (char *) s1;
}

char *strsep(char **stringp, const char *delim)
{
    char *s = *stringp;
    char *e;

    if (!s)
        return NULL;

    e = strpbrk(s, delim);
    if (e)
        *e++ = '\0';

    *stringp = e;
    return s;
}

size_t strspn(const char *s1, const char *s2)
{
    const char *s = s1;
    const char *c;

    while (*s1)
    {
        for (c = s2; *c; c++)
        {
            if (*s1 == *c)
                break;
        }
        if (*c == '\0')
            break;
        s1++;
    }

    return s1 - s;
}

char *strstr(const char *haystack, const char *needle)
{
    return (char *)memmem(haystack, strlen(haystack), needle,
                          strlen(needle));
}

char *strtok(char *s, const char *delim)
{
    static char *holder;

    return strtok_r(s, delim, &holder);
}

char *strtok_r(char *s, const char *delim, char **holder)
{
    if (s)
        *holder = s;

    do {
        s = strsep(holder, delim);
    } while (s && !*s);

    return s;
}
