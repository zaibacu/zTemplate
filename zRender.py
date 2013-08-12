import platform
from ctypes import *

class Param(Structure): #Forward declaration
	pass

PARAM_P = POINTER(Param)
Param._fields_ = [
		("key", c_char_p),
		("value", c_char_p),
		("next", PARAM_P)
	]

class zRender(object):
	def __init__(self):
		if platform.system() == "Windows":
			self.lib = cdll.LoadLibrary("bin/zRender.dll")
		else: 
			self.lib = cdll.LoadLibrary("bin/zRender.so")
		self.lib.render.restype = c_char_p
		self.lib.render.argtype = [c_char_p, PARAM_P]

	def render(self, file, params = {}):
		root = Param()
		cursor = root
		for key, value in params.items():
			p = Param()
			p.key = key.encode("UTF-8")
			p.value = value.encode("UTF-8")
			cursor.next = PARAM_P(p)
			cursor = p
			
		return self.lib.render(file.encode("UTF-8"), byref(root))