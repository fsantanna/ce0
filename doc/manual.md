# Ce

A simple language with algebraic data types, ownership semantics, and scoped
memory management.

- [Install & Use](../README.md)
- Manual
    1. [Lexical rules](TODO)
    2. [Types](TODO)
    3. [Expressions](TODO)
    4. [Statements](TODO)
    5. [Syntax](TODO)
- [Memory management](memory.md)
- [Comparison with Rust](rust.md)

# 1. LEXICAL RULES

## Comments

A comment starts with a double hyphen `--` and runs until the end of the line:

```
-- this is a single line comment
```

## Keywords and Symbols

The following keywords are reserved:

```
    arg         -- function argument
    break       -- escape loop statement
    call        -- function invocation
    clone       -- clone function
    else        -- conditional statement
    func        -- function declaration
    if          -- conditional statement
    input       -- input invocation
    loop        -- loop statement
    native      -- native statement
    output      -- output invocation
    pre         -- native/type pre declaration
    return      -- function return
    set         -- assignment statement
    type        -- new type declaration
    var         -- variable declaration
    @ptr        -- recursive pointer type annotation
    @rec        -- recursive type annotation
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
    \           -- pointer type, upref & dnref operation
    $           -- empty subcase
    !           -- type discriminator
    ?           -- type predicate, unknown initialization
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

## Native tokens

A native token starts with an underscore `_` and might contain letters,
digits, and underscores:

```
_char    _printf    _errno      -- native identifiers
```

A native token may also be enclosed with curly braces `{` and `}` or
parenthesis `(` and `)`.
In this case, a native token can contain any other characters:

```
_(1 + 1)     _{2 * (1+1)}
```

# 2. TYPES

## Unit

The unit type `()` only has the [single value](TODO) `()`.

## Native

A native type holds external [values from the host language](TODO), i.e.,
values which *Ce* do not create or manipulate directly.

Native type identifiers follow the rules for [native tokens](TODO):

```
_char     _int    _{FILE*}
```

## User

A user type is a [new type](TODO) introduced by the programmer.
A user type holds values created from [subcase constructors](TODO) also
introduced by the programmer.

A user type identifier starts with an uppercase letter:

```
List    Int    Tree
```

The type `Int` is a primitive type that holds [integer values](TODO).

## Tuple

A tuple type holds [compound values](TODO) from a fixed number of other types.
A tuple type identifier is a comma-separated list of types enclosed with
parentheses:

```
((),(),())          -- type is a triple of unit types
(Int,(Tree,Tree))   -- type is a pair containing another pair
```

## Function

A function type holds a [function](TODO) value and is composed of an input and
output types separated by an arrow `->`:

```
() -> Tree          -- input is unit and output is Tree
(List,List) -> ()   -- input is a pair and output is unit
```

## Pointers

A pointer type can be applied to any other type with the prefix backslash `\`
and holds a pointer to another value:

```
\Int        -- pointer to Int
\List       -- pointer to List
```

# 3. EXPRESSIONS

## Unit

The unit value is the single value of the [unit type](TODO):

```
()
```

## Native expressions

A native expression holds a value of a [host language type](TODO):

```
_printf    _(2+2)     _{f(x,y)}
```

Symbols defined in *Ce* can also be accessed inside native expressions:

```
var x: Int = 10
output std _(x + 10)    -- outputs 20
```

## Variables

A variable holds a value of its [type](TODO):

```
i    myCounter    x_10
```

## Tuples and Indexes

A tuple holds a fixed number of values of a compound [tuple type](TODO):

```
((),False)              -- a pair with () and False
(x,(),y)                -- a triple
```

A tuple index suffixes a tuple with a dot `.` and evaluates to the value at the
given position:

```
(x,()).2                -- yields ()
```

## Calls, Input & Output

A call invokes a [function](TODO) with the given argument:

```
f ()                    -- f   receives unit     ()
call (id) x             -- id  receives variable x
add (x,y)               -- add receives tuple    (x,y)
```

The prefix keyword `call` can appear on assignments and statements, but not in
the middle of expressions.

The special function `clone` copies the contents of its argument.
If the value is of a recursive type, the copy is also recursive:

```
var y: List = Item Item $List
var x: List = clone y           -- `x` becomes "Item Item $List"
```

Just like a `call`, the `input` & `output` keywords also invoke functions, but
with the purpose of communicating with external I/O devices:

```
input libsdl Delay 2000            -- waits 2 seconds
output libsdl Draw Pixel (0,0)     -- draws a pixel on the screen
```

The supported I/O functions and associated behaviors dependend on the
platform in use.
The special device `std` works for the standard input & output devices and
accepts any value as argument:

```
var x: Int = input std      -- reads an `Int` from stdin (`TODO: not implemented`)
output std x                -- writes the value of `x` to stdout
```

The `input` & `output` expressions can appear on assignments and statements,
but not in the middle of expressions.

The declarations for the I/O functions must prefix their identifiers `input_`
or `output_`:

```
func output_libsdl: IO_Sdl -> () {
    ...
}
```

## Constructors, Discriminators & Predicates

### Constructors

A constructor creates a value of a [user type](TODO) given one subcase and its
argument:

```
True ()                 -- value of type `Bool`
False                   -- () is optional
Item (Int,List)         -- subcase `Item` holds a tuple
```

### Discriminators

A discriminator accesses the value of a [user type](TODO) as one of its
subcases.
A discriminator expression suffixes the value to access with a dot `.`, a
subcase identifier, and an exclamation mark `!`:

```
(True ()).True!         -- yields ()

