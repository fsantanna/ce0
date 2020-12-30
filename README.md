# Ce

A simple language with algebraic data types with ownership semantics and scoped
memory management similar to Rust (i.e. no garbage collection).

- Jump straight to [memory management](TODO).

# 1. Lexical rules

## Comment

A comment starts with `--` and runs until the end of the line:

```
-- this is a single line comment
```

## Keywords and Symbols

The following keywords are reserved:

```
    arg         -- function argument
    call        -- function invocation
    clone       -- clone function
    else        -- conditional statement
    func        -- function declaration
    if          -- conditional statement
    native      -- native statement
    pre         -- native pre declaration
    rec         -- type recursive declaration
    return      -- function return
    set         -- assignment statement
    show        -- output function
    type        -- new type declaration
    var         -- variable declaration
```

The following symbols are valid:

```
    {   }       -- block delimeter
    (   )       -- unit type, unit value, group expression
    ;           -- sequence separator
    :           -- variable, type, function declaration
    ->          -- function type signature
    =           -- variable assignment
    ,           -- tuple separator
    .           -- tuple index, type predicate & discriminator
    &           -- alias prefix
    $           -- null subtype
    !           -- type discriminator
    ?           -- type predicate
```

## Identifiers

A variable identifier starts with a lowercase letter and might contain letters,
digits, and underscores:

```
i    myCounter    x_10          -- variable identifiers
```

A type identifier starts with an uppercase letter and might contain letters,
digits, and underscores:

```
Int    U32    Tree              -- type identifiers
```

## Integer numbers

A number is a sequence of digits:

```
1    20    300                  -- tuple indexes / Int values
```

Numbers are used in values of [type `Int`](TODO) and in [tuple indexes](TODO):

## Native token

A native token starts with an underscore `_` and might contain letters,
digits, and underscores:

```
_char    _printf    _errno      -- native identifiers
```

A native token may also be enclosed by curly braces `{` and `}` or parenthesis
`(` and `)` to contain any other characters:

```
_(1 + 1)     _{2 * (1+1)}
```

# 2. Types

## Unit

The unit type `()` only allows the [single value](TODO) `()`.

## Native

A native type holds external [values from the host language](TODO), i.e.,
values which *Ce* do not create or manipulate directly.

Native type identifiers follow the rules for [native tokens](TODO):

```
_char     _int    _{FILE*}
```

## User

A user type is a [new type](TODO) introduced by the programmer.
A user type holds values created from [subtype constructors](TODO) also
introduced by the programmer.

A user type identifier starts with an uppercase letter and might be prefixed
with an ampersand `&` if an alias:

```
List    Int     Tree
```

