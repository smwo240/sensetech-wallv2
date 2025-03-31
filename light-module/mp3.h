#ifndef MP3_H
#define MP3_H

// AUDIO LIBRARY
typedef enum {
    BUBBLEPOP,
    CLAIRDELUNE,
    C5, D5, E5, F5,
    G5, A5, B5, C6,
    DOG_BARK,
    CAT_MEOW,
    COW_MOO,
    HORSE_NEIGH,
    ROOSTER_CROW,
    FART,
    CRASH,
    HIHAT,
    KICK,
    CLAP,
    SNARE,
    TOM
} SOUNDS;

typedef struct {
    uint16_t folder;
    uint16_t file;
} sound_id;

static const sound_id MP3_LIBRARY[] = {
    [BUBBLEPOP] = { .folder = 0x01, .file = 0x01 },
    [CLAIRDELUNE] = { .folder = 0x01, .file = 0x02 },
    [C5] = { .folder = 0x01, .file = 0x03 },
    [D5] = { .folder = 0x01, .file = 0x04 },
    [E5] = { .folder = 0x01, .file = 0x05 },
    [F5] = { .folder = 0x01, .file = 0x06 },
    [G5] = { .folder = 0x01, .file = 0x07 },
    [A5] = { .folder = 0x01, .file = 0x08 },
    [B5] = { .folder = 0x01, .file = 0x09 },
    [C6] = { .folder = 0x01, .file = 0x0A },
    [DOG_BARK] = { .folder = 0x01, .file = 0x0B },
    [CAT_MEOW] = { .folder = 0x01, .file = 0x0C },
    [COW_MOO] = { .folder = 0x01, .file = 0x0D },
    [HORSE_NEIGH] = { .folder = 0x01, .file = 0x0E },
    [ROOSTER_CROW] = { .folder = 0x01, .file = 0x0F },
    [FART] = { .folder = 0x01, .file = 0x10 },
    [CRASH] = { .folder = 0x01, .file = 0x11 },
    [HIHAT] = { .folder = 0x01, .file = 0x12 },
    [KICK] = { .folder = 0x01, .file = 0x13 },
    [CLAP] = { .folder = 0x01, .file = 0x14 },
    [SNARE] = { .folder = 0x01, .file = 0x15 },
    [TOM] = { .folder = 0x01, .file = 0x16 }
};

void mp3_initialize();
uint8_t* mp3_send_command(uint8_t command, uint16_t param);
void mp3_query_status();
void mp3_play_sound(SOUNDS sound);
void mp3_set_volume(uint8_t volume);
bool mp3_busy();

#endif // MP3_H