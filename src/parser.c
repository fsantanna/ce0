#include <assert.h>
#include <stdlib.h>

#include "all.h"

int _N_ = 0;

int err_expected (const char* v) {
    sprintf(ALL.err, "(ln %ld, col %ld): expected %s : have %s",
        ALL.tk1.lin, ALL.tk1.col, v, lexer_tk2str(&ALL.tk1));
    return 0;
}

int accept (TK enu) {
    if (ALL.tk1.enu == enu) {
        lexer();
        return 1;
    } else {
        return 0;
    }
}

int accept_err (TK enu) {
    int ret = accept(enu);
    if (ret == 0) {
        err_expected(lexer_tk2err(enu));
        //puts(ALL.err);
    }
    return ret;
}

int check (TK enu) {
    return (ALL.tk1.enu == enu);
}

///////////////////////////////////////////////////////////////////////////////

int parser_type (Type* ret) {
    int isalias = accept('&');

// TYPE_UNIT
    if (accept(TK_UNIT)) {
        *ret = (Type) { TYPE_UNIT, isalias };

// TYPE_PARENS / TYPE_TUPLE
    } else if (accept('(')) {
        if (!parser_type(ret)) {
            return 0;
        }

// TYPE_PARENS
        if (accept(')')) {
            return 1;
        }

// TYPE_TUPLE
        if (!accept_err(',')) {
            return 0;
        }

        int n = 1;
        Type* vec = malloc(n*sizeof(Type));
        assert(vec != NULL);
        vec[n-1] = *ret;

        do {
            n++;
            vec = realloc(vec, n*sizeof(Type));
            if (!parser_type(&vec[n-1])) {
                return 0;
            }
        } while (accept(','));

        if (!accept_err(')')) {
            return 0;
        }
        *ret = (Type) { TYPE_TUPLE, isalias, .Tuple={n,vec} };

// TYPE_NATIVE
    } else if (accept(TX_NATIVE)) {
        *ret = (Type) { TYPE_NATIVE, 0, .Native=ALL.tk0 };

// TYPE_USER
    } else if (accept('&') || accept(TX_USER)) {
        int isalias = (ALL.tk0.enu == '&');
        if (isalias && !accept_err(TX_USER)) {
            return 0;
        }
        *ret = (Type) { TYPE_USER, isalias, .User=ALL.tk0 };

    } else {
        return err_expected("type");
    }

// TYPE_FUNC
    if (accept(TK_ARROW)) {
        Type tp;
        if (!parser_type(&tp)) {
            return 0;
        }
        Type* inp = malloc(sizeof(Type));
        Type* out = malloc(sizeof(Type));
        assert(inp != NULL);
        assert(out != NULL);
        *inp = *ret;
        *out = tp;
        *ret = (Type) { TYPE_FUNC, 0, .Func={inp,out} };
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////

Stmt* enseq (Stmt* s1, Stmt* s2) {
    if (s1 == NULL) {
        return s2;
    } else if (s2 == NULL) {
        return s1;
    } else {
        Stmt* ret = malloc(sizeof(Stmt));
        assert(ret != NULL);
        *ret = (Stmt) { _N_++, STMT_SEQ,   NULL, {NULL,NULL}, ALL.tk0, .Seq={s1,s2} };
        return ret;
    }
}

Stmt* stmt_tmp (Tk tk0, Exp1* e, Exp1 init) {
    Tk tmp = tk0;
    tmp.enu = TX_VAR;
    sprintf(tmp.val.s, "_tmp_%d", _N_);

    Stmt* ret = malloc(sizeof(Stmt));
    assert(ret != NULL);
    *ret = (Stmt) {
        _N_++, STMT_VAR, NULL, {NULL,NULL}, tk0,
        .Var = { tmp, {TYPE_UNIT,0}, init }
    };

    *e = (Exp1) { _N_++, EXPR_VAR, 0, .tk=tmp };
    return ret;
}


///////////////////////////////////////////////////////////////////////////////

int parser_expr_ (Stmt** s, Exp1* e)
{
// EXPR_UNIT
    if (accept(TK_UNIT)) {
        *e = (Exp1) { _N_++, EXPR_UNIT, 0, .tk=ALL.tk0 };
        *s = NULL;

// EXPR_NULL
    } else if (accept(TX_NULL)) {   // $Nat
        *e = (Exp1) { _N_++, EXPR_NULL, 0, .tk=ALL.tk0 };
        *s = NULL;

// EXPR_NATIVE
    } else if (accept(TX_NATIVE)) {
        *s = NULL;
        *e = (Exp1) { _N_++, EXPR_NATIVE, 0, .tk=ALL.tk0 };

// EXPR_VAR
    } else if (accept(TX_VAR)) {
        *s = NULL;
        *e = (Exp1) { _N_++, EXPR_VAR, 0, .tk=ALL.tk0 };

// EXPR_PARENS / EXPR_TUPLE
    } else if (accept('(')) {
        Tk tk0 = ALL.tk0;

        if (!parser_expr(s,e)) {
            return 0;
        }

// EXPR_PARENS
        if (accept(')')) {
            return 1;
        }

// EXPR_TUPLE
        if (!accept_err(',')) {
            return 0;
        }

        int n = 1;

        Tk* vec = malloc(n*sizeof(Tk));
        assert(vec != NULL);
        vec[n-1] = e->tk;

        do {
            Exp1 e2;
            Stmt* s2;
            if (!parser_expr(&s2,&e2)) {
                return 0;
            }

            n++;
            vec = realloc(vec, n*sizeof(Tk));
            vec[n-1] = e2.tk;

            *s = enseq(*s, s2);
        } while (accept(','));

        if (!accept_err(')')) {
            return 0;
        }

        Exp1 tuple = (Exp1) { _N_++, EXPR_TUPLE, 0, .Tuple={n,vec} };
        *s = enseq(*s, stmt_tmp(tk0,e,tuple));

// EXPR_CONS
    } else if (accept(TX_USER)) {  // True
        Tk sub = ALL.tk0;

        Exp1 arg;
        if (!parser_expr(s,&arg)) {   // ()
            return 0;
        }

        Exp1 cons = (Exp1) { _N_++, EXPR_CONS, 0, .Cons={sub,arg.tk} };
        *s = enseq(*s, stmt_tmp(sub,e,cons));

    } else {
        return err_expected("expression");
    }
    return 1;
}

int parser_expr (Stmt** s, Exp1* e) {
    if (!parser_expr_(s, e)) {
        return 0;
    }

    while (1)
    {
// EXPR_CALL
        Tk tk0 = ALL.tk0;
        Stmt* s2;
        Exp1 arg;
        if (parser_expr_(&s2,&arg)) {
            *s = enseq(*s, s2);
            Exp1 call = { _N_++, EXPR_CALL, 0, .Call={e->tk,arg.tk} };
            *s = enseq(*s, stmt_tmp(tk0,e,call));

        } else if (accept('.')) {
// EXPR_INDEX
            if (accept(TX_NUM)) {
                Exp1 idx = { _N_++, EXPR_INDEX, 0, .Index={e->tk,ALL.tk0} };
                *s = enseq(*s, stmt_tmp(tk0,e,idx));
#if 0
    // EXPR_DISC / EXPR_PRED
            } else if (accept(TX_USER) || accept('$')) {
                if (ALL.tk0.enu == '$') {
                    if (!accept_err(TX_USER)) {
                        return 0;
                    }
                    ALL.tk0.enu = TX_NIL;   // TODO: move to lexer
                }
                Tk tk = ALL.tk0;

                Exp1* val = malloc(sizeof(Exp1));
                assert(val != NULL);
                *val = *ret;
                if (accept('?')) {
                    *ret = (Exp1) { _N_++, EXPR_PRED, 0, .Pred={val,tk} };
                } else if (accept('!')) {
                    *ret = (Exp1) { _N_++, EXPR_DISC, 0, .Disc={val,tk} };
                } else {
                    return err_expected("`?´ or `!´");
                }
#endif
            } else {
                return err_expected("index or subtype");
            }
        } else {
            break;
        }
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////

int parser_stmt_sub (Sub* ret) {
    if (!accept_err(TX_USER)) {
        return 0;
    }
    Tk id = ALL.tk0;                // True

    if (!accept_err(':')) {         // :
        return 0;
    }

    Type tp;
    if (!parser_type(&tp)) {        // ()
        return 0;
    }

    *ret = (Sub) { id, tp };
    return 1;
}

int parser_stmts (TK opt, Stmt** ret);

int parser_stmt (Stmt** ret) {
    int parser_block (Stmt** ret) {
        if (!accept('{')) {
            return 0;
        }
        Tk tk = ALL.tk0;

        Stmt* blk;
        if (!parser_stmts('}',&blk)) {
            return 0;
        }

        if (!accept_err('}')) { return 0; }

        Stmt* block = malloc(sizeof(Stmt));
        assert(block != NULL);
        *block = (Stmt) { _N_++, STMT_BLOCK, NULL, {NULL,NULL}, tk, .Block=blk };
        *ret = block;
        return 1;
    }

    // STMT_VAR
    if (accept(TK_VAR)) {
        Tk tk = ALL.tk0;
        if (!accept_err(TX_VAR)) {
            return 0;
        }
        Tk id = ALL.tk0;

        if (!accept_err(':')) {
            return 0;
        }
        Type tp;
        if (!parser_type(&tp)) {
            return 0;
        }
        if (!accept_err('=')) {
            return 0;
        }

        Exp1 e;
        Stmt* s;
        if (!parser_expr(&s,&e)) {
            return 0;
        }

        // reuse STMT_VAR from inner expression
        if (s!=NULL && s->sub==STMT_VAR) {
            s->Var.id = id;
            s->Var.type = tp;
            //s->Var.init = keep old;
            *ret = s;
        } else {
            assert(s == NULL);
            Stmt* var = malloc(sizeof(Stmt));
            assert(var != NULL);
            *var = (Stmt) { _N_++, STMT_VAR, NULL, {NULL,NULL}, tk, .Var={id,tp,e} };
            *ret = enseq(s, var);
        }

    // STMT_USER
    } else if (accept(TK_TYPE)) {       // type
        Tk tk = ALL.tk0;
        int isrec = 0;
        if (accept(TK_REC)) {           // rec
            isrec = 1;
        }
        if (!accept_err(TX_USER)) {
            return 0;
        }
        Tk id = ALL.tk0;                // Bool

        if (!accept_err('{')) {
            return 0;
        }

        Sub s;
        if (!parser_stmt_sub(&s)) {     // False ()
            return 0;
        }
        int n = 1;
        Sub* vec = malloc(n*sizeof(Sub));
        assert(vec != NULL);
        vec[n-1] = s;

        while (1) {
            accept(';');    // optional
            Sub q;
            if (!parser_stmt_sub(&q)) { // True ()
                break;
            }
            n++;
            vec = realloc(vec, n*sizeof(Sub));
            vec[n-1] = q;
        }

        if (!accept_err('}')) {
            return 0;
        }

        Stmt* user = malloc(sizeof(Stmt));
        assert(user != NULL);
        *user = (Stmt) { _N_++, STMT_USER, NULL, {NULL,NULL}, tk, .User={isrec,id,n,vec} };
        *ret = user;

    // STMT_CALL
    } else if (accept(TK_CALL)) {
        Stmt* s;
        Exp1 e;
        if (!parser_expr(&s,&e)) {
            return 0;
        }
        if (s == NULL) {
            ALL.tk1.enu = TK_ERR;  // workaround to fail enclosing '}'/EOF
            sprintf(ALL.err, "(ln %ld, col %ld): expected call expression : have %s",
                ALL.tk0.lin, ALL.tk0.col, lexer_tk2str(&ALL.tk0));
            return 0;
        }

        Stmt* tmp = s;
        while (tmp->sub == STMT_SEQ) {
            tmp = tmp->Seq.s2;
        }
        assert(tmp->sub == STMT_VAR);
        assert(tmp->Var.init.sub == EXPR_CALL);

        *ret = s;

    // STMT_IF
    } else if (accept(TK_IF)) {         // if
        Tk tk = ALL.tk0;

        Stmt* s;
        Exp1 e;
        if (!parser_expr(&s,&e)) {         // x
            return 0;
        }

        Stmt *t,*f;

        err_expected("{");
        if (!parser_block(&t)) {         // true()
            return 0;
        }

        if (accept(TK_ELSE)) {
            err_expected("{");
            if (!parser_block(&f)) {     // false()
                return 0;
            }
        } else {
            *f = (Stmt) { _N_++, STMT_BLOCK, NULL, {NULL,NULL}, ALL.tk0, .Block=NULL };
        }

        Stmt* If = malloc(sizeof(Stmt));
        assert(If != NULL);
        *If = (Stmt) { _N_++, STMT_IF, NULL, {NULL,NULL}, tk, .If={e.tk,t,f} };

        *ret = enseq(s, If);

    // STMT_FUNC
    } else if (accept(TK_FUNC)) {   // func
        Tk tk = ALL.tk0;
        if (!accept(TX_VAR)) {    // f
            return 0;
        }
        Tk id = ALL.tk0;
        if (!accept(':')) {         // :
            return 0;
        }
        Type tp;
        if (!parser_type(&tp)) {    // () -> ()
            return 0;
        }

        Stmt* blk;
        err_expected("{");
        if (!parser_block(&blk)) {
            return 0;
        }

        Stmt* func = malloc(sizeof(Stmt));
        assert(func != NULL);
        *func = (Stmt) { _N_++, STMT_FUNC, NULL, {NULL,NULL}, tk, .Func={id,tp,blk} };
        *ret = func;

    // STMT_RETURN
    } else if (accept(TK_RETURN)) {
        Tk tk = ALL.tk0;

        Stmt* s;
        Exp1 e;
        if (!parser_expr(&s,&e)) {
            return 0;
        }

        Stmt* Ret = malloc(sizeof(Stmt));
        assert(Ret != NULL);
        *Ret = (Stmt) { _N_++, STMT_RETURN, NULL, {NULL,NULL}, tk, .Return=e.tk };

        *ret = enseq(s, Ret);

    // STMT_BLOCK
    } else if (check('{')) {
        return parser_block(ret);

    } else {
        return err_expected("statement");
    }

    return 1;
}

int parser_stmts (TK opt, Stmt** ret) {
    *ret = NULL;
    while (1) {
        accept(';');    // optional
        Stmt* q;
        if (!parser_stmt(&q)) {
            if (check(opt)) {
                break;
            } else {
                return 0;
            }
        }
        *ret = enseq(*ret,q);
        accept(';');    // optional
    }
    return 1;
}

int parser (Stmt** ret) {
    static Type Type_Unit  = { TYPE_UNIT, 0 };

    *ret = NULL;

    // clone, output
    {
        static Type any   = { TYPE_NATIVE, 0, {.Native={TX_NATIVE,{.s="any"},0,0}} };
        static Type alias = { TYPE_NATIVE, 1, {.Native={TX_NATIVE,{.s="any"},0,0}} };

        Stmt* clone = malloc(sizeof(Stmt));
        assert(clone != NULL);
        *clone = (Stmt) {   // clone ()
            0, STMT_FUNC, NULL, {NULL,NULL},
            .Func = {
                { TX_VAR,{.s="clone"},0,0 },
                { TYPE_FUNC,.Func={&alias,&any} },
                NULL
            }
        };

        *ret = enseq(*ret, clone);

        Stmt* output = malloc(sizeof(Stmt));
        assert(output != NULL);
        *output = (Stmt) {   // output ()
            0, STMT_FUNC, NULL, {NULL,NULL},
            .Func = {
                { TX_VAR,{.s="output"},0,0 },
                { TYPE_FUNC,.Func={&alias,&Type_Unit} },
                NULL
            }
        };

        *ret = enseq(*ret, output);
    }

    Stmt* tmp;
    if (!parser_stmts(TK_EOF,&tmp)) {
        return 0;
    }

    *ret = enseq(*ret, tmp);

    if (!accept_err(TK_EOF)) {
        return 0;
    }
    return 1;
}
