# Header 
| Name  | Size | Description |
|-------|-------| -----------
|Magic  | 3 bytes     | The magic bytes are 0x67, 0x76, 0x6d|
|Version | 1 byte    |The version is the `MajorVersion * 10 + MinoorVersion` a version like `5.1` becomes `51`|
|CodeSize | 8 bytes   | This is the size of the code that will be ran, this is from the 33nd byte unward|
|NULL| 20 bytes| These are null bytes(0) they are reserved for future versions|


# Instructions 
In the code the first 8 bytes are expected to be the amount of variables that will be created in the global scope. After you can start writing you program. 

> Each instruction is 1 byte each

|Instruction|Id|Arguments|
|-----------|--|---------|
|MakeFrame | 0 | This instruction expects 8 bytes, which is the amount of variables this frame holds after the frame will be pushed to the top of the framestack|
|Print | 1 | This Calls the `repr` function on an object a prints the output to stdout |
|Push | 2 | This instruction expects 1 byte which is the type that will be pushed 1 being for a number. The other bytes are determined by the type expcted a number expects 8 bytes|



