#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "duration.h"

int get_bitrate(unsigned char c) {
	switch (c) {
		case 16: return 32;
		case 32: return 40;
		case 48: return 48;
		case 64: return 56;
		case 80: return 64;
		case 96: return 80;
		case 112: return 96;
		case 128: return 112;
		case 144: return 128;
		case 160: return 160;
		case 176: return 192;
		case 192: return 224;
		case 208: return 256;
		case 224: return 320;
		default: return -1;
	}
}

int get_samplerate(unsigned char c) {
	switch (c) {
		case 0: return 44100;
		case 4: return 48000;
		case 8: return 32000;
		default: return -1;
	}
}

void write_h_to_str(int h, int m, int s, char* str) {
	str[1] = (h%10)+'0'; h/=10;
	str[0] = (h%10)+'0';
	str[4] = (m%10)+'0'; m/=10;
	str[3] = (m%10)+'0';
	str[7] = (s%10)+'0'; s/=10;
	str[6] = (s%10)+'0';
}

void write_m_to_str(int m, int s, char* str) {
	str[1] = (m%10)+'0'; m/=10;
	str[0] = (m%10)+'0';
	str[4] = (s%10)+'0'; s/=10;
	str[3] = (s%10)+'0';
}

char* second_to_str(int sec) {
	int h=sec/3600;
	int x= (sec-3600*h);
	int m = x/60;
	int s = x%60;
	char* str;
	if (h) {
		if (h > 99) {return NULL;}
		str = calloc(9, 1);
		strcpy(str, "00:00:00");
		write_h_to_str(h, m, s, str);
	} else {
		str = calloc(6, 1);
		strcpy(str, "00:00");
		write_m_to_str(m, s, str);
    	}
    	return str; 
}

int read_header(unsigned char* buffer, int p, struct frame_data* frdt) {
	if (buffer[p]!=255 || (buffer[p+1]&250)!=250) {return 0;}
	if ((buffer[p+1]&1) != 1) {frdt->crc=1;}
	else {frdt->crc=0;}
	frdt->bitrate = get_bitrate(buffer[p+2]&240);
	frdt->samplerate = get_samplerate(buffer[p+2]&12);
	if (frdt->bitrate == -1 || frdt->samplerate == -1) {return 0;}
	if ((buffer[p+2]&2)==2) {frdt->padding=1;}
	else {frdt->padding=0;}
	return 1;
}

int duration(char* path) {
	FILE* F = fopen(path, "rb");
	struct stat st; stat(path, &st);
	unsigned char* buffer = malloc(st.st_size);
	fread(buffer, 1, st.st_size, F);
	fclose(F);
	int c=0;
	int p=0;
	struct frame_data data = {0,0,0,0};
	for (int i=0; i<st.st_size; i++) {
		if (buffer[i] == 255) {p=i;break;}
	}
	while (p != st.st_size) {
		read_header(buffer, p, &data);
		int frame_length = (int)((144.0*data.bitrate/data.samplerate*1000)+data.padding);
		p += frame_length;
		if (data.crc) {p += 2;}
		c++;
	}
	free(buffer);
	return c*1152/data.samplerate;
}
