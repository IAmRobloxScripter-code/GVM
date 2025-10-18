push number 100
store_local 0

main: 0 _endupv 1
    load_local 0
    load_local 1

    add
    ret
endfunc
store_local 1

foo: _endupv 2
    load_local 0
    load_local 1
    add
    ret
endfunc
store_local 2

push number 200
load_local 1
call

push number 200

load_local 2
call