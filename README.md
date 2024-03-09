[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/tag/jcs090218/flx-c.svg?label=release&logo=github)](https://github.com/jcs090218/flx-c/releases/latest)

# flx-c
> Rewrite emacs-flx in C

[![CI](https://github.com/jcs090218/flx-c/actions/workflows/test.yml/badge.svg)](https://github.com/jcs090218/flx-c/actions/workflows/test.yml)

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

## 🛠 Development

How to detect memory leaks: (macOS only)

```sh
# leaks -atExit -- ./path/to/exe/test_flx
```

On Linux, you should use [Valgrind](https://valgrind.org/) instead!

## 🔍 See Also

- [flx][] - Original algorithm in Emacs Lisp
- [flx-rs][] - Rewrite emacs-flx in Rust for dynamic modules
- [FlxCs][] - Rewrite emacs-flx in C#
- [flx-ts][] - Rewrite emacs-flx in TypeScript, with added support for JavaScript

## ⚜️ License

`flx-c` is distributed under the terms of the MIT license.

See [LICENSE](./LICENSE) for details.


<!-- Links -->

[flx]: https://github.com/lewang/flx
[flx-rs]: https://github.com/jcs090218/flx-rs
[FlxCs]: https://github.com/jcs090218/FlxCs
[flx-ts]: https://github.com/jcs090218/flx-ts
[flx-c]: https://github.com/jcs090218/flx-c
