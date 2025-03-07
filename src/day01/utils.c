#include <stdbool.h>
#include <string.h>
#include <stdint.h>

// Unicode constants for handling ZWJ, regional indicators, skin-tone modifiers, etc.
#define ZWJ 0x200D     // Zero Width Joiner
#define FE0E 0xFE0E    // Text Variation Selector
#define FE0F 0xFE0F    // Emoji Variation Selector

// Regional Indicator Unicode range
#define REGIONAL_INDICATOR_START 0x1F1E6
#define REGIONAL_INDICATOR_END   0x1F1FF

// Skin-tone Modifier Unicode range
#define SKIN_TONE_MODIFIER_START 0x1F3FB
#define SKIN_TONE_MODIFIER_END   0x1F3FF

// Helper macros
#define IS_IN_UNICODE_RANGE(cp, start, end) ((cp) >= (start) && (cp) <= (end))

// Check if a code point is a regional indicator
static bool is_regional_indicator(uint32_t codepoint) {
    return IS_IN_UNICODE_RANGE(codepoint, REGIONAL_INDICATOR_START, REGIONAL_INDICATOR_END);
}

// Check if a code point is a skin tone modifier
static bool is_skin_tone_modifier(uint32_t codepoint) {
    return IS_IN_UNICODE_RANGE(codepoint, SKIN_TONE_MODIFIER_START, SKIN_TONE_MODIFIER_END);
}

// Check if a code point is a combining character
static bool is_combining_character(uint32_t codepoint) {
    return (codepoint >= 0x0300 && codepoint <= 0x036F) ||  // Combining Diacritical Marks
           (codepoint >= 0x1AB0 && codepoint <= 0x1AFF) ||  // Combining Diacritical Marks Extended
           (codepoint >= 0x1DC0 && codepoint <= 0x1DFF) ||  // Combining Diacritical Marks Supplement
           (codepoint >= 0x20D0 && codepoint <= 0x20FF) ||  // Combining Diacritical Marks for Symbols
           (codepoint >= 0xFE20 && codepoint <= 0xFE2F);    // Combining Half Marks
}

// Check if a code point is an emoji variation selector
static bool is_variation_selector(uint32_t codepoint) {
    return codepoint == FE0E || codepoint == FE0F;
}

// Decode a single Unicode code point from a UTF-8 string
// Advances the input pointer to the next character byte
static uint32_t decode_utf8(const char **str) {
    const uint8_t *s = (const uint8_t *)(*str);
    uint32_t codepoint = 0;

    if ((*s & 0x80) == 0x00) {
        // 1-byte (ASCII)
        codepoint = *s;
        (*str)++;
    } else if ((*s & 0xE0) == 0xC0) {
        // 2-byte
        codepoint = (*s & 0x1F) << 6;
        s++;
        codepoint |= (*s & 0x3F);
        (*str) += 2;
    } else if ((*s & 0xF0) == 0xE0) {
        // 3-byte
        codepoint = (*s & 0x0F) << 12;
        s++;
        codepoint |= (*s & 0x3F) << 6;
        s++;
        codepoint |= (*s & 0x3F);
        (*str) += 3;
    } else if ((*s & 0xF8) == 0xF0) {
        // 4-byte
        codepoint = (*s & 0x07) << 18;
        s++;
        codepoint |= (*s & 0x3F) << 12;
        s++;
        codepoint |= (*s & 0x3F) << 6;
        s++;
        codepoint |= (*s & 0x3F);
        (*str) += 4;
    } else {
        // Invalid byte, move to the next byte
        (*str)++;
    }

    return codepoint;
}

// Function to calculate grapheme cluster length
size_t calc_char_length(const char *str) {
    size_t char_length = 0;
    uint32_t prev_codepoint = 0; // To remember the last code point
    uint32_t codepoint = 0;

    while (*str) {
        codepoint = decode_utf8(&str);

        // Handle cases where the code point is part of the current grapheme cluster
        if (prev_codepoint) {
            // ZWJ-Based Sequence
            if (prev_codepoint == ZWJ) {
                prev_codepoint = codepoint; // Continue the cluster
                continue;
            }

            // Regional Indicator Pair
            if (is_regional_indicator(prev_codepoint) && is_regional_indicator(codepoint)) {
                prev_codepoint = 0; // Start fresh for future clusters
                continue;
            }
        }

        // Skin-Tone Modifiers
        if (is_skin_tone_modifier(codepoint)) {
            prev_codepoint = codepoint; // Extend the current cluster
            continue;
        }

        // Emoji Variation Selector
        if (is_variation_selector(codepoint)) {
            prev_codepoint = codepoint; // Extend the current cluster
            continue;
        }

        // Combining Characters
        if (is_combining_character(codepoint)) {
            prev_codepoint = codepoint; // Extend the current cluster
            continue;
        }

        // New grapheme cluster begins
        char_length++;
        prev_codepoint = codepoint;
    }
    return char_length;
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}