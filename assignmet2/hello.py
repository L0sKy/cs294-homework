import ctypes

lib = ctypes.CDLL("./libc_interface.so")

lib.helloworld()
print(lib.aplusb(3, 4))
