#ifndef DURATION_H
#define DURATION_H

struct frame_data {
	int crc;
	int bitrate;
	int samplerate;
	int padding;
};


int get_bitrate(unsigned char c);
int get_samplerate(unsigned char c);
char* second_to_str(int sec);
int duration(char* path);

#endif
