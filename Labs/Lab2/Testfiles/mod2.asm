one: equ 1
zero: equ 0
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
if one
jmpp mod3
if zero
jmp wrong
section data
r: space 2
ten: const 10
end