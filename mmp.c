#include <gtk/gtk.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include "libvector/vector.h"
#include "tagreader/tagreader.h"
#include "tagreader/duration.h"

#include "structs.h"
#include "components.h"
#include "callbacks.h"
#include "utils.h"

struct {
	vector name;
	vector songs;
} Playlists;

int scan_dir(char* path, vector* Artists) {
	DIR* dir = opendir(path);
	struct dirent* dt;
	int c=0;
	while ((dt = readdir(dir))) {
		string str = string_init(path); string_append_char(&str, '/');
		string_append(&str, dt->d_name);
	char* cstr = string_get_c_str(&str);
		if (!endswith(cstr, ".mp3")) continue;
		/*
		 * add song to artists
		 */
		Id3v2Tag tag = tagreader_read_tags(cstr);
		int segs = duration(cstr);
		char* Duration = second_to_str(segs);
		struct Song* song = malloc(sizeof(struct Song));
		song->name = tag.TIT2; song->duration = Duration;
		song->year = tag.TYER; song->file = dt->d_name;
		song->seconds = segs;
		struct Artist* _art;
		if ((_art=find_artist(Artists, tag.TPE1)) == NULL) {
			struct Album* album = malloc(sizeof(struct Album));
			album->name = tag.TALB;
			album->songs = vector_init(VGEN);
			vector_append_generic(&album->songs, song);

			_art = malloc(sizeof(struct Artist));
			_art->name = tag.TPE1;
			_art->albums = vector_init(VGEN);
			vector_append_generic(&_art->albums, album);

			vector_append_generic(Artists, _art);
		} else {
			struct Album* album;
			if ((album = find_album(&_art->albums, tag.TALB)) == NULL) {
				album = malloc(sizeof(struct Album));
				album->name = tag.TALB;
				album->songs = vector_init(VGEN);
				vector_append_generic(&album->songs, song);

				vector_append_generic(&_art->albums, album);
			} else {
				vector_append_generic(&album->songs, song);
				vector_append_generic(&_art->albums, album);
	}
		}
		c++;
	}
	return c;
}

void show_playlists(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	struct Sections* Section = _data[3];
	GObject* player_song_name = _data[4];
	GObject* playbt = _data[5];
	GObject* player_bar = _data[6];
	pthread_t* Thread = _data[7];
	int* pipearr = _data[8];

	gtk_widget_set_visible(GTK_WIDGET(Section->sections[Section->index]), 0);
	gtk_widget_set_visible(GTK_WIDGET(Section->sections[PLAYLISTBOX]), 1);

	gtk_widget_set_name(GTK_WIDGET(Section->objects[3]), "btsec2");
	if (Section->index >= 4) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index-3]), "btsec");
	} else if (Section->index != 3) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index]), "btsec");
	}
	Section->index=3;
}

void show_artists(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	struct Sections* Section = _data[3];
	GObject* player_song_name = _data[4];
	GObject* playbt = _data[5];
	GObject* player_bar = _data[6];
	pthread_t* Thread = _data[7];
	int* pipearr = _data[8];

	gtk_widget_set_visible(GTK_WIDGET(Section->sections[Section->index]), 0);
	gtk_widget_set_visible(GTK_WIDGET(Section->sections[ARTISTBOX]), 1);

	gtk_widget_set_name(GTK_WIDGET(Section->objects[2]), "btsec2");
	if (Section->index >= 4) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index-3]), "btsec");
	} else if (Section->index != 2) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index]), "btsec");
	}
	Section->index=2;
}

void show_albums(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	struct Sections* Section = _data[3];
	GObject* player_song_name = _data[4];
	GObject* playbt = _data[5];
	GObject* player_bar = _data[6];
	pthread_t* Thread = _data[7];
	int* pipearr = _data[8];

	gtk_widget_set_visible(GTK_WIDGET(Section->sections[Section->index]), 0);
	gtk_widget_set_visible(GTK_WIDGET(Section->sections[ALBUMBOX]), 1);

	gtk_widget_set_name(GTK_WIDGET(Section->objects[1]), "btsec2");
	if (Section->index >= 4) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index-3]), "btsec");
	} else if (Section->index != 1) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index]), "btsec");
	}
	Section->index=1;
}

void show_songs(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	struct Sections* Section = _data[3];
	GObject* player_song_name = _data[4];
	GObject* playbt = _data[5];
	GObject* player_bar = _data[6];
	pthread_t* Thread = _data[7];
	int* pipearr = _data[8];

	if (Section->index != -1)
		gtk_widget_set_visible(GTK_WIDGET(Section->sections[Section->index]), 0);
	gtk_widget_set_visible(GTK_WIDGET(Section->sections[SONGBOX]), 1);

	gtk_widget_set_name(GTK_WIDGET(Section->objects[0]), "btsec2");
	if (Section->index >= 4) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index-3]), "btsec");
	} else if (Section->index != 0 && Section->index != -1) {
		gtk_widget_set_name(GTK_WIDGET(Section->objects[Section->index]), "btsec");
	}
	Section->index=0;
}

