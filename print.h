#ifndef PRINT_H
#define PRINT_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

static int __print_enable_color = 1;

static void __print_color(FILE* fd, int a) {
	if (!__print_enable_color) return;
	if (a == -1) fputs("\x1b(B\x1b[m", fd);
	else fprintf(fd, "\x1b[38;5;%im", a);
}

#define __print_array(fd, T, qual, color) \
	__print_color(fd, __print_color_normal); \
	int max_len = 16; \
	int n = size/sizeof(T); \
	T *m = va_arg(v, T*); \
	fputc('[', fd);	  \
	__print_color(fd, color); \
	for (int i = 0; i < (n < max_len ? n : max_len); i++) { \
		if (i > 0) fputc(' ', fd);			\
		fprintf(fd, qual, m[i]); \
	} \
	__print_color(fd, __print_color_normal); \
	if (n > max_len) fputs("...", fd);	 \
	fputc(']', fd);

static int __print_color_normal = -1; // -1 means default terminal foreground color
static int __print_color_number = 4;
static int __print_color_string = 1;
static int __print_color_hex = 2;
static int __print_color_float = 5;

static void __print_setup_colors(int normal, int number, int string, int hex, int fractional) {
	__print_color_string = string;
	__print_color_number = number;
	__print_color_hex = hex;
	__print_color_normal = normal;
	__print_color_float = fractional;
}

static void __print_func (FILE *fd, int count, unsigned short types[], ...) {
	va_list v;
	va_start(v, types);
	#ifdef __print_DEBUG
	fprintf(fd, "args[%i]: ", count);
	for (int i = 0; i < count; i++) {
		char type = types[i] & 0x1F;
		char size = types[i] >> 5;
		if (i > 0) fputc(' ', fd);
		fprintf(fd, "%i[%i]", type, size);
	}
	fputc('\n', fd);
	#endif // __print_DEBUG

	double d;
	char c;
	for (int i = 0; i < count; i++) {
		if (i > 0) fputc(' ', fd);
		char type = types[i] & 0x1F;
		char size = types[i] >> 5;
		switch (type) {
		case 1: {
			__print_color(fd, __print_color_float);
		        d = va_arg(v, double);
			fprintf(fd, "%'G", d);
			break;
		}
		case 2: {
			__print_color(fd, __print_color_string);
		        c = va_arg(v, int);
			fprintf(fd, "'%c'", c); __print_color(fd, __print_color_number);
			fprintf(fd, "%i", (int)c);
			break;
		}
		case 3: {
			__print_color(fd, __print_color_number);
		        c = va_arg(v, int);
			fprintf(fd, "%i", (unsigned char)c);
			__print_color(fd, __print_color_normal);
		        fputc('<', fd);
			__print_color(fd, __print_color_hex);
			fprintf(fd, "0x%X", (unsigned char)c);
			__print_color(fd, __print_color_normal);
			fputc('>', fd);
			break;
		}
		case 4: {
			__print_color(fd, __print_color_number);
			fprintf(fd, "%'i", va_arg(v, int));
			break;
		}
		case 5: {
			__print_color(fd, __print_color_number);
			fprintf(fd, "%'u", va_arg(v, int));
			break;
		}
		case 6: {
			__print_color(fd, __print_color_number);
			fprintf(fd, "%'li", va_arg(v, unsigned long));
			break;
		}
		case 7: {
			__print_color(fd, __print_color_number);
			fprintf(fd, "%'lu", va_arg(v, long));
			break;
		}
		case 8: {
			__print_color(fd, __print_color_string);
			fprintf(fd, "\"%s\"", va_arg(v, char*));
			break;
		}
		case 9: {
			__print_color(fd, __print_color_normal);
			fprintf(fd, "%s", va_arg(v, char*));
			break;
		}
		case 10: {
			__print_color(fd, __print_color_hex);
			fprintf(fd, "%p", va_arg(v, void*));
			break;
		}
		case 11: {
			__print_array(fd, int, "%i", __print_color_number);
		        break;
		}
		case 12: {
			__print_array(fd, unsigned int, "%u", __print_color_number);
			break;
		}
		case 13: {
			__print_array(fd, short, "%i", __print_color_number);
			break;
		}
		case 14: {
			__print_array(fd, unsigned short, "%i", __print_color_number);
			break;
		}
		case 15: {
			__print_array(fd, char*, "\"%s\"", __print_color_string);
			break;
		}
		case 16: {
		        c = va_arg(v, int);
			fprintf(fd, c ? "\x1b[38;5;2mtrue": "\x1b[38;5;1mfalse");
			break;
		}
		default:
			fprintf(fd, "print: unsupported type (of size %i)\n", size);
			break;
		}
	}
	va_end(v);
	__print_color(fd, -1);
	fputc('\n', fd);
}

