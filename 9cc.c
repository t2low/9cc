#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_EOF, // 終端トークン
};

// トークンの型
typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Token tokens[100];

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void tokenize(char *p) {
    int i = 0;
    while(*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    if (tokens[0].ty != TK_NUM) {
        error("最初の項が数ではありません");
    }
    printf("  mov rax, %d\n", tokens[0].val);

    int i = 1;
    while (tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            i++;
            if (tokens[i].ty != TK_NUM) {
                error("予期しないトークンです: %s", tokens[i].input);
            }
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '-') {
            i++;
            if (tokens[i].ty != TK_NUM) {
                error("予期しないトークンです: %s", tokens[i].input);
            }
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        error("予期しないトークンです: %s", tokens[i].input);
        return 1;
    }

    printf("  ret\n");
    return 0;
}
