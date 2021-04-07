// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Ulrich Hecht

#ifdef H3

#include "h3audio.h"
#include <h3_codec.h>
#include <audio.h>

H3Audio audio;

int H3Audio::m_curr_buf_pos;
sample_t *H3Audio::m_curr_buf;
static int m_read_buf;
static int m_read_pos;

sample_t H3Audio::m_sound_buf[2][SOUND_BUFLEN * SOUND_CHANNELS];
int H3Audio::m_block_size;

void H3Audio::timerInterrupt(H3Audio *audioOutput) {
}

void H3Audio::init(int sample_rate) {
  m_curr_buf_pos = 0;
  m_read_pos = 0;
  m_read_buf = 0;
  m_curr_buf = m_sound_buf[m_read_buf ^ 1];
  m_block_size = SOUND_BUFLEN;

  audio_start(SOUND_BUFLEN * SOUND_CHANNELS);
}

void hook_audio_get_sample(int16_t *l, int16_t *r) {
  *l = audio.m_sound_buf[m_read_buf][m_read_pos * SOUND_CHANNELS];
  *r = audio.m_sound_buf[m_read_buf][m_read_pos * SOUND_CHANNELS + 1];
  m_read_pos++;

  if (m_read_pos >= audio.m_block_size) {
    m_read_buf ^= 1;
    audio.m_curr_buf = audio.m_sound_buf[m_read_buf ^ 1];
    audio.m_curr_buf_pos = 0;
    m_read_pos = 0;

    h3_codec_push_data(audio.m_curr_buf);
  }
}

#endif	// H3
