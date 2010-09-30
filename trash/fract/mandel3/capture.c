#include "capture.h"
 
void capture_init() {
  avcodec_init();
  avcodec_register_all();
}

capture_context* capture_context_new(CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  capture_context* context = (capture_context*) malloc(sizeof(capture_context));

  context->filename = f;

  context->filehandle = fopen(context->filename, "wb");

  if (! context->filehandle) {
    pexit(1, "capture_context_new: could not open %s\n", context->filename);
  }

  context->x = x;
  context->y = y;
  context->w = w;
  context->h = h;

  return context;
}

capture_context* capture_context_new_image(CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  capture_context* context = capture_context_new(f,x,y,w,h);
  context->details = (capture_context_detail*) malloc(sizeof(capture_context_detail));
  return context;
}

capture_context* capture_context_new_image_png(CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  capture_context* context = capture_context_new_image(f,x,y,w,h);
  return context;
}

capture_context* capture_context_new_video(CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  capture_context* context = capture_context_new(f,x,y,w,h);

  context->type = CAPTURE_VIDEO;

  return context;
}

capture_context* capture_new(u_int ctype, u_int stype, CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  switch (ctype) {
  case CAPTURE_IMAGE:
    return capture_new_image(stype, f,x,y,w,h);
  case CAPTURE_VIDEO:
    return capture_new_video(stype, f,x,y,w,h);
  default:
    pexit(1, "capture_new: unknown capture type %d", ctype);
  }

  return NULL;
}

capture_context* capture_new_image(u_int stype, CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  switch (stype) {
  case CAPTURE_IMAGE_PNG:
    return capture_new_image_png(f,x,y,w,h);
  default:
    pexit(1, "capture_new_image: unknown capture type %d", stype);
  }

  return NULL;
}

capture_context* capture_new_image_png(CAPTURE_COMMON_VARS(f,x,y,w,h)) {
  capture_context* context = capture_context_new_image(f,x,y,w,h);
  return context;
}

capture_context* capture_new_video(u_int stype, CAPTURE_COMMON_VARS(filename, x, y, width, height)) {
  u_int size;
  capture_context_video* video;

  capture_context* context = capture_context_new_video(filename, x, y, width, height);
    
  context->details = (capture_context_detail*) malloc(sizeof(capture_context_detail));

  video = (capture_context_video*) context->details;

  video->codec = avcodec_find_encoder(CODEC_ID_MPEG2VIDEO);

  //temp->codec = avcodec_find_encoder(CODEC_ID_XVID);
  //temp->codec = avcodec_find_encoder(CODEC_ID_MPEG4);
  //temp->codec = avcodec_find_encoder(CODEC_ID_MPEG4);
  //temp->codec = avcodec_find_encoder(CODEC_ID_H263P);

  assert(video->codec);

  video->pix_fmt_src    = PIX_FMT_RGB24;
  //video->pix_fmt_target = PIX_FMT_YUV420P;
  video->pix_fmt_target = PIX_FMT_YUV422P;

  assert(video->c = avcodec_alloc_context());

  assert(video->picture = avcodec_alloc_frame());
  assert(video->picture_rgb = avcodec_alloc_frame());

  video->c->bit_rate = 40000000;
  video->c->width    = width;
  video->c->height   = height;
  video->c->time_base    = (AVRational){1,25};
  video->c->gop_size     = 10;
  video->c->max_b_frames = 0;
  video->c->pix_fmt      = video->pix_fmt_target;

  assert(video->temp_buf = (uint8_t*) malloc(video->c->width * 3));

  if (avcodec_open(video->c, video->codec) < 0) {
    pexit(1, "capture_new_video: could not open codec");
  }

  video->outbuf_size = 10000000;

  assert(video->outbuf = (uint8_t*) malloc(video->outbuf_size));

  size = avpicture_get_size(video->pix_fmt_target, video->c->width, video->c->height);
  assert(video->picture_buf = (uint8_t*) malloc(size));
  avpicture_fill((AVPicture*) video->picture, video->picture_buf, video->pix_fmt_target, video->c->width, video->c->height);

  printf("size = %d\n", size);

  size = avpicture_get_size(video->pix_fmt_src, video->c->width, video->c->height);
  assert(video->picture_buf_rgb = (uint8_t*) malloc(size));
  avpicture_fill((AVPicture*) video->picture_rgb, video->picture_buf_rgb, video->pix_fmt_src, video->c->width, video->c->height);

  video->sws_context = sws_getContext(video->c->width, video->c->height,
				      video->pix_fmt_src,
				      video->c->width, video->c->height,
				      video->pix_fmt_target,
				      SWS_POINT, 0, 0, 0);

  assert(video->sws_context);

  return context;
}

CAPTURE_DISPATCH(void, snap)

void capture_snap_video(capture_context* context) {
  //u_int j;
  //uint8_t* row1;
  //uint8_t* row2;
  u_int row_size;

  capture_context_video* video = (capture_context_video*) context->details;

  glReadPixels(0,0,context->w,context->h,GL_RGB,GL_BYTE,video->picture_buf_rgb);

  row_size = video->c->width * 3;

  printf("encoding frame %3d ... ", video->i++);

  /*
  for (j = 0; j < c->c->height / 2; j++) {
    row1 = c->picture_buf_rgb + j * row_size;
    row2 = c->picture_buf_rgb + (c->c->height - j) * row_size;

    memcpy(c->temp_buf, row2, row_size);
    memcpy(row2, row1, row_size);
    memcpy(row1, c->temp_buf, row_size);
  }
  */

  //img_convert((AVPicture*) video->picture, PIX_FMT_YUV420P, (AVPicture*) video->picture_rgb, PIX_FMT_RGB24, c->c->width, c->c->height);

  sws_scale(video->sws_context,
	    video->picture_rgb->data, video->picture_rgb->linesize,
	    0, video->c->height,
	    video->picture->data, video->picture->linesize);

  video->out_size = avcodec_encode_video(video->c, video->outbuf, video->outbuf_size, video->picture);

  printf("writing (size=%5d)\n", video->out_size);

  fwrite(video->outbuf, 1, video->out_size, context->filehandle);
}

void capture_snap_image(capture_context* c) {

}

void capture_end_video(capture_context* c) {
  capture_context_video* video = (capture_context_video*) c->details;

  for(; video->out_size; video->i++) {
    video->out_size = avcodec_encode_video(video->c, video->outbuf, video->outbuf_size, NULL);
    printf("write frame %3d (size=%5d)\n", video->i, video->out_size);
    fwrite(video->outbuf, 1, video->out_size, c->filehandle);
  }
  video->outbuf[0] = 0x00;
  video->outbuf[1] = 0x00;
  video->outbuf[2] = 0x01;
  video->outbuf[3] = 0xb7;

  fwrite(video->outbuf, 1, 4, c->filehandle);


  av_free(video->c);
  av_free(video->picture);
  av_free(video->picture_rgb);

  avcodec_close(video->c);

  printf("\n");

  capture_context_free_video(c);

  capture_end(c);
}

void capture_end(capture_context* c) {
  fclose(c->filehandle);

  capture_context_free(c);
}

void capture_context_free_video(capture_context* c) {
  free(((capture_context_video*)c->details)->picture_buf);
  free(((capture_context_video*)c->details)->picture_buf_rgb);
  free(((capture_context_video*)c->details)->outbuf);

  free(c->details);
}

void capture_context_free(capture_context* c) {
  free(c);
}
