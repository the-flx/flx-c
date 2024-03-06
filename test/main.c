/**
 * $File: main.c $
 * $Date: 2024-03-05 19:47:17 $
 * $Revision: $
 * $Creator: Jen-Chieh Shen $
 * $Notice: See LICENSE.txt for modification and distribution information
 *                   Copyright © 2024 by Shen, Jen-Chieh $
 */

#include <stdio.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"

#include "../include/flx.h"

void try_arr() {
    int* arr = 0;
    arrpush(arr, 4);
    arr[0] += 10;
    arrfree(arr);
}

int main(int argc, char* argv[]) {
    flx_result result;
    flx_score(&result, "buffer-file-name", "bfn");
    printf("Score %d\n", result.score);
    printf("Indicies %d\n", result.indices[0]);

    flx_free(&result);

    return 0;
}
