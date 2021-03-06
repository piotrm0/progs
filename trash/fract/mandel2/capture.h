#ifndef __capture_h__
#define __capture_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <libavcodec/avcodec.h>

// based on apiexample.c from ffmpeg

typedef struct _capture {
  const char* filename;
  AVCodec* codec;
  AVCodecContext* c;
  int i, out_size, size, x, y, outbuf_size;
  FILE* f;
  AVFrame* picture;
  AVFrame* picture_rgb;
  uint8_t* outbuf;
  uint8_t* picture_buf, *picture_buf_rgb;
  uint8_t* temp_buf;
} capture;

void capture_init();

capture* capture_new(const char* filename, u_int x, u_int y, u_int width, u_int height);
void capture_snap(capture* c);
void capture_end(capture* c);

#endif /* __capture_h__ */
