#include <gtk/gtk.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libvector/vector.h"
#include "tagreader/tagreader.h"
#include "tagreader/duration.h"

struct Song {
	char* name;
	char* duration;
	char* year;
	char* file;
};

struct Album {
	char* name;
	vector songs;
};

struct Artist {
	char* name;
	vector albums;
};

struct {
	vector name;
	vector songs;
} Playlists;

int endswith(char* string, char* p) {
	char* tmp = strrchr(string, '.');
	if (tmp && !strcmp(tmp, p)) return 1;
	return 0;
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


void play_pause(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	GtkWidget* playbt = GTK_WIDGET((GObject*)_data[1]);
	gtk_button_set_label(GTK_BUTTON(playbt), "󰏤");
	fputs("pause\n", P);
	fflush(P);
}

void load_song(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = (FILE*)_data[0];
	char* song = (char*)_data[1];
	char* path = (char*)_data[2];
	string s = string_init("load ");
	string_append(&s, path); string_append_char(&s, '/');
	string_append(&s, song); string_append_char(&s, '\n');
	fputs(string_get_c_str(&s), P);
	fflush(P);
}

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
		char* Duration = second_to_str(duration(cstr));
		struct Song* song = malloc(sizeof(struct Song));
		song->name = tag.TIT2; song->duration = Duration;
		song->year = tag.TYER; song->file = dt->d_name;
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

GtkWidget* MusicItem(char* song, char* album, char* artist, char* duration) {
	GtkWidget* main = gtk_button_new();
	gtk_widget_set_name(main, "songbutton");
	GtkWidget* wdg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_name(wdg, "wdg");
	GtkWidget* vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_hexpand(vertbox, 1);

	GtkWidget* mname = gtk_label_new(song); gtk_widget_set_name(mname, "songname");
	gtk_label_set_xalign(GTK_LABEL(mname),0);
	string mp3data = string_init(NULL);string_append_fmt(&mp3data, "%s - %s", artist, album);
	GtkWidget* data = gtk_label_new(string_get_c_str(&mp3data));
	gtk_label_set_xalign(GTK_LABEL(data), 0);
	GtkWidget* wduration = gtk_label_new(duration);
	GtkWidget* songmenu = gtk_button_new_with_label("󰇙");
	gtk_widget_set_name(songmenu, "songmenu");

	gtk_container_add(GTK_CONTAINER(vertbox), mname);
	gtk_container_add(GTK_CONTAINER(vertbox), data);

	gtk_container_add(GTK_CONTAINER(wdg), vertbox);
	gtk_container_add(GTK_CONTAINER(wdg), wduration);
	gtk_container_add(GTK_CONTAINER(wdg), songmenu);
	gtk_container_add(GTK_CONTAINER(main), wdg);
	gtk_widget_show_all(main);
	return main;
}

void** new_data_arr(FILE* P, char* song, char* path) {
	void** da = malloc(sizeof(void*)*3);
	*da = P; *(da+1) = song; *(da+2) = path;
	return da;
}

void show_songs(GtkApplication* app, gpointer data) {
	void** _data = (void**)data;
	FILE* P = _data[0];
	vector* Artists = _data[1];
	char* PATH = _data[2];
	GtkWidget* songbox = _data[3];
	for (int i=0; i<Artists->memsize; i++) {
		struct Artist* _art = vector_get_generic_at(Artists, i);
		for (int e=0; e<_art->albums.memsize; e++) {
			struct Album* album = vector_get_generic_at(&_art->albums, e);
			for (int j=0; j<album->songs.memsize; j++) {
				struct Song* song = vector_get_generic_at(&album->songs, j);
				GtkWidget* musicItem = MusicItem(song->name, _art->name, album->name, song->duration);
				g_signal_connect(musicItem, "clicked", G_CALLBACK(load_song), new_data_arr(P, song->file, PATH));
				gtk_container_add(GTK_CONTAINER(songbox), musicItem);
			}
		}
	}
}

void activate(GtkApplication* app, gpointer data) {
	char* PATH = "music";
	FILE* P = (FILE*)data;
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	int res = gtk_builder_add_from_file(builder, "gtk.ui", &error);
	GObject* window = gtk_builder_get_object(builder, "mainwindow");

	GObject* songbox = gtk_builder_get_object(builder, "songbox");
	GObject* nosongbox = gtk_builder_get_object(builder, "nosongbox");

	GObject* nosongicon = gtk_builder_get_object(builder, "nosongicon");

	GObject* player_bar = gtk_builder_get_object(builder, "player_bar");
	GObject* playbt = gtk_builder_get_object(builder, "play");

	GObject* section1 = gtk_builder_get_object(builder, "section1");
	GObject* section2 = gtk_builder_get_object(builder, "section2");
	GObject* section3 = gtk_builder_get_object(builder, "section3");
	GObject* section4 = gtk_builder_get_object(builder, "section4");
	
	GtkAdjustment* Adj = gtk_adjustment_new(0, 0, 100, 1, 1, 0);
	gtk_range_set_adjustment(GTK_RANGE(player_bar), Adj);

	GdkPixbuf* imgbuff = gdk_pixbuf_new_from_file_at_size("media/music.png", 120, 120, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(nosongicon), imgbuff);

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "gtk.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	Playlists.name = vector_init(VSTRLIST);
	Playlists.songs = vector_init(VGEN);
	vector Artists = vector_init(VGEN);

	void* data_arr[] = {P, &Artists, PATH, songbox};
	void** pause_arr = malloc(sizeof(void*));
	pause_arr[0]=P;pause_arr[1]=playbt;

	if (!scan_dir(PATH, &Artists)) {
		gtk_widget_set_visible(GTK_WIDGET(nosongbox), 1);
	} else {
		/*Render list of songs on dir*/
		gtk_widget_set_visible(GTK_WIDGET(songbox), 1);	
		gtk_widget_set_name(GTK_WIDGET(section1), "btsec2");
		g_signal_connect(GTK_WIDGET(playbt), "clicked", G_CALLBACK(play_pause), pause_arr);
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
