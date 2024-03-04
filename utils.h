#include "structs.h"
#include "libvector/vector.h"
int endswith(char* string, char* p);
void** new_data_arr(int argc, ...);
GObject** new_gobject_arr(int argc, ...);
vector* new_generic_vector();
struct Artist* find_artist(vector* V, char* artist);
struct Album* find_album(vector* V, char* album);
