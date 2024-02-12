#include <gtk/gtk.h>
#include <dirent.h>
#include <string.h>
#include "vector.h"

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

int scan_dir(char* path) {
	DIR* dir = opendir(path);
	struct dirent* dt;
	int c=0;
	while ((dt = readdir(dir))) {
		if (!endswith(dt->d_name, ".mp3") && !endswith(dt->d_name, ".wav")) continue;
		/*
		 * add song to artists
		 */
		c++;
	}
	return c;
}

GtkWidget* MusicItem() {
	GtkWidget* wdg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_name(wdg, "wdg");

	GtkWidget* mname = gtk_label_new("Song");

	gtk_container_add(GTK_CONTAINER(wdg), mname);
	gtk_widget_show_all(wdg);
	return wdg;
}

void activate(GtkApplication* app, gpointer data) {

	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	int res = gtk_builder_add_from_file(builder, "gtk.ui", &error);
	GObject* window = gtk_builder_get_object(builder, "mainwindow");
	GObject* mainlayout = gtk_builder_get_object(builder, "mainlayout");
	GObject* nosongbox = gtk_builder_get_object(builder, "nosongbox");
	GObject* nosongicon = gtk_builder_get_object(builder, "nosongicon");

	GdkPixbuf* imgbuff = gdk_pixbuf_new_from_file_at_size("music.png", 120, 120, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(nosongicon), imgbuff);

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "gtk.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	Playlists.name = vector_init(VSTRLIST);
	Playlists.songs = vector_init(VGEN);
	vector Artists = vector_init(VGEN);

	if (!scan_dir(".")) {
		gtk_widget_set_visible(GTK_WIDGET(nosongbox), 1);
	} else {
		/*Render list of songs on dir*/
		gtk_container_add(GTK_CONTAINER(mainlayout), MusicItem());
		gtk_container_add(GTK_CONTAINER(mainlayout), MusicItem());
	}
}

int main(int argc, char** argv) {
	GtkApplication* app = gtk_application_new("org.hu.gtktest", G_APPLICATION_DEFAULT_FLAGS);
	int status;

	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	//g_object_unref(app);
}
