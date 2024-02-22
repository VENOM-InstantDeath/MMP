#include <gtk/gtk.h>
#include <dirent.h>
#include <string.h>
#include "libvector/vector.h"
#include "tagreader/tagreader.h"

struct Song {
	char* name;
	char* duration;
	char* year;
	char* file;
};

struct Album {
	char* name;
	char** songs;
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

int scan_dir(char* path, vector* Artists) {
	DIR* dir = opendir(path);
	struct dirent* dt;
	int c=0;
	while ((dt = readdir(dir))) {
		if (!endswith(dt->d_name, ".mp3")) continue;
		/*
		 * add song to artists
		 */
		c++;
	}
	return c;
}

GtkWidget* MusicItem(char* song, char* album, char* artist, char* duration) {
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
	gtk_widget_show_all(wdg);
	return wdg;
}

void activate(GtkApplication* app, gpointer data) {

	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	int res = gtk_builder_add_from_file(builder, "gtk.ui", &error);
	GObject* window = gtk_builder_get_object(builder, "mainwindow");
	GObject* songbox = gtk_builder_get_object(builder, "songbox");
	GObject* nosongbox = gtk_builder_get_object(builder, "nosongbox");
	GObject* nosongicon = gtk_builder_get_object(builder, "nosongicon");

	GdkPixbuf* imgbuff = gdk_pixbuf_new_from_file_at_size("media/music.png", 120, 120, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(nosongicon), imgbuff);

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "gtk.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	Playlists.name = vector_init(VSTRLIST);
	Playlists.songs = vector_init(VGEN);
	vector Artists = vector_init(VGEN);

	if (!scan_dir("music", &Artists)) {
		gtk_widget_set_visible(GTK_WIDGET(nosongbox), 1);
	} else {
		/*Render list of songs on dir*/
		gtk_widget_set_visible(GTK_WIDGET(songbox), 1);
		gtk_container_add(GTK_CONTAINER(songbox), MusicItem("Unity", "TheFatRat", "Unity", "03:00"));
		gtk_container_add(GTK_CONTAINER(songbox), MusicItem("Serpentine", "Disturbed", "Asylum", "04:09"));
	}
}

int main(int argc, char** argv) {
	GtkApplication* app = gtk_application_new("org.hu.gtktest", G_APPLICATION_DEFAULT_FLAGS);
	int status;

	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	//g_object_unref(app);
}
