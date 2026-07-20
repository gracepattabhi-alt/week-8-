#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

static void print_total(int sec) {
    int hours = sec / 3600;
    int mins  = (sec % 3600) / 60;
    int secs  = sec % 60;
    if (hours > 0)
        printf("Total: %d:%02d:%02d\n", hours, mins, secs);
    else
        printf("Total: %d:%02d\n", mins, secs);
}

static int parse_song_arg(char       *buf,
                           char       title[64],
                           char       artist[64],
                           int        *dur) {
    char *p1 = strchr(buf, '|');
    if (p1 == NULL) return -1;
    *p1 = '\0';

    char *p2 = strchr(p1 + 1, '|');
    if (p2 == NULL) return -1;
    *p2 = '\0';

    strncpy(title,  buf,    63); title[63]  = '\0';
    strncpy(artist, p1 + 1, 63); artist[63] = '\0';
    *dur = atoi(p2 + 1);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];

    Playlist pl;
    playlist_init(&pl);
    playlist_load(&pl, path);

    char line[512];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;   
        size_t len = strlen(line);
        while (len > 0 &&
               (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;   

           
        char cmd[8]   = {0};
        char arg[504] = {0};

        char *sp = strchr(line, ' ');
        
        if (sp != NULL) {
            size_t clen = (size_t)(sp - line);
            if (clen >= sizeof(cmd)) clen = sizeof(cmd) - 1;
            strncpy(cmd, line, clen);
            cmd[clen] = '\0';
            strncpy(arg, sp + 1, sizeof(arg) - 1);
        } else {
            strncpy(cmd, line, sizeof(cmd) - 1);
        }

       
        if (strcmp(cmd, "p") == 0) {
            playlist_print(&pl);

        
        } else if (strcmp(cmd, "r") == 0) {
            playlist_print_reverse(&pl);

        
        } else if (strcmp(cmd, "t") == 0) {
            print_total(playlist_total_duration(&pl));

        
        } else if (strcmp(cmd, "a") == 0) {
            char title[64], artist[64]; int dur;
            if (parse_song_arg(arg, title, artist, &dur) == 0)
                playlist_append(&pl, title, artist, dur);
            else
                printf("Usage: a title|artist|sec\n");

        
        } else if (strcmp(cmd, "pre") == 0) {
            char title[64], artist[64]; int dur;
            if (parse_song_arg(arg, title, artist, &dur) == 0)
                playlist_prepend(&pl, title, artist, dur);
            else
                printf("Usage: pre title|artist|sec\n");

        } else if (strcmp(cmd, "ins") == 0) {
            
            char *p1 = strchr(arg, '|');
            if (p1 == NULL) {
                printf("Usage: ins after_title|title|artist|sec\n");
                continue;
            }
            *p1 = '\0';   

            char after[64];
            strncpy(after, arg, 63); after[63] = '\0';

            char title[64], artist[64]; int dur;
            if (parse_song_arg(p1 + 1, title, artist, &dur) == 0) {
                if (playlist_insert_after(&pl, after,
                                           title, artist, dur) == -1)
                    printf("Not found: \"%s\"\n", after);
            } else {
                printf("Usage: ins after_title|title|artist|sec\n");
            }

        
        } else if (strcmp(cmd, "del") == 0) {
            if (playlist_remove(&pl, arg) == -1)
                printf("Not found: \"%s\"\n", arg);

        
        } else if (strcmp(cmd, "up") == 0) {
            if (playlist_move_up(&pl, arg) == -1)
                printf("Not found: \"%s\"\n", arg);

        
        } else if (strcmp(cmd, "q") == 0) {
            break;

        
        } else {
            printf("Unknown command. Type 'p' to print.\n");
        }
    }

   
    if (playlist_save(&pl, path) != 0)
        fprintf(stderr, "Error: could not save playlist to '%s'.\n", path);


    playlist_free(&pl);
    return 0;
}