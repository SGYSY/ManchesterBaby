; THIS PROGRAM ADDS TWO NUMBERS TOGETHER
; The program starts at address 1, so we need to pad out address 0 
          VAR 0       ; Declare 32-bit variable to fill space 
START:    LDN NUM01   ; Copy variable to accumulator (negated)
          SUB NUM02   ; Subtract variable from accumulator to get sum (negated)
          STO MYSUM   ; Store accumulator to variable - saves answer
; The next two instructions negate the stored answer
          LDN MYSUM   ; Copy variable to accumulator (negated)
          STO MYSUM   ; Store accumulator to variable
END:      STP         ; Stop processor
; Variable declarations follow
NUM01:    VAR 1025    ; Declare 32-bit variable
NUM02:    VAR 621     ; Declare 32-bit variable
MYSUM:    VAR 0       ; Declare 32-bit variable
