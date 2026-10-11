#ifndef _COMMON_H_
#define _COMMON_H_

#define MIN(x, a)       (((x) < (a)) ? (x) : (a))
#define MAX(x, a)       (((x) > (a)) ? (x) : (a))
#define CLAMP(x, a, b)  (MIN(MAX(x, a), b))

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;

#endif /* _COMMON_H_ */
