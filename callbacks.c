#include <gtk/gtk.h>
#include <sys/poll.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "libvector/vector.h"
#include "callbacks.h"
#include "utils.h"
#include "structs.h"

void* thread_counter(void* data) {
	void** _data = data;
	GtkScale* scale = _data[0];
	int* pipearr = _data[1];
	int c=0;
	int max=0;
	int pause=1;
	struct pollfd fds[1] = {pipearr[0], POLLIN, 0};
	char order[1];
	/*each second that passes:
	 *  Augment value of scale by 1
	 */
	for (;;) {
		int X = poll(fds, 1, 1000);
		switch (X) {
			case 0:
				// aumentar segundo
				switch (pause) {
					case 1: continue;
					default:
						c++;
						gtk_range_set_value(GTK_RANGE(scale), c);
				}
				break;
			default:
				{
					// comando en pipe
					read(pipearr[0], order, 1);
					switch (order[0]) {
						case 1:  // load
							max=gtk_adjustment_get_upper(gtk_range_get_adjustment(GTK_RANGE(scale)));
							c=0;
							pause=0;
							continue;
						case 2:  // pause
							pause = !pause;
							continue;
						case 3:  // jump
							c = gtk_range_get_value(GTK_RANGE(scale));
							continue;
					}
					break;
				}
		}
	}
}

int change_value (GtkRange* self, GtkScrollType* a, gdouble value, gpointer data) {
	void** _data = data;
	int* pipearr = _data[0];
	FILE* P = _data[1];
	string str = string_init("jump "); string_append_int(&str, (int)value);
	string_append(&str, "s\n");
	fputs(string_get_c_str(&str), P);
	fflush(P);
	write(pipearr[1], "\3", 1);
	return 0;
}

void play_pause(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	GtkWidget* playbt = GTK_WIDGET((GObject*)_data[1]);
	int* pipearr = _data[2];
	write(pipearr[1], "\2", 1);
	if (!strcmp(gtk_button_get_label(GTK_BUTTON(playbt)), "󰐊")) {
		gtk_button_set_label(GTK_BUTTON(playbt), "󰏤");
	} else { gtk_button_set_label(GTK_BUTTON(playbt), "󰐊"); }
	fputs("pause\n", P);
	fflush(P);
}

void load_song(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = (FILE*)_data[0];
	struct Song* song = _data[1];
	char* path = (char*)_data[2];
	GObject* player_song_name = _data[3];
	GObject* playbt = _data[4];
	GObject* player_bar = _data[5];
	pthread_t* Thread = _data[6];
	int* pipearr = _data[7];

	gtk_range_set_range(GTK_RANGE(player_bar), 0, song->seconds);

	// here was the create thread call
	write(pipearr[1], "\1", 1);

	string s = string_init("load ");
	string_append(&s, path); string_append_char(&s, '/');
	string_append(&s, song->file); string_append_char(&s, '\n');
	fputs(string_get_c_str(&s), P);
	fflush(P);
	gtk_button_set_label(GTK_BUTTON(playbt), "󰏤");
	gtk_label_set_label(GTK_LABEL(player_song_name), song->name);
}

