import platform
from copy import *
from ctypes import *

class Param(Structure): #Forward declaration
	pass

class Value(Structure):
	pass

class StringValue(Structure):
	pass

PARAM_P = POINTER(Param)

Value._fields_ = [
	("type", c_uint),
	("val", c_void_p)
]

StringValue._fields_ = [
	("value", c_char_p)
]

Param._fields_ = [
		("key", c_char_p),
		("value", Value),
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
		self.Values = [] #Just to keep our value structs not destroyed
		for key, value in params.items():
			v = Value()
			p = Param()
			p.key = key.encode("UTF-8")
			if type(value) == list:
				pass
			elif type(value) == dict:
				pass
			else:
				sv = StringValue()
				sv.value = value.encode("UTF-8")
				self.Values.append(sv)
				v.type = 1
				v.val = cast(byref(sv), c_void_p)

			p.value = v
			cursor.next = PARAM_P(p)
			cursor = p
			
		return self.lib.render(file.encode("UTF-8"), byref(root))