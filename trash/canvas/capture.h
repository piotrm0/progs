#ifndef __capture_h__
#define __capture_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <assert.h>

#include <igl.h>
#include <util.h>

// based on apiexample.c from ffmpeg

struct SwsContext;

typedef struct _capture_context_image {
  u_int type;

} capture_context_image;

typedef struct _capture_context_video {
  AVCodec* codec;
  AVCodecContext* c;

  int i, out_size, size, x, y, outbuf_size;

  AVFrame* picture;
  AVFrame* picture_rgb;
  uint8_t* outbuf;
  uint8_t * picture_buf, * picture_buf_rgb;
  uint8_t* temp_buf;
  struct SwsContext* sws_context;

  u_int pix_fmt_src;
  u_int pix_fmt_target;

} capture_context_video;

typedef union {
    capture_context_image details_image;
    capture_context_video details_video;
} capture_context_detail;

typedef struct _capture_context {
  u_int type;

  u_int x,y,w,h;

  capture_context_detail* details;

  const char* filename;
  FILE* filehandle;
  
} capture_context;

void capture_init();

#define CAPTURE_IMAGE 1<<0
#define CAPTURE_IMAGE_PNG 1<<0

#define CAPTURE_VIDEO 1<<1
#define CAPTURE_VIDEO_MPEG 1<<0

#define CAPTURE_COMMON_VARS(f,x,y,w,h)			\
  const char* f, u_int x, u_int y, u_int w, u_int h

capture_context* capture_context_new(CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_context_new_image(CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_context_new_video(CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_context_new_image_png(CAPTURE_COMMON_VARS(f,x,y,w,h));

capture_context* capture_new(u_int ctype, u_int itype, CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_new_video(u_int stype, CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_new_image(u_int stype, CAPTURE_COMMON_VARS(f,x,y,w,h));
capture_context* capture_new_image_png(CAPTURE_COMMON_VARS(f,x,y,w,h));

// capture_snap dispatch
void capture_snap(capture_context* c);
void capture_snap_video(capture_context* c);
void capture_snap_image(capture_context* c);
void capture_snap_image_png(capture_context* c);

// capture_end dispatch
void capture_end(capture_context* c);
void capture_end_video(capture_context* c);
void capture_end_image(capture_context* c);
void capture_end_image_png(capture_context* c);

void capture_context_free(capture_context* c);
void capture_context_free_video(capture_context* c);
void capture_context_free_image(capture_context* c);
void capture_context_free_image_png(capture_context* c);

#define CAPTURE_DISPATCH(ret, subfun)			\
  ret capture_##subfun(capture_context* c) {            \
    switch (c->type) {					\
    case CAPTURE_VIDEO:					\
      return capture_##subfun##_video(c);		\
      break;						\
    case CAPTURE_IMAGE:					\
      capture_##subfun##_image(c);			\
      break;						\
    default:						\
      pexit(1, "capture_" #subfun "(CAPTURE_DISPATCH): unknown capture type %d", c->type);	\
    }							\
  }

#endif /* __capture_h__ */
