#include <gtk/gtk.h>
#include <dirent.h>
#include <string.h>
#include "vector.h"

struct {
	char** name;
	char** songs;
} Playlists;

struct {
	struct {
		char** songs;
	} *albums;
} Artists;


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

int main(int argc, char** argv) {
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;

	gtk_init(&argc, &argv);
	int res = gtk_builder_add_from_file(builder, "gtk.ui", &error);
	GObject* window = gtk_builder_get_object(builder, "mainwindow");
	GObject* nosongbox = gtk_builder_get_object(builder, "nosongbox");
	GObject* nosongicon = gtk_builder_get_object(builder, "nosongicon");

	GdkPixbuf* imgbuff = gdk_pixbuf_new_from_file_at_size("music.png", 120, 120, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(nosongicon), imgbuff);

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "gtk.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	puts("\tAbout to start main");

	if (!scan_dir(".")) {
		gtk_widget_set_visible(GTK_WIDGET(nosongbox), 1);
	}

	gtk_main();
}
