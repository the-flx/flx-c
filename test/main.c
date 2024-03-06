/**
 * $File: main.c $
 * $Date: 2024-03-05 19:47:17 $
 * $Revision: $
 * $Creator: Jen-Chieh Shen $
 * $Notice: See LICENSE.txt for modification and distribution information
 *                   Copyright © 2024 by Shen, Jen-Chieh $
 */

#include "../include/flx.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    flx_result result;
    flx_score("buffer-file-name", "bfn", &result);
    printf("Score %d\n", result.score);
    printf("Indicies %d\n", result.indices[0]);

    flx_free(&result);
    return 0;
}
