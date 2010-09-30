#include "capture.h"
 
void capture_init() {
  avcodec_init();
  avcodec_register_all();
}

capture* capture_new(const char* filename, u_int x, u_int y, u_int width, u_int height) {
  u_int size;
  capture* temp;

  temp = (capture*) malloc(sizeof(capture));
  temp->filename = filename;
  
  temp->codec = avcodec_find_encoder(CODEC_ID_MPEG2VIDEO);
  // temp->codec = avcodec_find_encoder(CODEC_ID_MPEG4);
  // temp->codec = avcodec_find_encoder(CODEC_ID_H263P);
  if (!temp->codec) {
    fprintf(stderr, "codec not found\n");
    exit(1);
  }

  temp->c = avcodec_alloc_context();
  temp->picture = avcodec_alloc_frame();
  temp->picture_rgb = avcodec_alloc_frame();

  temp->c->bit_rate = 40000000;
  temp->c->width = width;
  temp->c->height = height;
  temp->c->time_base= (AVRational){1,25};
  temp->c->gop_size = 10;
  temp->c->max_b_frames=0;
  temp->c->pix_fmt = PIX_FMT_YUV420P;
  // temp->c->pix_fmt = PIX_FMT_YUV422P;

  temp->temp_buf = (uint8_t*) malloc(temp->c->width * 3);

  if (avcodec_open(temp->c, temp->codec) < 0) {
    fprintf(stderr, "could not open codec\n");
    exit(1);
  }
  
  temp->f = fopen(temp->filename, "wb");
  if (!temp->f) {
    fprintf(stderr, "could not open %s\n", temp->filename);
    exit(1);
  }

  temp->outbuf_size = 10000000;
  temp->outbuf = (uint8_t*) malloc(temp->outbuf_size);

  size = avpicture_get_size(PIX_FMT_YUV420P, temp->c->width, temp->c->height);
  temp->picture_buf = (uint8_t*) malloc(size);
  avpicture_fill((AVPicture*) temp->picture, temp->picture_buf, PIX_FMT_YUV420P, temp->c->width, temp->c->height);

  printf("size = %d\n", size);

  size = avpicture_get_size(PIX_FMT_RGB24, temp->c->width, temp->c->height);
  temp->picture_buf_rgb = (uint8_t*) malloc(size);
  avpicture_fill((AVPicture*) temp->picture_rgb, temp->picture_buf_rgb, PIX_FMT_RGB24, temp->c->width, temp->c->height);

  return temp;
}

void capture_snap(capture* c) {
  u_int j;
  uint8_t* row1;
  uint8_t* row2;
  u_int row_size;

  row_size = c->c->width * 3;

  for (j = 0; j < c->c->height / 2; j++) {
    row1 = c->picture_buf_rgb + j * row_size;
    row2 = c->picture_buf_rgb + (c->c->height - j) * row_size;

    memcpy(c->temp_buf, row2, row_size);
    memcpy(row2, row1, row_size);
    memcpy(row1, c->temp_buf, row_size);
  }

  img_convert((AVPicture*) c->picture, PIX_FMT_YUV420P, (AVPicture*) c->picture_rgb, PIX_FMT_RGB24, c->c->width, c->c->height);

  c->out_size = avcodec_encode_video(c->c, c->outbuf, c->outbuf_size, c->picture);
  printf("encoding frame %3d (size=%5d)\n", c->i++, c->out_size);
  fwrite(c->outbuf, 1, c->out_size, c->f);
}

void capture_end(capture* c) {
  for(; c->out_size; c->i++) {
    c->out_size = avcodec_encode_video(c->c, c->outbuf, c->outbuf_size, NULL);
    printf("write frame %3d (size=%5d)\n", c->i, c->out_size);
    fwrite(c->outbuf, 1, c->out_size, c->f);
  }
  c->outbuf[0] = 0x00;
  c->outbuf[1] = 0x00;
  c->outbuf[2] = 0x01;
  c->outbuf[3] = 0xb7;

  fwrite(c->outbuf, 1, 4, c->f);

  fclose(c->f);
  free(c->picture_buf);
  free(c->picture_buf_rgb);
  free(c->outbuf);
  av_free(c->c);
  av_free(c->picture);
  av_free(c->picture_rgb);
  avcodec_close(c->c);

  printf("\n");

  free(c);
}

