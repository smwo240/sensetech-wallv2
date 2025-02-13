#ifndef MP3_H
#define MP3_H

// AUDIO LIBRARY
typedef enum {
    BUBBLEPOP,
    CLAIRDELUNE
} SOUNDS;

typedef struct {
    uint16_t folder;
    uint16_t file;
} sound_id;

static const sound_id MP3_LIBRARY[] = {
    [BUBBLEPOP] = { .folder = 0x01, .file = 0x01 },
    [CLAIRDELUNE] = { .folder = 0x01, .file = 0x02 }
};

void mp3_initialize();
uint8_t* mp3_send_command(uint8_t command, uint16_t param);
void mp3_query_status();
void mp3_play_sound(SOUNDS sound);
void mp3_set_volume(uint8_t volume);
bool mp3_busy();

#endif // MP3_H