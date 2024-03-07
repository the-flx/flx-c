[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

# flx-c
> Rewrite emacs-flx in C

## 🔧 Usage

```c
flx_result* result = flx_score("buffer-file-name", "bfn");

if (result == NULL) {
    return -1;
}

printf("Score: %d\n", result->score);

for (int i = 0; i < arrlen(result->indices); ++i) {
    printf("idicies: %d %d\n", i, (*result).indices[i]);
}

flx_free(result);
```

## ⚜️ License

`flx-c` is distributed under the terms of the MIT license.

See [LICENSE](./LICENSE) for details.
