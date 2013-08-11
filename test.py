from ctypes import *

class Param(Structure): #Forward declaration
	pass

Param._fields_ = [
		("key", c_char_p),
		("value", c_char_p),
		("next", POINTER(Param))
	]


zRender = cdll.LoadLibrary("bin/zRender.so")

zRender.Render.restype = c_char_p
zRender.Render.argtype = [c_char_p, Param]


p = Param();
p.key = b"Hello"
p.value = b"Hello world!"

print(zRender.Render(b"test.html", p))