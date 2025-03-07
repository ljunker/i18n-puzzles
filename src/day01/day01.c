#include <stdio.h>
#include "line.c"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FileLines file_lines;
    init_file_lines(&file_lines);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        free_file_lines(&file_lines);
        return EXIT_FAILURE;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        add_line(&file_lines, buffer);
    }
    fclose(file);

    int total_cost = 0;
    printf("Contents of the file:\n");
    for (int i = 0; i < file_lines.line_count; i++) {
        Line *line = &file_lines.lines[i];
        printf("Line %d: \"%s\"\n", i + 1, line->content);
        printf("  Byte length: %zu, Character length: %zu\n", line->byte_length, line->char_length);
        printf("  Send option: %d (Cost: %d)\n", line->send_option, line->cost);
        total_cost += line->cost;
    }

    printf("\nTotal sending cost: %d\n", total_cost);
    free_file_lines(&file_lines);

    return EXIT_SUCCESS;
}