void populate_sections(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	struct Sections* Section = _data[3];
	GObject* player_song_name = _data[4];
	GObject* playbt = _data[5];
	GObject* player_bar = _data[6];
	pthread_t* Thread = _data[7];
	int* pipearr = _data[8];
	for (int i=0; i<Artists->memsize; i++) {
		struct Artist* _art = vector_get_generic_at(Artists, i);
		for (int e=0; e<_art->albums.memsize; e++) {
			struct Album* album = vector_get_generic_at(&_art->albums, e);
			for (int j=0; j<album->songs.memsize; j++) {
				struct Song* song = vector_get_generic_at(&album->songs, j);
				GtkWidget* musicItem = MusicItem(song->name, _art->name, album->name, song->duration);
				g_signal_connect(musicItem, "clicked", G_CALLBACK(load_song), new_data_arr(8, P, song, PATH, player_song_name, playbt, player_bar, Thread, pipearr));
				gtk_container_add(GTK_CONTAINER(Section->sections[SONGBOX]), musicItem);
			}
		}
	}

	//ALBUMS
	for (int i=0; i<Artists->memsize; i++) {
		struct Artist* _art = vector_get_generic_at(Artists, i);
		for (int e=0; e<_art->albums.memsize; e++) {
			struct Album* album = vector_get_generic_at(&_art->albums, e);
			GtkWidget* albumItem = AlbumItem(album->name, _art->name, album->songs.memsize);
			//g_signal_connect(albumItem, "clicked", G_CALLBACK(show_in_album), new_data_arr(8, P, song, PATH, player_song_name, playbt, player_bar, Thread, pipearr));
			gtk_container_add(GTK_CONTAINER(Section->sections[ALBUMBOX]), albumItem);
		}
	}

	//ARTIST
	/*for (int i=0; i<Artists->memsize; i++) {
		struct Artist* _art = vector_get_generic_at(Artists, i);
		GtkWidget* artistItem = ArtistItem();
		g_signal_connect(artistItem, "clicked", G_CALLBACK(show_in_artist), new_data_arr(8, P, song, PATH, player_song_name, playbt, player_bar, Thread, pipearr));
	}*/

	//PLAYLIST -- NEED TO LOAD DATA FILE
}

gchar* scalecb(GtkScale* self, gdouble value) {
	return second_to_str(value);
}

void activate(GtkApplication* app, gpointer data) {
	char* PATH = "music";
	FILE* P = (FILE*)data;
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	int res = gtk_builder_add_from_file(builder, "mmp.ui", &error);
	GObject* window = gtk_builder_get_object(builder, "mainwindow");

	GObject* songbox = gtk_builder_get_object(builder, "songbox");
	GObject* albumbox = gtk_builder_get_object(builder, "albumbox");
	GObject* inalbumbox = gtk_builder_get_object(builder, "inalbumbox");
	GObject* artistbox = gtk_builder_get_object(builder, "artistbox");
	GObject* inartistbox = gtk_builder_get_object(builder, "inartistbox");
	GObject* playlistbox = gtk_builder_get_object(builder, "playlistbox");

	GObject* nosongbox = gtk_builder_get_object(builder, "nosongbox");

	GObject* nosongicon = gtk_builder_get_object(builder, "nosongicon");

	GObject* player_bar = gtk_builder_get_object(builder, "player_bar");
	GObject* player_song_name = gtk_builder_get_object(builder, "player_song_name");
	GObject* playbt = gtk_builder_get_object(builder, "play");

	GObject* section1 = gtk_builder_get_object(builder, "section1");
	GObject* section2 = gtk_builder_get_object(builder, "section2");
	GObject* section3 = gtk_builder_get_object(builder, "section3");
	GObject* section4 = gtk_builder_get_object(builder, "section4");
	
	g_signal_connect(GTK_WIDGET(player_bar), "format-value", G_CALLBACK(scalecb), NULL);

	GdkPixbuf* imgbuff = gdk_pixbuf_new_from_file_at_size("media/music.png", 120, 120, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(nosongicon), imgbuff);

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "mmp.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	Playlists.name = vector_init(VSTRLIST);
	Playlists.songs = vector_init(VGEN);
	vector* Artists = new_generic_vector();

	pthread_t Thread;
	int* pipearr = malloc(sizeof(int)*2);
	pipe(pipearr);
	pthread_create(&Thread, NULL, thread_counter, new_data_arr(2, player_bar, pipearr));

	struct Sections* Section = malloc(sizeof(struct Sections));
	Section->sections = new_gobject_arr(6, songbox, albumbox, artistbox, playlistbox, inalbumbox, inartistbox);
	Section->objects = new_gobject_arr(4, section1, section2, section3, section4);
	Section->index = -1;
	void** data_arr = new_data_arr(9, P, Artists, PATH, Section, player_song_name, playbt, player_bar, &Thread, pipearr);

	if (!scan_dir(PATH, Artists)) {
		gtk_widget_set_visible(GTK_WIDGET(nosongbox), 1);
	} else {
		/*Render list of songs on dir*/
		g_signal_connect(GTK_WIDGET(section1), "clicked", G_CALLBACK(show_songs), data_arr);
		g_signal_connect(GTK_WIDGET(section2), "clicked", G_CALLBACK(show_albums), data_arr);
		g_signal_connect(GTK_WIDGET(section3), "clicked", G_CALLBACK(show_artists), data_arr);
		g_signal_connect(GTK_WIDGET(section4), "clicked", G_CALLBACK(show_playlists), data_arr);
		g_signal_connect(GTK_WIDGET(playbt), "clicked", G_CALLBACK(play_pause), new_data_arr(3, P, playbt, pipearr));
		g_signal_connect(GTK_WIDGET(player_bar), "change-value", G_CALLBACK(change_value), new_data_arr(2, pipearr, P));
		populate_sections(app, data_arr);
		show_songs(app, data_arr);
	}
}

int main(int argc, char** argv) {
	GtkApplication* app = gtk_application_new("org.hu.gtktest", G_APPLICATION_DEFAULT_FLAGS);
	int status;
	FILE* P = popen("mpg123 -R", "w");
	g_signal_connect(app, "activate", G_CALLBACK(activate), P);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	//g_object_unref(app);
}
