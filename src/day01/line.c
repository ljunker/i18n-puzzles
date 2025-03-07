#include <stdlib.h>

#include "utils.c"

typedef enum {
    NONE = 0,
    ONLY_TWEET = 7,
    ONLY_SMS = 11,
    BOTH = 13
} SendOption;

typedef struct {
    char *content;
    size_t byte_length;
    size_t char_length;
    SendOption send_option;
    int cost;
} Line;

typedef struct {
    Line *lines;
    int line_count;
    int capacity;
} FileLines;

void init_file_lines(FileLines *file_lines) {
    file_lines->lines = malloc(10 * sizeof(Line));
    if (!file_lines->lines) {
        perror("Error allocating memory for lines");
        exit(EXIT_FAILURE);
    }
    file_lines->line_count = 0;
    file_lines->capacity = 10;
}

void free_file_lines(FileLines *file_lines) {
    for (int i = 0; i < file_lines->line_count; i++) {
        free(file_lines->lines[i].content);
    }
    free(file_lines->lines);
}

static void determine_send_option(Line *line) {
    int can_send_sms = (line->byte_length <= 160);
    int can_send_tweet = (line->char_length <= 140);

    if (can_send_sms && can_send_tweet) {
        line->send_option = BOTH;
        line->cost = BOTH;
    } else if (can_send_sms) {
        line->send_option = ONLY_SMS;
        line->cost = ONLY_SMS;
    } else if (can_send_tweet) {
        line->send_option = ONLY_TWEET;
        line->cost = ONLY_TWEET;
    } else {
        line->send_option = NONE;
        line->cost = NONE;
    }
}

void add_line(FileLines *file_lines, char *buffer) {
    if (file_lines->line_count == file_lines->capacity) {
        file_lines->capacity *= 2;
        Line *temp = realloc(file_lines->lines, file_lines->capacity * sizeof(Line));
        if (!temp) {
            perror("Error reallocating memory for lines");
            exit(EXIT_FAILURE);
        }
        file_lines->lines = temp;
    }

    Line *new_line = &file_lines->lines[file_lines->line_count++];
    trim_newline(buffer);
    new_line->content = strdup(buffer);
    if (!new_line->content) {
        perror("Error allocating memory for line content");
        exit(EXIT_FAILURE);
    }

    new_line->byte_length = strlen(new_line->content);
    new_line->char_length = calc_char_length(new_line->content);
    determine_send_option(new_line);
}
