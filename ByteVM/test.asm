push number 100
store_local 0

main: 0 _endupv 1
    store_local 1

    load_local 0
    load_local 1

    add
    ret
endfunc
store_local 1

foo: _endupv 0
    push number 150
    push number 150

    add
    ret
endfunc
store_local 2

push number 200
load_local 1
call

load_local 2
call