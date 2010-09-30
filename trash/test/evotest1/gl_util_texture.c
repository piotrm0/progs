#include "gl_util_texture.h"

// load a 256x256 RGB .RAW file as a texture
// http://www.nullterminator.net/gltexture.html
GLuint gl_load_texture_raw(const char * filename, u_int width, u_int height, u_int alpha) {
  char* data;
  FILE* file;

  printf("loading raw texture [%s]\n", filename);

  // open texture data
  file = fopen(filename, "rb");
  if ( file == NULL ) return 0;

  // allocate buffer
  //  width = 256;
  //  height = 256;
  width = 512;
  height = 512;

  data = (char*) malloc(width * height * (alpha ? 4 : 3));
  if (data == NULL)
    pexit(errno, "gl_load_texture_raw: can't allocate space for image data");

  // read texture data
  fread(data, width * height * (alpha ? 4 : 3), 1, file );
  fclose(file);

  return gl_load_texture_from_data(data, 512, 512, (alpha ? 4 : 3));
}

char* texture_deinterlace(char** data, u_int width, u_int height, u_int size) {
  u_int y;

  char* d = (char*) calloc(width * height, size * sizeof(char));
  if (d == NULL)
    pexit(errno, "texture_deinterlace: can't allocate space for image data");

  //  printf("in texture deinterlace size:[%d]\n", size);

  for (y = 0; y < height; y++)
    memcpy(d + size * sizeof(char) * y * width, data[y], size * sizeof(char) * width);

  //  printf("end deinterlace\n");

  return d;
}

GLuint gl_load_texture_from_data(char* data, u_int width, u_int height, u_int alpha) {
  GLuint texture;
  int wrap = 0;
  
  //  printf("in load from data alpha [%d]\n", alpha);

  // allocate a texture name
  glGenTextures(1, &texture);

  // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );

  // select modulate to mix texture with color for shading
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

  // when texture area is small, bilinear filter the closest mipmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first mipmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		   wrap ? GL_REPEAT : GL_CLAMP );

  // build our texture mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, (alpha ? 4 : 3), width, height,
		    (alpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);

  // free buffer
  //  free(data);

  return texture;
}

GLuint gl_load_texture_png(const char* filename, u_int alpha) {
  FILE* fp;
  char* header;
  u_int width, height, y;
  u_int number_of_passes;
  png_byte color_type, bit_depth;
  png_structp png_ptr;
  png_infop info_ptr, end_info;
  png_bytep* row_pointers;

  header = (char*) calloc(8, sizeof(char));
  if (header == NULL)
    pexit(errno, "gl_load_texture_png: can't allocate space for png header");

  printf("loading png texture [%s]\n", filename);

  fp = fopen(filename, "rb");
  if (!fp) pexit(EIO, "gl_load_texture_png: can't open file");

  fread(header, 1, 8, fp);

  if (png_sig_cmp(header, (png_size_t) 0, (png_size_t) 8)) pexit(0, "gl_load_texture_png: not a png file");

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) pexit(0, "gl_load_texture_png: failed to make png pointer");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
      png_destroy_read_struct(&png_ptr,
			      (png_infopp)NULL, (png_infopp)NULL);
      pexit(0, "gl_load_texture_png: failed to make png info pointer");
    }

  end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
      png_destroy_read_struct(&png_ptr, &info_ptr,
			      (png_infopp)NULL);
      pexit(0, "gl_load_texture_png: failed to make png end info pointer");
  }

  if(setjmp(png_jmpbuf(png_ptr)))
    pexit(0, "gl_load_texture_png: random png reading error");

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  width = info_ptr->width;
  height = info_ptr->height;
  color_type = info_ptr->color_type;
  bit_depth = info_ptr->bit_depth;

  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  /* read file */
  if (setjmp(png_jmpbuf(png_ptr))) pexit(0, "gl_load_texture_png: random png reading error");

  //  printf("read png info: [%d,%d,%d] %d\n", width, height, (u_int) bit_depth, (u_int) color_type);
  //  printf("rowbytes: %d\n", info_ptr->rowbytes);

  row_pointers = (png_bytep*) calloc(height, sizeof(png_bytep));
  if (row_pointers == NULL)
    pexit(errno, "gl_load_texture_png: can't allocate space for image rows");

  for (y=0; y<height; y++) {
    row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
    if (row_pointers[y] == NULL)
      pexit(errno, "gl_load_texture_png: can't allocate space for image data");
  }

  png_read_image(png_ptr, row_pointers);

  fclose(fp);

  //  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
  //  row_pointers = png_get_rows(png_ptr, info_ptr);
  //  png_bytep row_pointers[height];

  return gl_load_texture_from_data(texture_deinterlace((char**) row_pointers,
						       width,
						       height,
						       (alpha ? 4 : 3)),
				   width,
				   height,
				   alpha);
}

