;one2: equ 1
;zero2: equ 0
mod2: begin
section text
n1: extern
n2: extern
mod3: extern
wrong: extern
two: extern
five: extern
public mod2
public r
load n1
add n2
dont: sub ten
mult two
div five
store r
load ten
;if one2
if 1
jmpp mod3
;if zero2
if 0
jmp wrong
section data
r: space 2
ten: const 10
end