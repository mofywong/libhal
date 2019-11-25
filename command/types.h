#ifndef __HE_TYPES_H__
#define __HE_TYPES_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char bool_t;
typedef char* string_t;
typedef const char* const_string_t;

typedef struct {
    size_t size;
    void* data;
} bytes_t;

#ifndef false
#define false   0
#endif

#ifndef true
#define true    1
#endif

#ifndef count_of
#define count_of(a) (sizeof(a)/sizeof(a[0]))
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifdef __cplusplus
}
#endif

#endif // __HE_TYPES_H__
