#ifndef PLAYLIST_H
#define PLAYLIST_H

typedef struct Song {
    char title[64];
    char artist[64];
    int duration_sec;
    struct Song *prev;
    struct Song *next;
} Song; 

typedef struct {
    Song *head;
    Song *tail;
    int count; 
} Playlist;

void playlist_init(Playlist *p1);
void playlist_free(Playlist *p1);

int playlist_append(
    Playlist *pi,
    const char *title,
    const char *artist,
    int duration_sec
);

int playlist_prepend(
    Playlist *p1,
    const char *title,
    const char *artist,
    int duration_sec
);

int playlist_insert_after(
    Playlist *p1,
    const char *after_title,
    const char *title, 
    const char *artist,
    int duration_sec
);

int playlist_remove(
    Playlist *p1,
    const char *title
);

int playlist_move_up(
    Playlist *p1,
    const char *title
);

void playlist_print(
    const Playlist *p1
);

void playlist_print_reverse(
    const Playlist *p1
);

int playlist_total_duration(
    const Playlist *p1
);

int playlist_load(
    Playlist *p1,
    const char *path
);

int playlist_save(
    const Playlist *p1,
    const char *path
);

#endif

