#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "libvector/vector.h"
#include "structs.h"

int endswith(char* string, char* p) {
	char* tmp = strrchr(string, '.');
	if (tmp && !strcmp(tmp, p)) return 1;
	return 0;
}

void** new_data_arr(int argc, ...) {
	va_list arglist;
	va_start(arglist, argc);
	void** arr = malloc(sizeof(void*)*argc);
	for (int i=0; i<argc; i++) {
		arr[i] = va_arg(arglist, void*);
	}
	va_end(arglist);
	return arr;
}

GObject** new_gobject_arr(int argc, ...) {
	va_list arglist;
	va_start(arglist, argc);
	GObject** arr = malloc(sizeof(GObject*)*argc);
	for (int i=0; i<argc; i++) {
		arr[i] = va_arg(arglist, GObject*);
	}
	va_end(arglist);
	return arr;
}

vector* new_generic_vector() {
	vector* vec = malloc(sizeof(vector));
	*vec=vector_init(VGEN);
	return vec;

}

struct Artist* find_artist(vector* V, char* artist) {
	for (int i=0; i<V->memsize; i++) {
		struct Artist* _art = vector_get_generic_at(V, i);
		if ( !strcmp( _art->name, artist) ) {
			return _art;
		}
	}
	return NULL;
}

struct Album* find_album(vector* V, char* album) {
	for (int i=0; i<V->memsize; i++) {
		struct Album* _alb = vector_get_generic_at(V, i);
		if ( !strcmp( _alb->name, album) ) {
			return _alb;
		}
	}
	return NULL;
}

void load_data_file() { /*ON PROGRESS*/ }