The type `Int` is a primitive type that holds [integer values](#TODO).

## Tuple

A tuple type holds [compound values](TODO) from a fixed number of other types:
A tuple type identifier is a comma-separated list of types, enclosed in
parentheses:

```
((),())     (Int,(Tree,Tree))
```

## Function

A function type holds a [function](TODO) value and is composed of an input and
output types separated by an arrow `->`:

```
() -> Tree
(List,List) -> ()
```

## Alias

An alias type can be applied to any other type with the prefix ampersand `&`:

```
&Int    &List
```

# 3. Expressions

## Unit

The unit value is the single value of the [unit type](TODO):

```
()
```

## Native expression

A native expression holds a value from a [host language type](TODO):

```
_printf    _(2+2)     _{f(x,y)}
```

## Variable

A variable holds a value of its [type](TODO):

```
i    myCounter    x_10
```

## Tuple and Index

A tuple holds a fixed number of values of a compound [tuple type](TODO):

```
((),False)              -- a pair with () and False
(x,(),y)                -- a triple
```

A tuple index suffix a tuple with a dot `.` and holds the value at the given
position:

```
(x,()).2                -- yields ()
```

## Call

A call invokes an expression as a [function](TODO) with the given argument:

```
f ()                    -- f   receives unit     ()
(id) x                  -- id  receives variable x
add (x,y)               -- add receives tuple    (x,y)
```

The special function `clone` copies the contents of its argument.
If the value is of a recursive type, the copy is also recursive:

```
var y: List = Item Item $List
var x: List = clone y           -- `x` becomes "Item Item $List"
```

The special function `show` outputs the given argument to the screen:

```
var x: ((),()) = ((),())
show x          -- outputs "((),())" to the screen
```

## Constructor, Discriminator, Predicate

### Constructor

A constructor creates a value of a [user type](TODO) given one subtype and its
argument:

```
True ()                 -- value of type Bool
False                   -- () is optional
Car (True,())           -- subtype Car holds a tuple
```

### Discriminator

A discriminator accesses the value of a [user type](TODO) as one of its
subtypes.
It suffixes the value with a dot `.`, a subtype identifier, and an exclamation
mark `!`:

```
(True ()).True!         -- yields ()

x = Node ($Tree,(),$Tree)
x.Node!.2               -- yields ()
x.$True                 -- error: `x` is a `Node`
```

An error occurs during execution if the discriminated subtype does not match
the actual value.

### Predicate

A predicate checks if the value of a [user type](TODO) is of its given subtype.
It suffixes the value with a dot `.`, a subtype identifier, and a question mark
`?`:

```
type Member {
    Student:   ()
    Professor: ()
}
x = Professor
b = x.Professor?    -- yields True
```

## Alias

An alias is a [reference](TODO) to another value acquired with the prefix
ampersand `&`:

```
var y: &List = &x    -- alias to `x`
```

# 4. Statements

## Type declaration

A type declaration creates a new [user type](TODO).
Each case in the type declaration defines a subtype of it:

```
type Bool {
    False: ()       -- subtype False holds unit value
    True:  ()       -- subtype True  holds unit value
}
```

A recursive type uses a `rec` modifier and always contains the implicit base
null subtype with the prefix `$´:

```
type rec Tree {
    -- $Tree: ()            -- implicit null subtype always present
    Node: (Tree,(),Tree)    -- subtype Node holds left subtree, unit value, and right subtree
}
```

The prefix `$` yields the null subtype of all recursive types, e.g., `$Tree` is
the null subtype of `Tree`.
The null subtype can be used in a
[constructor, discriminator, or predicate](#TODO).

## Variable declaration

A variable declaration intoduces a name of a given type and assigns a value to
it:

```
var x : () = ()                  -- `x` of type `()` holds `()`
var y : Bool = True              -- `y` of type `Bool` holds `True`
var z : (Bool,()) = (False,())   -- `z` of given tuple type holds the given tuple
var n : List = Cons(Cons($List)) -- `n` of recursive type `List` holds result of constructor
```

## Assignment

An assignment changes the value of a variable, native identifier, tuple index,
or discriminator:

```
set x = ()
set _n = 1
set tup.1 = n
set x.Student! = ()
```

## Call

A call statement invokes a call expression:

```
call f()    -- calls f passing ()
```

## Sequence

Statements execute one after the other and can be separated by semicolons:

```
call f() ; call g()
call h()
```

## Conditional

A conditional tests a `Bool` value and executes one of its true or false
branches depending on the test:

```
if x {
    call f()    -- if x is True
} else {
    call g()    -- if x is False
}
```

## Function, Argument and Return

A function declaration binds a block of statements to a name which can be
[called](TODO) afterwards.
The declaration also determines the [function type](TODO) with the argument and
return types.
The argument can be accessed through the identifier `arg`.
A `return` exits a function with a value:

```
func f : () -> () {
    return arg
}
```

## Block

A block delimits, between curly braces `{` and `}`, the scope and visibility of
[variables](TODO):

```
{
    var x: () = ()
    ... x ...           -- `x` is visible here
}
... x ...               -- `x` is not visible here
```

## Native

A native statement executes a [native token](TODO) in the host language:

```
native _{
    printf("Hello World!");
}
```

The modifier `pre` makes the native block to be included before the main
program:

```
native pre _{
    #include <math.h>
}
```

# 5. Syntax

```
Stmt ::= `var´ VAR `:´ [`&´] Type       -- variable declaration     var x: () = ()
            `=´ Expr
      |  `type´ [`rec´] USER `{`        -- user type declaration    type rec List {
            { USER `:´ Type [`;´] }     --    subtypes                 Cons: List
         `}´                                                        }
      |  `set´ Expr `=´ Expr            -- assignment               set x = 1
      |  `call´ Expr                    -- call                     call f()
      |  `if´ Expr `{´ Stmt `}´         -- conditional              if x { call f() } else { call g() }
         [`else´ `{´ Stmt `}´]
      |  `func´ VAR `:´ Type `{´        -- function                 func f : ()->() { return () }
            Stmt
         `}´
      |  `return´ Expr                  -- function return          return ()
      |  { Stmt [`;´] }                 -- sequence                 call f() ; call g()
      |  `{´ Stmt `}´                   -- block                    { call f() ; call g() }
      |  `native´ [`pre´] `{´ ... `}´   -- native                   native { printf("hi"); }

