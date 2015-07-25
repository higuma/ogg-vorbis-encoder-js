// adapted from https://svn.xiph.org/trunk/vorbis/examples/encoder_example.c
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vorbis/vorbisenc.h>

typedef struct encoder_state {
  ogg_stream_state os;
  ogg_packet op;
  ogg_page og;
  vorbis_info vi;
  vorbis_comment vc;
  vorbis_dsp_state vd;
  vorbis_block vb;
  unsigned char *data;
  long len;
} encoder_state;

static void encoder_add_data(encoder_state *enc);

encoder_state *encoder_init(int num_ch, float sample_rate, float quality) {
  ogg_packet h_comm, h_code;
  encoder_state *enc = malloc(sizeof(encoder_state));
  vorbis_info_init(&enc->vi);
  vorbis_encode_init_vbr(&enc->vi, num_ch, sample_rate, quality);
  vorbis_comment_init(&enc->vc);
  vorbis_comment_add_tag(&enc->vc, "ENCODER", "OggVorbisEncoder.js");
  vorbis_analysis_init(&enc->vd, &enc->vi);
  vorbis_block_init(&enc->vd, &enc->vb);
  srand(time(NULL));
  ogg_stream_init(&enc->os, rand());
  enc->data = NULL;
  enc->len = 0;
  vorbis_analysis_headerout(&enc->vd, &enc->vc, &enc->op, &h_comm, &h_code);
  ogg_stream_packetin(&enc->os, &enc->op);
  ogg_stream_packetin(&enc->os, &h_comm);
  ogg_stream_packetin(&enc->os, &h_code);
  while (ogg_stream_flush(&enc->os, &enc->og) != 0)
    encoder_add_data(enc);
  return enc;
}

void encoder_clear(encoder_state *enc) {
  ogg_stream_clear(&enc->os);
  vorbis_block_clear(&enc->vb);
  vorbis_dsp_clear(&enc->vd);
  vorbis_comment_clear(&enc->vc);
  vorbis_info_clear(&enc->vi);
  free(enc->data);
  free(enc);
}

float **encoder_analysis_buffer(encoder_state *enc, int length) {
  return vorbis_analysis_buffer(&enc->vd, length);
}

void encoder_process(encoder_state *enc, int length) {
  vorbis_analysis_wrote(&enc->vd, length);
  while (vorbis_analysis_blockout(&enc->vd, &enc->vb) == 1) {
    vorbis_analysis(&enc->vb, NULL);
    vorbis_bitrate_addblock(&enc->vb);
    while (vorbis_bitrate_flushpacket(&enc->vd, &enc->op)) {
      ogg_stream_packetin(&enc->os, &enc->op);
      while (ogg_stream_pageout(&enc->os, &enc->og) != 0)
        encoder_add_data(enc);
    }
  }
}

long encoder_data_len(encoder_state *enc) {
  return enc->len;
}

unsigned char *encoder_transfer_data(encoder_state *enc) {
  enc->len = 0;
  return enc->data;
}

void encoder_add_data(encoder_state *enc) {
  ogg_page *og = &enc->og;
  long len = enc->len + og->header_len + og->body_len;
  if (len == 0) return;
  enc->data = realloc(enc->data, len);
  memcpy(enc->data + enc->len, og->header, og->header_len);
  enc->len += og->header_len;
  memcpy(enc->data + enc->len, og->body, og->body_len);
  enc->len += og->body_len;
}
