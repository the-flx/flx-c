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
    int key;
    int value;
} mypair;

void try_hm() {
    mypair* map = NULL;
    int  k = 0;
    hmput(map, k, 1);
    printf("val: %d\n", hmget(map, k));
}

void modify_arr(int** arr) {
    *arr = NULL;
    arrput(*arr, 11);
    arrput(*arr, 22);

    //*arr = (int*)malloc(2 * sizeof(int));
    //(*arr)[0] = 10;
    //(*arr)[1] = 20;

    //printf("? %s\n", arrlen(*arr));
}

void test_flx() {
    flx_result result;
    flx_score(&result, "buffer-file-name", "bfn");
    printf("Score %d\n", result.score);
    printf("Indicies %d\n", result.indices[0]);
    flx_free(&result);
}

int main(int argc, char* argv[]) {
    test_flx();

    {
        //int* arr = NULL;

        //modify_arr(&arr);

        //printf("? %d\n", arrlen(arr));
        //arrfree(arr);
    }

    //try_hm();

    return 0;
}
