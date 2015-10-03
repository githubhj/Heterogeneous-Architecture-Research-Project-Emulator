.global
.perm x
show_array:
subi %sp, %sp, #60
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r4, %sp, #40

/* alloca */
addi %r0, %sp, #44
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #48
/* spill store */
st %r0, %sp, #16

ld %r0, %sp, #56
/* Gen store. */
st %r0, %r4, #0

ld %r0, %sp, #52
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r4, %r4, #0

ld %r0, %sp, #16
/* Gen store. */
st %r4, %r0, #0


_show_array_4:
ld %r0, %sp, #16
ld %r4, %r0, #0

ld %r0, %sp, #12
ld %r5, %r0, #0

sub %r2, %r4, %r5
rtop @p0, %r2
@p0 ? jmpi _show_array_8

_show_array_14:
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #60
jmpr %ra

_show_array_8:
ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #24

ld %r0, %sp, #24
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #28

ld %r1, %sp, #28
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, print_dec


_show_array_11:
ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #32

ld %r0, %sp, #32
/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r0, #4
/* spill store */
st %r0, %sp, #36

ld %r0, %sp, #36
ld %r1, %sp, #16
/* Gen store. */
st %r0, %r1, #0

jmpi _show_array_4



.global
.perm x
swap:
subi %sp, %sp, #40
st %r4, %sp, #0
st %r5, %sp, #4
/* alloca */
addi %r0, %sp, #20
/* spill store */
st %r0, %sp, #8

/* alloca */
addi %r0, %sp, #24
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #28
/* spill store */
st %r0, %sp, #16

ld %r0, %sp, #36
ld %r1, %sp, #8
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #32
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #8
ld %r4, %r0, #0

ld %r4, %r4, #0

ld %r0, %sp, #16
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

ld %r5, %r4, #0

ld %r0, %sp, #8
ld %r4, %r0, #0

/* Gen store. */
st %r5, %r4, #0

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

/* Gen store. */
st %r5, %r4, #0

ld %r4, %sp, #0
ld %r5, %sp, #4
addi %sp, %sp, #40
jmpr %ra



.global
.perm x
get_pivot:
subi %sp, %sp, #52
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r0, %sp, #28
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r5, %sp, #32

/* alloca */
addi %r4, %sp, #36

ld %r0, %sp, #48
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #44
/* Gen store. */
st %r0, %r5, #0

ld %r0, %sp, #40
/* Gen store. */
st %r0, %r4, #0

ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #16

ld %r4, %r4, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, rng
/* spill store */
st %r0, %sp, #20

ld %r4, %r5, #0

ld %r0, %sp, #12
ld %r5, %r0, #0

ori %r0, %r4, #0
/* spill store */
st %r0, %sp, #24

ori %r4, %r5, #0

ld %r0, %sp, #24
sub %r4, %r0, %r4

divi %r4, %r4, #4

ld %r0, %sp, #20
mod %r4, %r0, %r4

ld %r0, %sp, #16
/* GEP, input 1, sz=4 */
/* type =  20 */
shli %r2, %r4, #2
add %r4, %r0, %r2

ori %r0, %r4, #0
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #52
jmpr %ra



.global
.perm x
sort:
subi %sp, %sp, #148
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r0, %sp, #104
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #108
/* spill store */
st %r0, %sp, #16

/* alloca */
addi %r0, %sp, #112
/* spill store */
st %r0, %sp, #20

/* alloca */
addi %r0, %sp, #116
/* spill store */
st %r0, %sp, #24

/* alloca */
addi %r0, %sp, #120
/* spill store */
st %r0, %sp, #28

/* alloca */
addi %r0, %sp, #124
/* spill store */
st %r0, %sp, #32

/* alloca */
addi %r0, %sp, #128
/* spill store */
st %r0, %sp, #36

/* alloca */
addi %r0, %sp, #132
/* spill store */
st %r0, %sp, #40

ld %r0, %sp, #144
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #140
ld %r1, %sp, #16
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #136
ld %r1, %sp, #20
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #16
ld %r4, %r0, #0

ld %r0, %sp, #12
ld %r5, %r0, #0

ori %r0, %r4, #0
/* spill store */
st %r0, %sp, #44