x = Node ($Tree,(),$Tree)
x.Node!.2               -- yields ()
x.$True                 -- error: `x` is a `Node`
```

If the discriminated subcase does not match the actual value, the attempted
access raises a runtime error.

### Predicates

A predicate checks if the value of a [user type](TODO) is of its given subcase.
A predicate expression suffixes the value to test with a dot `.`, a subcase
identifier, and a question mark `?`:

```
type Member {
    Student:   ()
    Professor: ()
}
var x: Member = Professor
var b: Bool = x.Professor?    -- yields True
```

## Pointer uprefs & dnrefs

A pointer points to a variable holding a value.
An *upref* (up reference) acquires a pointer to a variable with the prefix
backslash `\`.
A *dnref* (down reference) recovers the value given a pointer with the sufix
backslash `\`:

```
var x: Int = 10
var y: \Int = \x    -- acquires a pointer to `x`
output std y\       -- recovers the value of `x`
```

# 4. STATEMENTS

## Type declarations

A type declaration creates a new [user type](TODO).
Each declaration inside the type defines a subcase of it:

```
type Bool {
    False: ()       -- subcase `False` holds unit value
    True:  ()       -- subcase `True`  holds unit value
}
```

A recursive type requires a `rec` modifier:

```
type rec Tree {
    -- $Tree: ()        -- implicit empty subcase, always present
    Node: (Tree,Tree)   -- subcase Node holds left/right subtrees
}
```

A recursive type always includes an implicit empty subcase, which is its name
with the dollar prefix `$´, e.g., `$Tree` is the empty subcase of `Tree`.

A mutually recursive type requires a `pre` declaration to signal its existence
before its full declaration:

```
type pre Bb     -- let `Aa` know that `Bb` exists
type rec Aa {
   Aa1: Bb      -- `Aa` contains `Bb`
}
type rec Bb {
   Bb1: Aa      -- `Bb` contains `Aa`
}
```

## Variable declarations

A variable declaration intoduces a name of the given type and assigns a value
to it:

```
var x : () = ()                  -- `x` of type `()` holds `()`
var y : Bool = True              -- `y` of type `Bool` holds `True`
var z : (Bool,()) = (False,())   -- `z` of tuple type holds tuple
var n : List = Cons(Cons($List)) -- `n` of type `List` holds result of constructor
```

## Assignments

An assignment changes the value of a variable, native identifier, tuple index,
or discriminator:

