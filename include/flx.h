#ifndef __FLX_H__
/**
 * $File: flx.h $
 * $Date: 2024-03-05 19:42:16 $
 * $Revision: $
 * $Creator: Jen-Chieh Shen $
 * $Notice: See LICENSE.txt for modification and distribution information
 *                   Copyright © 2024 by Shen, Jen-Chieh $
 */
#define __FLX_H__

/**
 * @struct Score result.
 */
typedef struct {
    int  score;    /* The score (string distance) */
    int* indices;  /* Indicies occurrence */
    int  tail;
} flx_result;

/**
 * Return best score matching QUERY against STR.
 * @param *str String to test.
 * @param *query Query use to score.
 */
flx_result* flx_score(const char* str, const char* query);

/**
 * Free result.
 * @param *result The score result to free.
 */
void flx_free(flx_result* result);

#endif /* __FLX_H__ */
