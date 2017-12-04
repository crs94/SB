one: equ 1
zero: equ 0
mod1: begin
section text
mod2: extern
mod3: extern
public n1
public n2
public back
public wrong
input n1
wrong: input n2
if one
jmp mod2
if zero
jmp mod3
back: stop
section data
n1: space 5
n2: space 
end