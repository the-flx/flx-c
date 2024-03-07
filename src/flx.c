/**
 * $File: flx.c $
 * $Date: 2024-03-05 19:41:19 $
 * $Revision: $
 * $Creator: Jen-Chieh Shen $
 * $Notice: See LICENSE.txt for modification and distribution information
 *                   Copyright © 2024 by Shen, Jen-Chieh $
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../include/stb_ds.h"

#include "../include/flx.h"

#define min(X,Y) (((X) < (Y)) ? (X) : (Y))

#define NIL (char)INT_MIN

static const char word_separators[] = {
        ' ', '-', '_', ':', '.', '/', '\\',
};

static const int default_score = -35;

/**
 * Create a new result.
 */
static flx_result new_result(int* indicies, int score, int tail) {
    flx_result new_result;
    new_result.indices = indicies;
    new_result.score   = score;
    new_result.tail    = tail;
    return new_result;
}

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
 * Insert data to hm_int hash map.
 */
static void insert_dict(hm_int** dic, int key, int val) {
    int* new_arr = hmget(*dic, key);

    if (!new_arr) {
        new_arr = NULL;
    }

    arrins(new_arr, 0, val);

    hmput(*dic, key, new_arr);
}

/**
 * Clone the hm_int* hash map.
 */
static void clone_hm_int(const hm_int* src, hm_int** dest) {
    if (!src) {
        return;
    }

    if (*dest) {
        hmfree(*dest);
    }
    *dest = NULL;

    for (int i = 0; i < hmlen(src); ++i) {
        int key = src[i].key;
        int* val = src[i].value;
        hmput(*dest, key, val);
    }
}

/**
 * Clone the int* array.
 */
static void clone_arr_int(const int* src, int** dest) {
    if (!src) {
        return;
    }

    if (*dest) {
        arrfree(*dest);
    }
    *dest = NULL;

    for (int i = 0; i < arrlen(src); ++i) {
        arrput(*dest, src[i]);
    }
}

/**
 * Clone the flx_result* array.
 */
static void clone_arr_result(const flx_result* src, flx_result** dest) {
    if (*dest) {
        arrfree(*dest);
    }
    *dest = NULL;

    for (int i = 0; i < arrlen(src); ++i) {
        arrput(*dest, src[i]);
    }
}

/**
 * Check if CHAR is a word character.
 */
