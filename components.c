#include <gtk/gtk.h>
#include "components.h"
#include "libvector/vector.h"
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

GtkWidget* AlbumItem(char* album, char* artist, int numsongs) {
	GtkWidget* main = gtk_button_new();
	gtk_widget_set_name(main, "songbutton");
	GtkWidget* wdg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_name(wdg, "wdg");
	GtkWidget* vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_hexpand(vertbox, 1);

	GtkWidget* mname = gtk_label_new(album); gtk_widget_set_name(mname, "songname");
	gtk_label_set_xalign(GTK_LABEL(mname),0);
	string mp3data = string_init(NULL);string_append_fmt(&mp3data, "%s - %d songs", artist, numsongs);
	GtkWidget* data = gtk_label_new(string_get_c_str(&mp3data));
	gtk_label_set_xalign(GTK_LABEL(data), 0);

	gtk_container_add(GTK_CONTAINER(vertbox), mname);
	gtk_container_add(GTK_CONTAINER(vertbox), data);

	gtk_container_add(GTK_CONTAINER(wdg), vertbox);
	gtk_container_add(GTK_CONTAINER(main), wdg);
	gtk_widget_show_all(main);
	return main;
}
