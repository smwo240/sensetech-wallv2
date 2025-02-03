#ifndef MP3_H
#define MP3_H

// AUDIO LIBRARY
typedef enum {
    BUBBLEPOP_01,
    BUBBLEPOP_02,
    BUBBLEPOP_03,
    BUBBLEPOP_04
} SOUNDS;

typedef struct {
    uint16_t folder;
    uint16_t file;
} sound_id;

static const sound_id MP3_LIBRARY[] = {
    [BUBBLEPOP_01] = { .folder = 0x01, .file = 0x01 },
    [BUBBLEPOP_02] = { .folder = 0x01, .file = 0x02 },
    [BUBBLEPOP_03] = { .folder = 0x01, .file = 0x03 },
    [BUBBLEPOP_04] = { .folder = 0x01, .file = 0x04 }
};

void mp3_initialize();
uint8_t* mp3_send_command(uint8_t command, uint16_t param);
void mp3_query_status();
void mp3_play_sound(SOUNDS sound);
void mp3_set_volume(uint8_t volume);
bool mp3_busy();

#endif // MP3_H