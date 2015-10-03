.global
alloc_ptr: .space 1


.global
.perm x
putc:
subi %sp, %sp, #8
ld %r0, %sp, #4
ld %r1, %sp, #0
/* Gen store. */
st %r0, %r1, #0

addi %sp, %sp, #8
jmpr %ra



.global
.perm x
alloc_init:
ldi %r0, alloc_ptr
/* Gen store. */
ldi %r1, #1024
shli %r1, %r1, #12
st %r1, %r0, #0

jmpr %ra



.global
.perm x
alloc:
subi %sp, %sp, #20
st %r4, %sp, #0
st %r5, %sp, #4

.phigen_alloc_0:
ld %r0, %sp, #16
/* spill store */
st %r0, %sp, #8


_alloc_1:
ld %r0, %sp, #8
andi %r4, %r0, #3

subi %r2, %r4, #0
iszero @p0, %r2
notp @p1, @p0
@p0 ? ldi %r5, #1
@p1 ? ldi %r5, #0

ld %r0, %sp, #8
addi %r0, %r0, #1
/* spill store */
st %r0, %sp, #12

rtop @p0, %r5
@p0 ? jmpi _alloc_5

.phigen_alloc_1:
ld %r0, %sp, #12
/* spill store */
st %r0, %sp, #8

jmpi _alloc_1

_alloc_5:
ldi %r0, alloc_ptr
ld %r4, %r0, #0

ori %r5, %r4, #0

ld %r0, %sp, #8
/* GEP, input 1, sz=4 */
/* type =  20 */
shli %r2, %r0, #2
add %r4, %r4, %r2

ldi %r0, alloc_ptr
/* Gen store. */
st %r4, %r0, #0

ori %r0, %r5, #0
ld %r4, %sp, #0
ld %r5, %sp, #4
addi %sp, %sp, #20
jmpr %ra



.global
.perm x
rng:
subi %sp, %sp, #8
st %r4, %sp, #0
ld %r0, %sp, #4
ld %r4, %r0, #0

ldi %r2, #5540
shli %r2, %r2, #12
ori %r2, %r2, #3637
mul %r4, %r4, %r2

addi %r4, %r4, #1

ld %r0, %sp, #4
/* Gen store. */
st %r4, %r0, #0

shri %r4, %r4, #16

ori %r0, %r4, #0
ld %r4, %sp, #0
addi %sp, %sp, #8
jmpr %ra



.global
.perm x
print_dec_internal:
subi %sp, %sp, #16
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
ld %r0, %sp, #12
modi %r4, %r0, #10

ori %r5, %r4, #48

ld %r0, %sp, #12
subi %r2, %r0, #9
isneg @p2, %r0
xori %r0, %r0, #9
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? orp @p0, @p2, @p2
notp @p1, @p1
@p1 ? isneg @p2, %r2
@p1 ? iszero @p0, %r2
@p1 ? orp @p0, @p0, @p2
@p1 ? notp @p0, @p0
@p0 ? jmpi _print_dec_internal_4

_print_dec_internal_6:
/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r5, %r2, #0

ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #16
jmpr %ra

_print_dec_internal_4:
ld %r0, %sp, #12
divi %r4, %r0, #10

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, print_dec_internal

jmpi _print_dec_internal_6



.global
.perm x
print_dec:
subi %sp, %sp, #8
st %ra, %sp, #0
ld %r1, %sp, #4
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, print_dec_internal

/* Gen store. */
ldi %r1, #10
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r1, %r2, #0

ld %ra, %sp, #0
addi %sp, %sp, #8
jmpr %ra



.global
.perm x
print_hex:
subi %sp, %sp, #12
st %r4, %sp, #0
st %r5, %sp, #4
ld %r0, %sp, #8
shri %r5, %r0, #28

ld %r0, %sp, #8
ldi %r2, #160
shli %r2, %r2, #12
shli %r2, %r2, #12
sub %r2, %r0, %r2
isneg @p2, %r0
ldi %r1, #160
shli %r1, %r1, #12
shli %r1, %r1, #12
xor %r0, %r0, %r1
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_3

_print_hex_5:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_7:
ld %r0, %sp, #8
shri %r4, %r0, #24

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_13

_print_hex_11:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_15:
ld %r0, %sp, #8
shri %r4, %r0, #20

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_21

_print_hex_19:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_23:
ld %r0, %sp, #8
shri %r4, %r0, #16

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_29

_print_hex_27:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_31:
ld %r0, %sp, #8
shri %r4, %r0, #12

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_37

_print_hex_35:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_39:
ld %r0, %sp, #8
shri %r4, %r0, #8

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_45

_print_hex_43:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_47:
ld %r0, %sp, #8
shri %r4, %r0, #4

andi %r5, %r4, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_53

_print_hex_51:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_55:
ld %r0, %sp, #8
andi %r5, %r0, #15

subi %r2, %r5, #10
isneg @p2, %r5
xori %r0, %r5, #10
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
@p0 ? jmpi _print_hex_60

_print_hex_58:
addi %r4, %r5, #87

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0


_print_hex_62:
/* Gen store. */
ldi %r1, #10
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r1, %r2, #0

ld %r4, %sp, #0
ld %r5, %sp, #4
addi %sp, %sp, #12
jmpr %ra

_print_hex_3:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_7

_print_hex_13:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_15

_print_hex_21:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_23

_print_hex_29:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_31

_print_hex_37:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_39

_print_hex_45:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_47

_print_hex_53:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_55

_print_hex_60:
ori %r4, %r5, #48

/* Gen store. */
ldi %r2, #128
shli %r2, %r2, #12
shli %r2, %r2, #12
st %r4, %r2, #0

jmpi _print_hex_62



