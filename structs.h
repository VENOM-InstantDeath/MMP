#ifndef STRUCTS_H
#define STRUCTS_H

#include "libvector/vector.h"
#include <gtk/gtk.h>

struct Song {
	char* name;
	char* duration;
	char* year;
	char* file;
	int seconds;
};

struct Album {
	char* name;
	vector songs;
};

struct Artist {
	char* name;
	vector albums;
};

struct Sections {
	GObject** objects;
	GObject** sections;
	int index;
};

enum SectionMembers {
	SONGBOX,
	ALBUMBOX,
	ARTISTBOX,
	PLAYLISTBOX,
	INALBUMBOX,
	INARTISTBOX
};

#endif
