#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "../all.h"

enum {
    MODE_CODE,
    MODE_EXE
} MODE;

int main (int argc, char* argv[]) {
    char* inp = NULL;
    char* out = "ce.out";
    int mode  = MODE_EXE;
    char* cc  = "";

    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            out = argv[++i];
        } else if (!strcmp(argv[i], "-c")) {
            mode = MODE_CODE;
        } else if (!strcmp(argv[i], "-cc")) {
            cc = argv[++i];
        } else {
            inp = argv[i];
        }
    }
    assert(inp != NULL);
    assert(strlen(out) < 64);

    FILE* finp = fopen(inp, "r");
    FILE* fout; {
        if (mode == MODE_CODE) {
            fout = fopen(out, "w");
        } else {
            char gcc[256];
            sprintf(gcc, GCC " -o %s -xc - %s", out, cc);
            //puts(gcc);
            fout = popen(gcc, "w");
        }
    }
    assert(finp!=NULL && fout!=NULL);

    Stmt* s;

    if (!all_init(fout, finp)) {
        fprintf(stderr, "%s\n", ALL.err);
        exit(EXIT_FAILURE);
    }

    if (!parser(&s) || !sets(s) || !check_dcls(s) || !check_types(s) || !check_ptrs(s) || !check_txs(s) || !check_owner(s)) {
        fprintf(stderr, "%s\n", ALL.err);
        fputs("int main (void) {}", fout);
        exit(EXIT_FAILURE);
    }

    code(s);

    return 0;
}