static bool word(char ch) {
    if (ch == NIL) {
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
static void inc_vec(int** vec, int inc, int beg, int end) {
    inc = (inc == NIL) ? 1 : inc;
    beg = (beg == NIL) ? 0 : beg;
    end = (end == NIL) ? arrlen(*vec) : end;

    while (beg < end) {
        (*vec)[beg] += inc;
        ++beg;
    }
}

/**
 * Return hash-table for string where keys are characters.
 * Value is a sorted list of indexes for character occurrences.
 */
static void get_hash_for_string(hm_int** result, const char* str) {
    if (*result) {
        hmfree(*result);
    }
    *result = NULL;

    const int str_len = strlen(str);
    int       index   = str_len - 1;

    char ch;
    char down_ch;

    while (0 <= index) {
        ch = str[index];

        if (capital(ch)) {
            insert_dict(result, ch, index);
            down_ch = tolower(ch);
        } else {
            down_ch = ch;
        }

        insert_dict(result, down_ch, index);

        --index;
    }
}

/**
 * Generate the heatmap vector of string.
 *
 * See documentation for logic.
 */
static void get_heatmap_str(int** scores, const char* str, char group_separator) {
    const int str_len        = strlen(str);
    const int str_last_index = str_len - 1;

    if (*scores) {
        arrfree(*scores);
    }
    *scores = NULL;

    for (int i = 0; i < str_len; ++i) {
        arrput(*scores, default_score);
    }

    int penalty_lead = '.';

    int* inner = NULL;
    arrput(inner, -1);
    arrput(inner, 0);

    int** group_alist = NULL;
    arrput(group_alist, inner);

    // final char bonus
    (*scores)[str_last_index] += 1;

    // Establish baseline mapping
    char last_ch          = NIL;
    int  group_word_count = 0;
    int  index1           = 0;

    for (int i = 0; i < str_len; ++i) {
        char ch = str[i];

        // before we find any words, all separaters are
        // considered words of length 1.  This is so "foo/__ab"
        // gets penalized compared to "foo/ab".
        char effective_last_char = (group_word_count == 0) ? 0 : last_ch;

        if (boundary(effective_last_char, ch)) {
            arrins(group_alist[0], 2, index1);
        }

        if (!word(last_ch) && word(ch)) {
            ++group_word_count;
        }

        // ++++ -45 penalize extension
        if (last_ch && last_ch == penalty_lead) {
            (*scores)[index1] += -45;
        }

        if (group_separator != NIL && group_separator == ch) {
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
        inc_vec(scores, group_count * -2, NIL, NIL);
    }

    int  index2           = separator_count;
    int  last_group_limit = NIL;
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
            if (index2 == 0) {
                num = -3;
            } else {
                num = -5 + (index2 - 1);
            }
        }

        int len = arrlen(*scores);

        inc_vec(scores, num, group_start + 1, last_group_limit);

        int* cddr_group = NULL;
        clone_arr_int(group, &cddr_group);
        arrdel(cddr_group, 0);
        arrdel(cddr_group, 0);

        int word_index = words_len - 1;
        int last_word  = (last_group_limit) ? last_group_limit : str_len;

        for (int i = 0; i < arrlen(cddr_group); ++i) {
            int word = cddr_group[i];

            // ++++  beg word bonus AND
            (*scores)[word] += 85;

            int index3 = word;
            int charI  = 0;

            while (index3 < last_word) {
                (*scores)[index3] += (-3 * word_index) - // ++++ word order penalty
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
static void bigger_sublist(int** result, int** sorted_list, int val) {
    if (*sorted_list == NULL) {
        return;
    }

    if (val) {
        for (int i = 0; i < arrlen(*sorted_list); ++i) {
            int sub = (*sorted_list)[i];
            if (sub > val) {
                arrput(*result, sub);
            }
        }
    } else {
        for (int i = 0; i < arrlen(*sorted_list); ++i) {
            int sub = (*sorted_list)[i];
            arrput(*result, sub);
        }
    }
}

/**
 * Recursively compute the best match for a string, passed as STR-INFO and
 * HEATMAP, according to QUERY.
 */
static void find_best_match(flx_result** imatch, hm_int** str_info, int** heatmap, int greater_than,
                            const char* query, int query_len, int q_index, hm_score** match_cache) {
    int         greater_num = (greater_than == NIL) ? 0 : greater_than;
    int         hash_key    = q_index + (greater_num * query_len);
    flx_result* hash_value  = hmget(*match_cache, hash_key);

    // Process matchCache here
    if (hash_value != NULL) {
        if (*imatch) {
            arrfree(*imatch);
        }
        *imatch = NULL;

        for (int i = 0; i < hmlen(hash_value); ++i) {
            flx_result val = hash_value[i];
            arrput(*imatch, val);
        }
    } else {
        int  uchar       = query[q_index];
        int* sorted_list = hmget(*str_info, uchar);
        int* indexes     = NULL;
        bigger_sublist(&indexes, &sorted_list, greater_than);
        int temp_score;
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
                    new_result.score   = (*heatmap)[index];
                    new_result.tail    = 0;
                    arrput(*imatch, new_result);
                }
            }
        } else {
            for (int i = 0; i < arrlen(indexes); ++i) {
                int index = indexes[i];

                flx_result* elem_group = NULL;

                hm_int*     new_dic    = NULL;
                clone_hm_int(*str_info, &new_dic);

                int* new_vec = NULL;
                clone_arr_int(*heatmap, &new_vec);

                find_best_match(&elem_group, &new_dic, &new_vec, index, query, query_len,
                                q_index + 1, match_cache);

                for (int j = 0; j < arrlen(elem_group); ++j) {
                    flx_result elem = elem_group[j];

                    int caar = elem.indices[0];
                    int cadr = elem.score;
                    int cddr = elem.tail;

                    if ((caar - 1) == index) {
                        temp_score = cadr + (*heatmap)[index] +
                                     (min(cddr, 3) * 15) + // boost contiguous matches
                                     60;
                    } else {
                        temp_score = cadr + (*heatmap)[index];
                    }

                    // We only care about the optimal match, so only forward the match
                    // with the best score to parent
                    if (temp_score > best_score) {
                        best_score = temp_score;

                        arrfree(*imatch);
                        *imatch = NULL;

                        int* indices = NULL;
                        clone_arr_int(elem.indices, &indices);

                        arrins(indices, 0, index);
                        int tail = 0;
                        if ((caar - 1) == index) {
                            tail = cddr + 1;
                        }

                        arrput(*imatch, new_result(indices, temp_score, tail));
                    }
                }

                hmfree(new_dic);
                arrfree(new_vec);
            }
        }

        // Calls are cached to avoid exponential time complexity
        flx_result* new_res = NULL;
        clone_arr_result(*imatch, &new_res);
        hmput(*match_cache, hash_key, new_res);
    }
}

/**
 * Return best score matching QUERY against STR.
 * @param *str String to test.
 * @param *query Query use to score.
 */
flx_result* flx_score(const char* str, const char* query) {
    flx_result* result = malloc(1 * sizeof(flx_result));

    const int str_len   = strlen(str);
    const int query_len = strlen(query);

    if (str_len == 0 || query_len == 0) {
        return NULL;
    }

    hm_int* str_info = NULL;
    get_hash_for_string(&str_info, str);

    int* heatmap = NULL;
    get_heatmap_str(&heatmap, str, NIL);

    bool        full_match_boost = (1 < query_len) && (query_len < 5);
    hm_score*   match_cache      = NULL;
    flx_result* optimal_match    = NULL;
    find_best_match(&optimal_match, &str_info, &heatmap, NIL, query, query_len, 0, &match_cache);

    if (arrlen(optimal_match) == 0) {
        return NULL;
    }

    flx_result result1 = optimal_match[0];

    int caar = arrlen(result1.indices);

    if (full_match_boost && caar == str_len) {
        result1.score += 10000;
    }

    /* Copy data */
    {
        result->indices = result1.indices;
        result->score   = result1.score;
        result->tail    = result1.tail;
    }

    /* Free memories. */
    {
        arrfree(heatmap);
        arrfree(optimal_match);

        for (int i = 0; i < hmlen(str_info); ++i) {
            arrfree(str_info[i].value);
        }
        hmfree(str_info);

        for (int i = 0; i < hmlen(match_cache); ++i) {
            arrfree(match_cache[i].value);
        }
        hmfree(match_cache);
    }

    return result;
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
    free(result);
}