```
set x = ()
set _n = 1
set tup.1 = n
set x.Student! = ()
```

## Calls, Input & Output

The `call`, `input` & `output` statements invoke [functions](#TODO):

```
call f()        -- calls f passing ()
input std       -- input from stdin
output std 10   -- output to stdout
```

## Sequences

Statements execute one after the other and can be separated by semicolons:

```
call f() ; call g()     -- executes f, g, and h in order
call h()
```

## Conditionals

A conditional tests a `Bool` value and executes one of its true or false
branches depending on the test:

```
if x {
    call f()    -- calls f if x is True
} else {
    call g()    -- calls g if x is False
}
```

## Loops

A `loop` executes a block of statements indefinitely until it reaches a `break`
statement:

```
loop {
    ...         -- repeats this command indefinitely
    if ... {    -- until this condition is met
        break
    }
}
```

## Functions, Arguments and Returns

A function declaration binds a block of statements to a name which can be
[invoked](TODO) afterwards.
The declaration also determines the [function type](TODO) with the argument and
return types.
The argument can be accessed through the identifier `arg`.
A `return` exits a function with a value:

```
func f : () -> () {
    return arg
}
```

## Blocks

A block delimits, between curly braces `{` and `}`, the scope and visibility of
[variables](TODO):

```
{
    var x: () = ()
    ... x ...           -- `x` is visible here
}
... x ...               -- `x` is not visible here
```

## Native statements

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

# 5. SYNTAX

```
Stmt ::= `var´ VAR `:´ Type             -- variable declaration     var x: () = ()
            `=´ (Expr | `?´)
      |  `type´ [`rec´] USER `{`        -- user type declaration    type rec List {
            { USER `:´ Type [`;´] }     --    subcases                 Cons: List
         `}´                                                        }
      |  `type´ `pre´ USER              -- type pre declaration     type pre Expr
      |  `set´ Expr `=´ Expr            -- assignment               set x = 1
      |  (`call´ | `input´ |` output´)  -- call                     call f()
            (VAR|NATIVE) [Expr]         -- input & output           input std ; output std 10
      |  `if´ Expr `{´ Stmt `}´         -- conditional              if x { call f() } else { call g() }
         [`else´ `{´ Stmt `}´]
      |  `loop´ `{´ Stmt `}´            -- loop                     loop { break }
      |  `break´                        -- break                    break
      |  `func´ VAR `:´ Type `{´        -- function                 func f : ()->() { return () }
            Stmt
         `}´
      |  `return´ [Expr]                -- function return          return ()
      |  { Stmt [`;´] }                 -- sequence                 call f() ; call g()
      |  `{´ Stmt `}´                   -- block                    { call f() ; call g() }
      |  `native´ [`pre´] `{´ ... `}´   -- native                   native { printf("hi"); }

Expr ::= `(´ `)´                        -- unit value               ()
      |  NATIVE                         -- native expression        _printf
      |  `$´ USER                       -- empty constructor        $List
      |  VAR                            -- variable identifier      i
      |  `\´ Expr                       -- upref                    \x
      |  Expr `\´                       -- dnref                    x\
      |  `(´ Expr {`,´ Expr} `)´        -- tuple                    (x,())
      |  USER [Expr]                    -- constructor              True ()
      |  [`call´ | `input´ | `output´]  -- call                     f(x)
            (VAR|NATIVE) [Expr]         -- input & output           input std ; output std 10
      |  Expr `.´ NUM                   -- tuple index              x.1
      |  Expr `.´ [`$´] USER `!´        -- discriminator            x.True!
      |  Expr `.´ [`$´] USER `?´        -- predicate                x.False?
      |  `(´ Expr `)´                   -- group                    (x)

Type ::= `(´ `)´                        -- unit                     ()
      |  NATIVE                         -- native type              _char
      |  USER                           -- user type                Bool
      |  `(´ Type {`,´ Type} `)´        -- tuple                    ((),())
      |  Type `->´ Type                 -- function                 () -> ()
      |  `\` Type                       -- function                 \Int
```
