#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

void playlist_init(Playlist *p1) {
    p1->head = NULL;
    p1->tail = NULL;
    p1->count = 0;
}

void playlist_free(Playlist *p1) {
    Song *cur = p1->head;
    Song *nxt;
    while (cur != NULL) {
        nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    p1->head = NULL; 
    p1->tail = NULL;
    p1->count = 0;
}

static Song *song_create(const char *title,
const char *artist, 
int duration_sec) {
    Song *n = malloc(sizeof(Song));
    if (n == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    strncpy(n->title, title, sizeof(n->title) - 1);
    strncpy(n->artist, artist, sizeof(n->artist) - 1);
    n->title[sizeof(n->title) - 1] = '\0';
    n->artist[sizeof(n->artist) - 1] = '\0';
    n->duration_sec = duration_sec;
    n->prev = NULL;
    n->next = NULL;
    return n;
}

int playlist_append(Playlist *p1,
const char *title,
const char *artist,
int duration_sec) {
    Song *n = song_create(title, artist, duration_sec);
    if (n == NULL) return -1; 

    n->prev = p1->tail;
    n->next = NULL;

if (p1->tail != NULL)
    p1->tail->next = n;
else p1->head = n;
p1->tail = n; 
p1->count++;
return 0;
}

int playlist_prepend(Playlist *p1,
const char *title,
const char *artist,
int duration_sec) {
    Song *n = song_create(title, artist, duration_sec);
    if (n == NULL) return -1;

    n->prev = NULL;
    n->next = p1->head;

    if (p1->head != NULL)
        p1->head->prev = n;
    else
        p1->tail = n;
    p1->head = n;   
    p1->count++;
    return 0;
}

int playlist_insert_after(Playlist *p1,
                const char *after_title,
                const char *title,
                const char *artist,
                int duration_sec) {

    Song *cur = p1->head;
    while (cur != NULL) {
        if (strcmp(cur->title, after_title) == 0) {
            Song *n = song_create(title, artist, duration_sec);
            if (n == NULL) return -1;

            n->prev = cur;
            n->next = cur->next;

            if (cur->next != NULL)
                cur->next->prev = n;
            else
                p1->tail = n;
            cur->next = n;
            p1->count++;
            return 0;
        }
        cur = cur->next;
    }
    return -1;
}

    int playlist_remove(Playlist *p1, const char *title) {
        Song *cur = p1->head;
        while (cur != NULL) {
            if(strcmp(cur->title, title) == 0) {
                if (cur->prev != NULL)
                    cur->prev->next = cur->next;
                else
                    p1->head = cur->next;
                if (cur->next != NULL)
                    cur->next->prev = cur->prev;
                else
                    p1->tail = cur->prev;
                free(cur);
                p1->count--;
                return 0;
            }
            cur = cur->next;
        }
        return -1;
    }

    int playlist_move_up(Playlist *p1, const char *title) {
        Song *cur = p1->head;
        while (cur != NULL) {
            if(strcmp(cur->title, title) == 0) {
                if (cur->prev == NULL)
                    return 0;
            Song *prv = cur->prev;

            char tmp_title[64];
            memcpy(tmp_title, prv->title, sizeof(tmp_title));
            memcpy(prv->title, cur->title, sizeof(prv->title));
            memcpy(cur->title, tmp_title, sizeof(cur->title));

            char tmp_artist[64];
            memcpy(tmp_artist, prv->artist, sizeof(tmp_artist));
            memcpy(prv->artist, cur->artist, sizeof(prv->artist));
            memcpy(cur->artist, tmp_artist, sizeof(cur->artist));

            int tmp_dur = prv->duration_sec;
            prv->duration_sec = cur->duration_sec;
            cur->duration_sec = tmp_dur;

            return 0;
            }
            cur = cur->next;
         }
         return -1;
    }
    
static void fmt_song_dur(int sec, char *buf, int buflen) {
    int mins = sec / 60;
    int secs = sec % 60;
    snprintf(buf, buflen, "%d:%02d", mins, secs);
}

void playlist_print(const Playlist *pl) {
    printf("= Playlist (%d songs) =\n", pl->count);
    Song *cur = pl->head;
    int   num = 1;
    while (cur != NULL) {
        char dur[16];
        fmt_song_dur(cur->duration_sec, dur, sizeof(dur));
        printf("  %d. %s - %s [%s]\n",
               num++, cur->title, cur->artist, dur);
        cur = cur->next;
    }
}

void playlist_print_reverse(const Playlist *p1) {
    printf("= Playlist (%d songs) = \n", p1->count);
    Song *cur = p1->tail;
    int num = 1;
    while (cur != NULL) {
        char dur[16];
        fmt_song_dur(cur->duration_sec, dur, sizeof(dur));
        printf("  %d. %s - %s [%s]\n",
            num++, cur->title, cur->artist, dur);
            cur = cur->prev;
    }
}

int playlist_total_duration(const Playlist *pl) {
    int   total = 0;
    Song *cur   = pl->head;
    while (cur != NULL) {
        total += cur->duration_sec;
        cur    = cur->next;
    }
    return total;
}

int playlist_load(Playlist *pl, const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr,
                "Could not open '%s' for reading; "
                "starting with empty playlist.\n", path);
        return -1;
    }
 char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 &&
               (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len == 0) continue;  

        char *p1 = strchr(line, '|');
        if (p1 == NULL) continue;
        *p1 = '\0'; 
        char *p2 = strchr(p1 + 1, '|');
        if (p2 == NULL) continue;
        *p2 = '\0';

        char title[64], artist[64];
        strncpy(title,  line,   sizeof(title)  - 1);
        strncpy(artist, p1 + 1, sizeof(artist) - 1);
        title[sizeof(title)   - 1] = '\0';
        artist[sizeof(artist) - 1] = '\0';
        int dur = atoi(p2 + 1);

        playlist_append(pl, title, artist, dur);
    }

    fclose(f);
    return 0;
}

int playlist_save(const Playlist *pl, const char *path) {
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "Could not open '%s' for writing.\n", path);
        return -1;
    }

    Song *cur = pl->head;
    while (cur != NULL) {
        fprintf(f, "%s|%s|%d\n",
                cur->title, cur->artist, cur->duration_sec);
        cur = cur->next;
    }

    fclose(f);
    return 0;
}
   
