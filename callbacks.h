#include <gtk/gtk.h>
void* thread_counter(void* data);
int change_value (GtkRange* self, GtkScrollType* a, gdouble value, gpointer data);
void play_pause(GtkApplication* app, gpointer data);
void load_song(GtkApplication* app, gpointer data);
