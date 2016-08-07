.386
.MODEL FLAT

EXTRN  _mini_ThrowException@8:PROC

.DATA

    ;
    ; Solves:
    ; error LNK2001: unresolved external symbol "const type_info::`vftable'" (??_7type_info@@6B@)
    ;
    ??_7type_info@@6B@ dw ?  
    PUBLIC ??_7type_info@@6B@

.CODE

    ;
    ; Solves:
    ; error LNK2001: unresolved external symbol __CxxThrowException@8
    ;
    __CxxThrowException@8 PROC PUBLIC
        jmp _mini_ThrowException@8
    __CxxThrowException@8 ENDP

END
