main: _endupv 0
    push number 100
    store_local 0
    foo: 0 _endupv 0
        load_local 0
        push number 250
        add
        ret
    endfunc
    ret
endfunc
store_local 0

load_local 0
call
call