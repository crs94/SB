;true: equ 1
;false: equ 0
mod3: begin
section text
r: extern
back: extern
public mod3
public two
public five
output three
output r
load minus
;if true
if 1
jmpn back
;if false
if 0
jmpz dont
section data
two: const 2
three: const 3
five: const 5
minus: const -0x01
end