ori %r4, %r5, #0

ld %r0, %sp, #44
sub %r4, %r0, %r4

divi %r4, %r4, #4

ld %r0, %sp, #24
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #24
ld %r4, %r0, #0

subi %r2, %r4, #1
isneg @p0, %r2
iszero @p1, %r2
orp @p0, @p0, @p1
@p0 ? jmpi _sort_66

_sort_13:
ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #48

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ld %r1, %sp, #48
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4084
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, get_pivot
ori %r4, %r0, #0

ld %r0, %sp, #28
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #28
ld %r4, %r0, #0

ld %r4, %r4, #0

ld %r0, %sp, #32
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r4, %r4, #4

ld %r0, %sp, #36
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #16
ld %r4, %r0, #0

/* GEP, input 1, sz=4 */
/* type =  20 */
ldi %r2, #255
shli %r2, %r2, #12
ori %r2, %r2, #4095
shli %r2, %r2, #12
ori %r2, %r2, #4092
add %r4, %r4, %r2

ld %r0, %sp, #40
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

ld %r5, %r4, #0

ld %r0, %sp, #28
ld %r4, %r0, #0

/* Gen store. */
st %r5, %r4, #0


_sort_27:
ld %r0, %sp, #36
ld %r4, %r0, #0

ld %r0, %sp, #40
ld %r5, %r0, #0

sub %r2, %r4, %r5
rtop @p0, %r2
@p0 ? jmpi _sort_31

_sort_45:
ld %r0, %sp, #36
ld %r4, %r0, #0

ld %r5, %r4, #0

ld %r0, %sp, #32
ld %r4, %r0, #0

sub %r2, %r5, %r4
isneg @p0, %r2
iszero @p1, %r2
orp @p0, @p0, @p1
notp @p0, @p0
@p0 ? jmpi _sort_50

_sort_53:
ld %r0, %sp, #36
ld %r4, %r0, #0

ld %r5, %r4, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

/* Gen store. */
st %r5, %r4, #0

ld %r0, %sp, #32
ld %r5, %r0, #0

ld %r0, %sp, #36
ld %r4, %r0, #0

/* Gen store. */
st %r5, %r4, #0

ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #96

ld %r0, %sp, #36
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ld %r1, %sp, #96
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4084
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, sort

ld %r0, %sp, #36
ld %r4, %r0, #0

/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r4, #4
/* spill store */
st %r0, %sp, #100

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ld %r1, %sp, #100
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4084
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, sort


_sort_66:
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #148
jmpr %ra

_sort_31:
ld %r0, %sp, #36
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #56

ld %r0, %sp, #56
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #60

ld %r0, %sp, #32
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #64

ld %r0, %sp, #60
ld %r1, %sp, #64
sub %r2, %r0, %r1
isneg @p0, %r2
iszero @p1, %r2
orp @p0, @p0, @p1
notp @p0, @p0
@p0 ? jmpi _sort_36

_sort_41:
ld %r0, %sp, #36
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #88

ld %r0, %sp, #88
/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r0, #4
/* spill store */
st %r0, %sp, #92

ld %r0, %sp, #92
ld %r1, %sp, #36
/* Gen store. */
st %r0, %r1, #0

jmpi _sort_27

_sort_36:
ld %r0, %sp, #36
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #72

ld %r0, %sp, #40
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #76

ld %r1, %sp, #72
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ld %r1, %sp, #76
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, swap

ld %r0, %sp, #40
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #80

ld %r0, %sp, #80
/* GEP, input 1, sz=4 */
/* type =  20 */
ldi %r2, #255
shli %r2, %r2, #12
ori %r2, %r2, #4095
shli %r2, %r2, #12
ori %r2, %r2, #4092
add %r0, %r0, %r2
/* spill store */
st %r0, %sp, #84

ld %r0, %sp, #84
ld %r1, %sp, #40
/* Gen store. */
st %r0, %r1, #0

jmpi _sort_27

_sort_50:
ld %r0, %sp, #36
ld %r4, %r0, #0

/* GEP, input 1, sz=4 */
/* type =  20 */
ldi %r2, #255
shli %r2, %r2, #12
ori %r2, %r2, #4095
shli %r2, %r2, #12
ori %r2, %r2, #4092
add %r4, %r4, %r2

