We provide a C++ wrapper of Windows overlapping sockets
that use IO completion ports.


We could use inheritance:

generic platform indpendent socket -----> linux socket impl
    |
    |
    |
    V
windows socket impl

dont think this works well, since we have different api's for win and linux.

Use macros --> let's do it.