Expr ::= `(´ `)´                        -- unit value               ()
      |  NATIVE                         -- native expression        _printf
      |  `$´ USER                       -- null constructor         $List
      |  VAR                            -- variable identifier      i
      |  `&´ Expr                       -- alias                    &x
      |  `(´ Expr {`,´ Expr} `)´        -- tuple                    (x,())
      |  USER [Expr]                    -- constructor              True ()
      |  Expr Expr                      -- call                     f(x)
      |  Expr `.´ NUM                   -- tuple index              x.1
      |  Expr `.´ [`$´] USER `!´        -- discriminator            x.True!
      |  Expr `.´ [`$´] USER `?´        -- predicate                x.False?
      |  `(´ Expr `)´                   -- group                    (x)

Type ::= `(´ `)´                        -- unit                     ()
      |  NATIVE                         -- native type              _char
      |  USER                           -- user type                Bool
      |  `(´ Type {`,´ Type} `)´        -- tuple                    ((),())
      |  Type `->´ Type                 -- function                 () -> ()
```

<!--
# A. Pools and recursive types

## Goals

Recursive types, such as lists and trees, require dynamic memory allocation
since their sizes are unbounded.

Pools are containers for mutually referred values of recursive types.
A pool is associated with the root variable of a recursive type.
When the root goes out of scope, all pool memory is automatically reclaimed.
A pool can be optionally declared with a bounded size, allowing it to be
allocated in the stack.
Pools enable to the following properties for recursive types:

- bounded memory allocation
- deterministic deallocation
- no garbage collection
-->

# A. Dynamic memory management

Values of recursive types, such as lists and trees, require dynamic memory
allocation since their sizes are unbounded.
They also grow and shrink during runtime since they typically represent complex
data structures that evolve over time.
Finally, they are manipulated by different parts of the program, even outside
the scope in which they were originally instantiated.
These three characteristics, (a) dynamic allocation, (b) variable size and (c)
scope portability, need to be addressed somehow.

*Ce* approaches recursive types with algebraic data types with ownership
semantics and scoped memory management.
Allocation is bound to the scope of the assignee, which is the owner of the
value.
Deallocation occurs automatically when the scope of the owner terminates.
Ownership can be transferred by reassigning the value to another assignee,
which can live in another scope.
A value can also be shared with an alias without transferring ownership.

*Ce* ensures that deallocation occurs exactly once at the very moment when there
are no more active references to the value.
In particular the following cases must be prevented:

- Memory leak: when a value cannot be referenced but is not deallocated and remains in memory.
- Dangling reference: when a value is deallocated but can still be referenced (aka. *use-after-free*).
- Double free: when a value is deallocated multiple times.

TODO: limitations (trees, aliasing rules)

## Basics

In *Ce*, a new type declaration supports variants (subtypes) with tuples:

```
type Character {
    Warrior: (Int,Int)      -- variant Warrior has strength and stamina
    Ranger:  (Int,Int)      -- variant Ranger has sight and speed
    Wizard:  Int            -- variant Wizard has mana power
}
```

Such composite types are also known as algebraic data types because they are
composed of sums (variants) and products (tuples).

A new type declaration can also be made recursive if it uses itself in one of
its subtypes:

```
type rec List {         -- a list is either empty ($List) or
    Item: (Int,List)    -- an item that holds a number and a sublist
}