ld %r0, %sp, #36
/* Gen store. */
st %r4, %r0, #0

jmpi _sort_53



.global
.perm x
sorted:
subi %sp, %sp, #96
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r5, %sp, #72

/* alloca */
addi %r0, %sp, #76
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #80
/* spill store */
st %r0, %sp, #16

/* alloca */
addi %r0, %sp, #84
/* spill store */
st %r0, %sp, #20

ld %r0, %sp, #92
/* Gen store. */
st %r0, %r5, #0

ld %r0, %sp, #88
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r4, %r5, #0

ld %r0, %sp, #16
/* Gen store. */
st %r4, %r0, #0

ld %r4, %r5, #0

/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r4, %r4, #4

ld %r0, %sp, #20
/* Gen store. */
st %r4, %r0, #0


_sorted_6:
ld %r0, %sp, #20
ld %r4, %r0, #0

ld %r0, %sp, #12
ld %r5, %r0, #0

sub %r2, %r4, %r5
rtop @p0, %r2
@p0 ? jmpi _sorted_10

_sorted_25:
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #96
jmpr %ra

_sorted_10:
ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #28

ld %r0, %sp, #28
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #32

ld %r0, %sp, #20
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #36

ld %r0, %sp, #36
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #40

ld %r0, %sp, #32
ld %r1, %sp, #40
sub %r2, %r0, %r1
isneg @p0, %r2
iszero @p1, %r2
orp @p0, @p0, @p1
notp @p0, @p0
@p0 ? jmpi _sorted_16

_sorted_20:
ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #56

ld %r0, %sp, #56
/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r0, #4
/* spill store */
st %r0, %sp, #60

ld %r0, %sp, #60
ld %r1, %sp, #16
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #20
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #64

ld %r0, %sp, #64
/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r0, #4
/* spill store */
st %r0, %sp, #68

ld %r0, %sp, #68
ld %r1, %sp, #20
/* Gen store. */
st %r0, %r1, #0

jmpi _sorted_6

_sorted_16:
ldi %r1, #88
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ldi %r1, #128
shli %r1, %r1, #12
shli %r1, %r1, #12
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, putc

ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #48

ld %r0, %sp, #48
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #52

ld %r1, %sp, #52
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, print_dec

jmpi _sorted_20



.global
.perm x
rng_unbiased:
subi %sp, %sp, #64
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r0, %sp, #40
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #44
/* spill store */
st %r0, %sp, #16

/* alloca */
addi %r0, %sp, #48
/* spill store */
st %r0, %sp, #20

/* alloca */
addi %r0, %sp, #52
/* spill store */
st %r0, %sp, #24

ld %r0, %sp, #60
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #56
ld %r1, %sp, #16
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

ldi %r2, #16
shli %r2, %r2, #12
div %r5, %r2, %r4

ld %r0, %sp, #12
ld %r4, %r0, #0

mul %r4, %r5, %r4

ld %r0, %sp, #20
/* Gen store. */
st %r4, %r0, #0


_rng_unbiased_7:
ld %r0, %sp, #16
ld %r4, %r0, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, rng
ori %r5, %r0, #0

ld %r0, %sp, #24
/* Gen store. */
st %r5, %r0, #0


_rng_unbiased_10:
ld %r0, %sp, #24
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #28

ld %r0, %sp, #20
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #32

ld %r0, %sp, #28
ld %r1, %sp, #32
sub %r2, %r0, %r1
isneg @p2, %r0
xor %r0, %r0, %r1
shri %r0, %r0, #31
rtop @p1, %r0
@p1 ? notp @p0, @p2
notp @p1, @p1
@p1 ? isneg @p0, %r2
notp @p0, @p0
@p0 ? jmpi _rng_unbiased_7

_rng_unbiased_14:
ld %r0, %sp, #24
ld %r5, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

mod %r4, %r5, %r4

ori %r0, %r4, #0
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #64
jmpr %ra



.global
.perm x
shuf:
subi %sp, %sp, #104
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r0, %sp, #80
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r0, %sp, #84
/* spill store */
st %r0, %sp, #16