#define __print_typeid(a) \
	__builtin_choose_expr(__print_is_type(a, double), 1, \
	__builtin_choose_expr(__print_is_type(a, float), 1, \
	__builtin_choose_expr(__print_is_type(a, char), 2, \
	__builtin_choose_expr(__print_is_type(a, unsigned char), 3, \
	__builtin_choose_expr(__print_is_type(a, int), 4, \
	__builtin_choose_expr(__print_is_type(a, unsigned int), 5, \
	__builtin_choose_expr(__print_is_type(a, long), 6, \
	__builtin_choose_expr(__print_is_type(a, unsigned long), 7, \
	__builtin_choose_expr(__print_is_type(a, short), 4, \
	__builtin_choose_expr(__print_is_type(a, char*), 8, \
	__builtin_choose_expr(__print_is_type(a, char[]), 9, \
	__builtin_choose_expr(__print_is_type(a, void*), 10, \
	__builtin_choose_expr(__print_is_type(a, int[]), 11, \
	__builtin_choose_expr(__print_is_type(a, unsigned int[]), 12, \
	__builtin_choose_expr(__print_is_type(a, short[]), 13, \
	__builtin_choose_expr(__print_is_type(a, unsigned short[]), 14, \
	__builtin_choose_expr(__print_is_type(a, char*[]), 15, \
	__builtin_choose_expr(__print_is_type(a, bool), 16, \
	__builtin_choose_expr(sizeof(a) == 1, 2, \
	__builtin_choose_expr(sizeof(a) == 2, 4, \
	(0)  ))))))))))))))))))))

/* Disable Clang's warning of "cont" unused variable. */
#if defined (__clang__)
#pragma clang diagnostic ignored "-Wunused-value"
#endif

#define __print_push(c,size,cont) (cont, *--_p = c | (size << 5))
#define __builtin_choose_expr __builtin_choose_expr
#define __print_is_type(a, t) __builtin_types_compatible_p(typeof(a), t)
#define __print_count_int(q,w,e,r,t,y,u,i,o,p,a,s,d,f,g,h,j,k,l,z,x,c,v,b,n,m,...) m
#define __print_count(a...)__print_count_int(a,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define __print_code(a, cont) __builtin_choose_expr(__print_is_type(a, void), 0, __print_push(__print_typeid(a), (sizeof(a)<(1<<16>>5)?sizeof(a):(1<<16>>5)-1), cont))

#define __print_types_int(q,w,e,r,t,y,u,i,o,p,a,s,d,f,g,h,j,k,l,z,x,c,v,b,n,m,...)__print_code(q,__print_code(w,__print_code(e,__print_code(r,__print_code(t,__print_code(y,__print_code(u,__print_code(i,__print_code(o,__print_code(p,__print_code(a,__print_code(s,__print_code(d,__print_code(f,__print_code(g,__print_code(h,__print_code(j,__print_code(k,__print_code(l,__print_code(z,__print_code(x,__print_code(c,__print_code(v,__print_code(b,__print_code(n,__print_code(m, 0))))))))))))))))))))))))))

#define __print_types(a...) __print_types_int(a, (void)0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)

#define fprint(fd, a...) ({ \
	int count = __print_count(a); \
	unsigned short stack[count], *_p = stack + count; \
	__print_types(a); \
	__print_func(fd, count, _p, a); \
})

#define print(a...) fprint(stdout, a)

#endif
