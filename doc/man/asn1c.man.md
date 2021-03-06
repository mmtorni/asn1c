% asn1c(1) ASN.1 Compiler
% Lev Walkin <vlm@lionet.info>
% 2016-01-23

# NAME

asn1c -- the ASN.1 Compiler

# SYNOPSIS

asn1c [**-E** [**-F**] | **-P** | **-R**] \
      [**-S***dir*] [**-X**] \
      [**-W***debug-*...] [**-f***option*] [**-gen-***option*] 
      [**-pdu**={**all**|**auto**|*Type*}] \
      [**-print-***option*] \
      *input-filenames*...

# DESCRIPTION

asn1c compiles ASN.1 specifications into a set of
target language (C/C++) encoders and decoders for BER, DER, PER, XER
and other encoding rules.

# OPTIONS

## Stage Selection Options

-E
:   Run the parsing stage only. Print the reconstructed ASN.1 text.

-F
:   Used together with **-E**,
    instructs the compiler to stop after the ASN.1 syntax
    tree fixing stage and dump the reconstructed ASN.1 specification
    to the standard output.

-P
:   Dump the compiled output to the standard output instead of creating the
    target language files on disk.

-R
:   Restrict the compiler to generate only the ASN.1 tables,
    omitting the usual support code.

-S *directory*
:   Use the specified directory with ASN.1 skeleton files.

-X
:   Generate an XML DTD schema for the specified ASN.1 files.

## Warning Options

-Werror
:   Treat warnings as errors; abort if any warning is produced.

-Wdebug-lexer
:   Enable lexer debugging during the ASN.1 parsing stage.

-Wdebug-fixer
:   Enable ASN.1 syntax tree fixer debugging during the fixing stage.

-Wdebug-compiler
:   Enable debugging during the actual compile time.

## Language Options

-fbless-SIZE
:   Allow `SIZE()` constraint for `INTEGER`, `ENUMERATED`,
    and other types for which this constraint is normally prohibited
    by the standard.
    This is a violation of ASN.1 standard, and the compiler may
    fail to produce a meaningful code.

-fcompound-names
:   Using this option prevents name collisions in the target source code
    by using complex names for target language structures.
    (Name collisions may occur if the ASN.1 module reuses the same identifiers
    in multiple contexts).

-findirect-choice
:   When generating code for a `CHOICE` type, compile the `CHOICE` members
    as indirect pointers instead of declaring them inline.
    Consider using this option together with **-fno-include-deps**
    to prevent circular references.

-fincludes-quoted
:   Refer to header files in `#include`s using **"**double**"** instead of **\<**angle**>** quotes.

-fknown-extern-type=*name*
:   Pretend the specified type is known.
    The compiler will assume the target language source files
    for the given type have been provided manually.

-fline-refs
:   Include ASN.1 module's line numbers in generated code comments.

-fno-constraints
:   Do not generate ASN.1 subtype constraint checking code.
    This may make a shorter executable.

-fno-include-deps
:   Do not generate courtesy #include lines for non-critical type dependencies.
    Helps prevent namespace collisions.

-funnamed-unions
:   Enable unnamed unions in the definitions of target language's structures.

-fwide-types
:   Use the unbounded size data types (`INTEGER_t`, `ENUMERATED_t`, `REAL_t`)
    by default, instead of using the native machine's data types (long, double).

-flink-skeletons
:   Symlink skeletons support files rather than copy them.

-fno-skeletons-copy
:   Do not copy skeletons support files.

## Codecs Generation Options

-gen-PER
:   Generate the Packed Encoding Rules (PER) support code.

-pdu={all|auto|*Type*}
:   Create a PDU table for specified types, or discover Protocol Data Units
    automatically. In case of **-pdu=all**,
    all ASN.1 types defined in all modules will form a PDU table.
    In case of **-pdu=auto**, all types not referenced by any other type will
    form a PDU table.
    If *Type* is an ASN.1 type identifier, the identifier is added to
    the generated PDU table.
    The last form may be specified multiple times to add any number of PDUs.

## Output Options

-print-constraints
:   When **-EF** options are also specified,
    this option forces the compiler to explain its internal understanding
    of subtype constraints.

-print-lines
:   Generate "`-- #line`" comments in **-E** output.

# SEE ALSO

`unber`(1), `enber`(1).

