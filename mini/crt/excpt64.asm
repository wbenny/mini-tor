EXTRN  mini_ThrowException:PROC

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
    ; error LNK2001: unresolved external symbol _CxxThrowException
    ;
    _CxxThrowException PROC PUBLIC
        jmp mini_ThrowException
    _CxxThrowException ENDP

END
