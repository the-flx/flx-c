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

#include "../include/stb_ds.h"

#include "../include/flx.h"

static const char word_separators[] = {
        ' ', '-', '_', ':', '.', '/', '\\',
};

static const default_score = -35;

/**
 * @struct Hashmap for value int.
 */
typedef struct {
    int  key;
    int* value;
} hm_int;

/**
 * @struct Hashmap for value flx_result.
 */
typedef struct {
    int         key;
    flx_result* value;
} hm_score;

/**
 * Clone the hm_int* hash map.
 */
static void clone_hm_int(hm_int* src, hm_int* dest) {
    if (!src) {
        return;
    }

    if (dest) {
        hmfree(dest);
    }
    dest = NULL;

    for (int i = 0; i < hmlen(src); ++i) {
        int key = src[i].key;
        hmput(dest, key, hmget(src, key));
    }
}

/**
 * Clone the int* array.
 */
static void clone_arr(int* src, int* dest) {
    if (!src) {
        return;
    }

    if (dest) {
        arrfree(dest);
    }
    dest = NULL;

    for (int i = 0; i < arrlen(src); ++i) {
        arrput(dest, src[i]);
    }
}

/**
 * Check if CHAR is a word character.
 */
static bool word(char ch) {
    if (!ch) {
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
    if (!last_ch) {
        return true;
    }

    if (!capital(last_ch) && capital(ch)) {
        return true;
    }

    if (!word(last_ch) && word(ch)) {
        return true;
    }

    return false;
}

/**
 * Increment each element in VEC between BEG and END by INC.
 */
static void inc_vec(int* vec, int inc, int beg, int end) {
    if (!vec) {
        return;
    }

    inc = (inc) ? inc : 1;
    beg = (beg) ? beg : 0;
    end = (end) ? end : arrlen(vec);

    while (beg < end) {
        vec[beg] = inc;
        ++beg;
    }
}

/**
 * Return hash-table for string where keys are characters.
 * Value is a sorted list of indexes for character occurrences.
 */
static void get_hash_for_string(hm_int* result, const char* str) {
    if (result) {
        hmfree(result);
    }
    result = NULL;

    const int str_len = strlen(str);
    int       index   = str_len - 1;

    char ch;
    char down_ch;

    while (0 <= index) {
        ch = str[index];

        if (capital(ch)) {
            hmput(result, ch, index);
            down_ch = tolower(ch);
        } else {
            down_ch = ch;
        }

        hmput(result, down_ch, index);

        --index;
    }
}

/**
 * Generate the heatmap vector of string.
 * 
 * See documentation for logic.
 */
static void get_heatmap_str(int* scores, const char* str, char group_separator) {
    if (!scores) {
        return;
    }

    const int str_len        = strlen(str);
    const int str_last_index = str_len - 1;

    if (scores) {
        arrfree(scores);
    }
    scores = NULL;

    for (int i = 0; i < str_len; ++i) {
        arrput(scores, default_score);
    }

    int penalty_lead = '.';

    int* inner = NULL;
    arrput(inner, -1);
    arrput(inner, 0);

    int** group_alist = NULL;
    arrput(group_alist, inner);

    // final char bonus
    scores[str_last_index] += 1;

    // Establish baseline mapping
    char last_ch;
    int  group_word_count = 0;
    int  index1           = 0;

    for (int i = 0; i < str_len; ++i) {
        char ch = str[i];

        // before we find any words, all separaters are
        // considered words of length 1.  This is so "foo/__ab"
        // gets penalized compared to "foo/ab".
        char effective_last_char = (group_word_count == 0) ? NULL : last_ch;

        if (boundary(effective_last_char, ch)) {
            arrins(group_alist[0], 2, index1);
        }

        if (!word(last_ch) && word(ch)) {
            ++group_word_count;
        }

        // ++++ -45 penalize extension
        if (last_ch && last_ch == penalty_lead) {
            scores[index1] += -45;
        }

        if (group_separator && group_separator == ch) {
            group_alist[0][1] = group_word_count;
            group_word_count  = 0;
            {
                int* new_arr = NULL;
                arrput(new_arr, index1);
                arrput(new_arr, group_word_count);
                arrins(group_alist, 0, new_arr);
            }
        }

        if (index1 == str_last_index) {
            group_alist[0][1] = group_word_count;
        } else {
            last_ch = ch;
        }

        ++index1;
    }

    int group_count     = arrlen(group_alist);
    int separator_count = group_count - 1;

    // ++++ slash group-count penalty
    if (separator_count != 0) {
        inc_vec(scores, group_count * -2, NULL, NULL);
    }

    int  index2           = separator_count;
    int  last_group_limit = NULL;
    bool basepath_found   = false;

    // score each group further
    for (int i = 0; i < arrlen(group_alist); ++i) {
        int* group = group_alist[i];

        int group_start = group[0];
        int word_count  = group[1];
        // this is the number of effective word groups
        int  words_len  = arrlen(group) - 2;
        bool basepath_p = false;

        if (words_len != 0 && !basepath_p) {
            basepath_found = true;
            basepath_p     = true;
        }

        int num;
        if (basepath_p) {
            // ++++ basepath separator-count boosts
            int boosts = 0;
            if (separator_count > 1) {
                boosts = separator_count - 1;
            }

            // ++++ basepath word count penalty
            int penalty = -word_count;
            num         = 35 + boosts + penalty;
        }
        // ++++ non-basepath penalties
        else {
            if (index2 == 0)
                num = -3;
            else
                num = -5 + (index2 - 1);
        }

        inc_vec(scores, num, group_start + 1, last_group_limit);

        int* cddr_group = NULL;
        clone_arr(group, cddr_group);
        arrdel(cddr_group, 0);
        arrdel(cddr_group, 0);

        int word_index = words_len - 1;
        int last_word  = (last_group_limit != NULL) ? last_group_limit : str_len;

        for (int i = 0; i < arrlen(cddr_group); ++i) {
            int word = cddr_group[i];

            // ++++  beg word bonus AND
            scores[word] += 85;

            int index3 = word;
            int charI  = 0;

            while (index3 < last_word) {
                scores[index3] += (-3 * word_index) - // ++++ word order penalty
                                  charI;              // ++++ char order penalty
                ++charI;

                ++index3;
            }

            last_word = word;
            --word_index;
        }

        last_group_limit = group_start + 1;
        --index2;

        arrfree(cddr_group);
    }
}

/**
 * Return sublist bigger than VAL from sorted SORTED-LIST.
 * 
 * If VAL is nil, return entire list.
 */
static void bigger_sublist(int* result, int* sorted_list, int val) {
    if (!sorted_list)
        return;

    if (val) {
        for (int i = 0; i < arrlen(sorted_list); ++i) {
            int sub = sorted_list[i];
            if (sub > val) {
                arrput(result, sub);
            }
        }
    } else {
        for (int i = 0; i < arrlen(sorted_list); ++i) {
            int sub = sorted_list[i];
            arrput(result, sub);
        }
    }
}

/**
 * Recursively compute the best match for a string, passed as STR-INFO and
 * HEATMAP, according to QUERY.
 */
static void find_best_match(flx_result* imatch, hm_int* str_info, int* heatmap, int greater_than,
                            const char* query, int query_len, int q_index, hm_score* match_cache) {
    int         greater_num = (greater_than) ? greater_than : 0;
    int         hash_key    = q_index + (greater_num * query_len);
    flx_result* hash_value  = hmget(match_cache, hash_key);

    // Process matchCache here
    if (hash_value != NULL) {
        if (imatch) {
            arrfree(imatch);
        }
        imatch = NULL;

        for (int i = 0; i < hash_value; ++i) {
            flx_result val = hash_value[i];
            arrput(imatch, val);
        }
    } else {
        int  uchar       = query[q_index];
        int* sorted_list = hmget(str_info, uchar);
        int* indexes     = NULL;
        bigger_sublist(indexes, sorted_list, greater_than);
        int best_score = INT_MIN;

        if (q_index >= query_len - 1) {
            // At the tail end of the recursion, simply generate all possible
            // matches with their scores and return the list to parent.
            for (int i = 0; i < arrlen(indexes); ++i) {
                int  index   = indexes[i];
                int* indices = NULL;
                arrput(indices, index);
                {
                    flx_result new_result;
                    new_result.indices = indices;
                    new_result.score   = heatmap[index];
                    new_result.tail    = 0;
                    arrput(imatch, new_result);
                }
            }
        } else {
            for (int i = 0; i < arrlen(indexes); ++i) {
                int index = indexes[i];

                flx_result* elem_group = NULL;
                hm_int*     new_dic    = NULL;
                clone_hm_int(new_dic, str_info);

                int* new_vec = NULL;
                clone_arr(new_vec, heatmap);

                find_best_match(elem_group, new_dic, new_vec, index, query, query_len, q_index + 1,
                                match_cache);

                hmfree(new_dic);
                arrfree(new_vec);
            }
        }
    }
}

/**
 * Return best score matching QUERY against STR.
 * @param *result Result pointer to store to.
 * @param *str String to test.
 * @param *query Query use to score.
 */
void flx_score(flx_result* result, const char* str, const char* query) {
    const int str_len   = strlen(str);
    const int query_len = strlen(query);

    if (str_len == 0 || query_len == 0) {
        return;
    }

    hm_int* str_info = NULL;
    get_hash_for_string(str_info, str);

    int* heatmap = NULL;
    get_heatmap_str(heatmap, str, NULL);

    bool        full_match_boost = (1 < query_len) && (query_len < 5);
    hm_score*   match_cache      = NULL;
    flx_result* optimal_match    = NULL;
    find_best_match(optimal_match, str_info, heatmap, NULL, query, query_len, 0, match_cache);

    if (arrlen(optimal_match) == 0) {
        return;
    }

    flx_result result1 = optimal_match[0];

    int caar = arrlen(result1.indices);

    if (full_match_boost && caar == str_len) {
        result1.score += 10000;
    }

    result->indices = result1.indices;
    result->score   = result1.score;
    result->tail    = result1.tail;

    hmfree(str_info);
    arrfree(heatmap);
    hmfree(match_cache);
    arrfree(optimal_match);
}

/**
 * Free result.
 * @param *result The score result to free.
 */
void flx_free(flx_result* result) {
    if (!result) {
        return;
    }

    if (!result->indices) {
        return;
    }
    arrfree(result->indices);
}