var l: List = Item (1, Item (2,$List))   -- list `1 -> 2 -> null`
```

A variable of a recursive type holds a reference to its value, since
constructors are always dynamically allocated in the heap:

```
var x: List = Item(1, $List)
    |              __|__
   / \            /     \
  | x |--------> |   1   | <-- actual allocated memory with the linked list
   \_/   ref     |  null |
    |             \_____/
    |                |
  stack             heap
```

The assigned variable is the owner of the allocated value, which is
automatically deallocated when the enclosing scope terminates:

```
{
    var x: List = Item(1, $List)
}
-- scope terminates, memory pointed by `x` is deallocated
```

A variable can be aliased or *borrowed* with the prefix ampersand `&`.
In this case, both the owner and the alias refer to the same allocated value:

```
var x: List  = Item(1, $List)
var y: &List = &x    |
    |              __|__
   / \            /     \
  | x |--------> |   1   | <-- actual allocated memory with the linked list
   \_/       /   |  null |
    |       /     \_____/
    |      /         |
  stack   /         heap
    |    /
   / \  /
  | y |-
   \_/
```

## Ownership and Borrowing

The ownership and borrowing of dynamically allocated values must follow a set
of rules:

1. Every allocated constructor has a single owner at any given time. Not zero, not two or more.
    - The owner is a variable that lives in the stack and reaches the allocated value.
2. When the owner goes out of scope, the allocated memory is automatically
   deallocated.
3. An alias cannot escape the scope of its owner.
4. Ownership can be transferred in three ways:
    - Assigning the owner to another variable, which becomes the new owner (e.g. `new = old`).
    - Passing the owner to a function call argument, which becomes the new owner (e.g. `f(old)`).
    - Returning the owner from a function call to an assignee, which becomes the new owner (e.g. `new = f()`).
5. Ownership cannot be transferred with an active alias in scope.
6. The original owner is invalidated after transferring its ownership.

All rules are verified at compile time, i.e., there are no runtime checks or
extra overheads.

### Ownership transfer

As stated in rule 6, an ownership transfer invalidates the original owner and
rejects further accesses to it:

```
{
    var x: List = Item(1, $List)    -- `x` is the original owner
    var y: List = x                 -- `y` is the new owner
    ... x ...                       -- error: `x` cannot be referred again
    ... y ...                       -- ok
}
```

Ownership transfer ensures that rule 1 is preserved.
If ownership were shared among multiple references, deallocation in rule 2
would be ambiguous or cause a double free, since owners could be in different
scopes:

```
{
    var x: List = Item(1, $List)    -- `x` is the original owner
    {
        var y: List = x             -- `y` is the new owner
    }                               -- deallocate here?
}                                   -- or here or both?
```

Ownership transfer is particularly important when the value must survive the
allocation scope, which is typical of constructor functions:

```
func build: () -> List {
    var tmp: List = ...     -- `tmp` is the original owner
    return tmp              -- `return` transfers ownership (we don't want to deallocate it now)
}
var l: List = build()       -- `l` is the new owner
```

It is possible to transfer only part of a recursive value.
In this case, the removed part will be automatically reset to the null subtype:

```
var x: List = Item(1, Item(2, $List))   -- after: Item(1,$List)
var y: List = x.Item!                   -- after: Item(2,$List)
```

Finally, it is also possible to loose ownership without transferring to anyone.
In this case, the value without owner will be deallocated immediately:

```
var x: List = Item(1, $List)    -- previous value
set x = $List                   -- previous value is deallocated
```

## Borrowing

In many situations, transferring ownership is undesirable, such as when passing
a value to a narrower scope for temporary manipulation:

```
var l: List = build()   -- `l` is the owner
... length(&l) ...      -- `l` is borrowed on call and unborrowed on return
... l ...               -- `l` is still the owner

func length: (&List -> Int) {
    ... -- use alias, which is destroyed on termination
}
```

Rule 3 states that an alias cannot escape the scope of its owner:

```
func f: () -> &List {
    var l: List = build()   -- `l` is the owner
    return &l               -- error: cannot return alias to deallocated value
}
```

If escaping an alias were allowed, it would refer to a value that would be
deallocated from memory, resulting in a dangling reference.

TODO: rule 5

<!--

All dependencies of an assignment are tracked and all constructors are
allocated in the same pool.
When the pool itself goes out of scope, all allocated memory inside it is
traversed and is automatically reclaimed.

The pool may be declared with bounded size (e.g. `y[64]`), which limits the
number of nodes in the tree.
This allows the pool items to be allocated in the stack (instead of `malloc`).
When the pool goes out of scope, the stack unwinds and all memory is
automatically reclaimed.

Internally, the pool is forwarded to all constructors locations where the
actual allocations takes place:

```
void f (Pool* pool) {
    Nat* _2 = pool_alloc(pool, sizeof(Nat));    // constructors allocate
    Nat* _1 = pool_alloc(pool, sizeof(Nat));    // in the forwarded pool
    *_2 = (Nat) { Succ, {._Succ=NULL} };
    *_1 = (Nat) { Succ, {._Succ=_2} };
    x = _1;
    return x;
}

Nat  _yv[64];               // stack-allocated buffer (if bounded)
Pool _yp = { _yv, 64, 0 };  // buffer, max size, cur size
Nat* y = f(&_yp);           // pool is NULL if unbounded
```

!--
If the pool is unbouded (e.g. `y[]`), all allocation is made in the heap with
`malloc`.
Then, when the root reference (e.g. `y`) goes out of scope, it is traversed to
`free` all memory.
--

## Details

### Pool allocation

A bounded pool is defined internally as follows:

```
typedef struct {
    void* buf;      // stack-allocated buffer
    int   max;      // maximum size
    int   cur;      // current size
} Pool;
```

Pool allocation depends if the pool is bounded or unbounded:

```
void* pool_alloc (Pool* pool, int n) {
    if (pool == NULL) {                     // pool is unbounded
        return malloc(n);                   // fall back to `malloc`
    } else {
        void* ret = &pool->buf[pool->cur];
        pool->cur += n;                     // nodes are allocated sequentially
        if (pool->cur <= cur->max) {
            return ret;
        } else {
            return NULL;
        }
    }
}
```

-->

A dynamic constructor must check if all allocations succeeded.

Illustrative example:

```
func f: () -> Nat {
    var x: Nat = Succ(Succ($Nat))
    return x
}
var y[]: Nat = f()    -- y[] or y[N]
```

Generated code:

```
void f (Pool* pool) {
    int _cur = pool->cur;                       // current pool size
    Nat* _2 = pool_alloc(pool, sizeof(Nat));
    Nat* _1 = pool_alloc(pool, sizeof(Nat));
    if (_2==NULL || _1==NULL) {                 // one of them failed
        if (pool == NULL) {
            free(_1);                           // free both
            free(_2);
        } else {
            pool->cur = _cur;                   // restore pool size
        }
    } else {
        *_2 = (Nat) { Succ, {._Succ=NULL} };    // assign both
        *_1 = (Nat) { Succ, {._Succ=_2} };
        x = _1;                                 // root value
    }
}
```

### Pool deallocation

- stack
- __cleanup__

### Tracking assignments

1. Check the root assignment for dependencies in nested scopes:

```
var y: Nat = Succ(Succ($Nat))   -- same scope: static allocation
```

```
var y[]: Nat = f()              -- body of `f` is nested: pool allocation
```

2. Check `return` of body for dependencies:

```
return x                        -- check `x`
```

```
var x: Nat = Succ(Succ($Nat))   -- constructor must be allocated in the received pool
```

### TODO

```
-- OK
call show(Succ($Nat))     -- ok stack

-- ERR
-- `f` returns `Nat` but have no pool to allocated it
-- if call returns isrec, it must be in an assignment or in a return (to use pool from outside)
func f: () -> Nat {}
call f()                    -- missing pool for return of "f"

-- OK
var three: Nat = ...
func fthree: () -> Nat {
    return three            -- should not use pool b/c defined outside
}
```
