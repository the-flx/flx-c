/**
 * $File: flx.c $
 * $Date: 2024-03-05 19:41:19 $
 * $Revision: $
 * $Creator: Jen-Chieh Shen $
 * $Notice: See LICENSE.txt for modification and distribution information
 *                   Copyright © 2024 by Shen, Jen-Chieh $
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"

#include "../include/flx.h"

static const char word_separators[] = {
        ' ', '-', '_', ':', '.', '/', '\\',
};

static const default_score = -35;

/**
 * Check if CHAR is a word character.
 */
static bool word(char ch) {
    if(!ch) {
        return false;
    }
    return !strchr(word_separators, ch);
}

/**
 * Check if CHAR is an uppercase character.
 */
static bool capital(char ch) { return word(ch) && isupper(ch); }

/**
 * Check if LAST-CHAR is the end of a word and CHAR the start of the next.
 *
 * This function is camel-case aware.
 */
static bool boundary(char last_ch, char ch) {
    if(!last_ch) {
        return true;
    }

    if(!capital(last_ch) && capital(ch)) {
        return true;
    }

    if(!word(last_ch) && word(ch)) {
        return true;
    }

    return false;
}

/**
 * Increment each element in VEC between BEG and END by INC.
 */
static void inc_vec() {

}

/**
 * Return best score matching QUERY against STR.
 * @param *str String to test.
 * @param *query Query use to score.
 * @return Return the score result.
 */
void flx_score(const char* str, const char* query, flx_result* result) {
    result->score   = 0;
    result->indices = 0;

    arrpush(result->indices, 1);

    printf("word %s\n", word(' ') ? "true" : "false");
}

void flx_free(flx_result* result) {
    if(!result->indices) {
        return;
    }
    arrfree(result->indices);
}
