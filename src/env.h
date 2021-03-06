int expr_isprefix (Expr* e1, Expr* e2);

#define __ENV_EXPR_TO_TYPE_FREE__ __attribute__ ((__cleanup__(env_expr_to_type_free)))
Type* env_expr_to_type      (Env* env, Expr* e);
void  env_expr_to_type_free (Type** tp);

Stmt* env_id_to_stmt    (Env* env, const char* id);
Type* env_tk_to_type    (Env* env, Tk* tk);
Stmt* env_stmt_to_func  (Stmt* s);
int   env_type_ishasrec (Env* env, Type* tp);
int   env_type_ishasptr (Env* env, Type* tp);
Stmt* env_sub_id_to_user_stmt (Env* env, const char* sub);
Type* env_sub_id_to_user_type (Env* env, const char* sub);
Stmt* env_tk_to_type_to_user_stmt (Env* env, Tk* tk);
Stmt* env_expr_leftmost_decl (Env* env, Expr* e);

void  env_held_vars (Env* env, Expr* e, int* vars_n, Expr** vars, int* uprefs);
Expr* env_held_vars_nonself (Env* env, const char* dst, Expr* src);

typedef void (*F_txed_vars) (Expr* e_, Expr* e);
void txs_txed_vars (Env* env, Expr* e, F_txed_vars f);