/* alloca */
addi %r0, %sp, #88
/* spill store */
st %r0, %sp, #20

ld %r0, %sp, #100
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #96
ld %r1, %sp, #16
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #92
ld %r1, %sp, #20
/* Gen store. */
st %r0, %r1, #0


_shuf_4:
ld %r0, %sp, #12
ld %r4, %r0, #0

ld %r0, %sp, #16
ld %r5, %r0, #0

sub %r2, %r4, %r5
rtop @p0, %r2
@p0 ? jmpi _shuf_8

_shuf_23:
ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #104
jmpr %ra

_shuf_8:
ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #28

ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #32

ld %r0, %sp, #16
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #36

ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #40

ld %r0, %sp, #36
/* spill store */
st %r0, %sp, #44

ld %r0, %sp, #40
/* spill store */
st %r0, %sp, #48

ld %r0, %sp, #44
ld %r1, %sp, #48
sub %r0, %r0, %r1
/* spill store */
st %r0, %sp, #52

ld %r0, %sp, #52
divi %r0, %r0, #4
/* spill store */
st %r0, %sp, #56

ld %r0, %sp, #20
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #60

ld %r1, %sp, #56
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ld %r1, %sp, #60
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, rng_unbiased
/* spill store */
st %r0, %sp, #64

ld %r0, %sp, #32
ld %r1, %sp, #64
/* GEP, input 1, sz=4 */
/* type =  20 */
shli %r2, %r1, #2
add %r0, %r0, %r2
/* spill store */
st %r0, %sp, #68

ld %r1, %sp, #28
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r1, %r3, #0
ld %r1, %sp, #68
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, swap


_shuf_20:
ld %r0, %sp, #12
ld %r0, %r0, #0
/* spill store */
st %r0, %sp, #72

ld %r0, %sp, #72
/* GEP, input 1, sz=4 */
/* type =  20 */
addi %r0, %r0, #4
/* spill store */
st %r0, %sp, #76

ld %r0, %sp, #76
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

jmpi _shuf_4



.global
.perm x
start:
subi %sp, %sp, #56
st %r4, %sp, #4
st %r5, %sp, #8
st %ra, %sp, #0
/* alloca */
addi %r0, %sp, #28
/* spill store */
st %r0, %sp, #12

/* alloca */
addi %r4, %sp, #32

/* alloca */
addi %r0, %sp, #36
/* spill store */
st %r0, %sp, #16

/* alloca */
addi %r0, %sp, #40
/* spill store */
st %r0, %sp, #20

/* alloca */
addi %r0, %sp, #44
/* spill store */
st %r0, %sp, #24

ld %r0, %sp, #52
ld %r1, %sp, #12
/* Gen store. */
st %r0, %r1, #0

ld %r0, %sp, #48
/* Gen store. */
st %r0, %r4, #0

ld %r4, %r4, #0


ld %r0, %sp, #16
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #12
ld %r4, %r0, #0

divi %r4, %r4, #4

/* GEP, input 1, sz=4 */
/* type =  20 */
shli %r2, %r4, #2
add %r4, %r5, %r2

ld %r0, %sp, #20
/* Gen store. */
st %r4, %r0, #0

ld %r0, %sp, #24
/* Gen store. */
ldi %r1, #64206
st %r1, %r0, #0

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r4, %r3, #0
ld %r1, %sp, #24
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4084
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, sort

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, sorted

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r4, %r3, #0
ld %r1, %sp, #24
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4084
add %r3, %r3, %sp
st %r1, %r3, #0
jali %ra, shuf

ld %r0, %sp, #16
ld %r5, %r0, #0

ld %r0, %sp, #20
ld %r4, %r0, #0

ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4092
add %r3, %r3, %sp
st %r5, %r3, #0
ldi %r3, #255
shli %r3, %r3, #12
ori %r3, %r3, #4095
shli %r3, %r3, #12
ori %r3, %r3, #4088
add %r3, %r3, %sp
st %r4, %r3, #0
jali %ra, show_array

ld %ra, %sp, #0
ld %r4, %sp, #4
ld %r5, %sp, #8
addi %sp, %sp, #56
jmpr %ra



