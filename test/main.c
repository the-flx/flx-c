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

typedef struct {
    int  key;
    int* value;
} hm_int;

void try_hm() {
    hm_int* hm = NULL;
    int*    elm = NULL;
    int*    elm2 = NULL;

    arrput(elm, 1);
    arrput(elm, 2);

    int key1 = 0;
    hmput(hm, key1, elm);

    int key2 = 9;
    hmput(hm, key2, elm2);

    for (int i = 0; i < hmlen(hm); ++i) {
        printf("%d", hm[i].key);
    }

    arrfree(elm2);
    arrfree(elm);
    hmfree(hm);

}

int main(int argc, char* argv[]) {
    flx_result result;
    flx_score(&result, "buffer-file-name", "bfn");
    printf("Score %d\n", result.score);
    printf("Indicies %d\n", result.indices[0]);
    flx_free(&result);

    //try_hm();

    return 0;
}
