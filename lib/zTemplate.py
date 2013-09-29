import platform
from copy import *
from ctypes import *

class Param(Structure): #Forward declaration
	pass

class Value(Structure):
	pass

class StringValue(Structure):
	pass

class BoolValue(Structure):
	pass

class NumberValue(Structure):
	pass

class ListValue(Structure):
	pass

PARAM_P = POINTER(Param)
VALUE_P = POINTER(Value)
LIST_P = POINTER(ListValue)

Value._fields_ = [
	("type", c_uint),
	("val", c_void_p)
]

StringValue._fields_ = [
	("value", c_char_p)
]

BoolValue._fields_ = [
	("value", c_bool)
]

NumberValue._fields_ = [
	("value", c_int)
]

ListValue._fields_ = [
	("value", VALUE_P),
	("next", LIST_P)
]

Param._fields_ = [
		("key", c_char_p),
		("value", VALUE_P),
		("next", PARAM_P)
	]

class zTemplate(object):
	def __init__(self):
		if platform.system() == "Windows":
			self.lib = cdll.LoadLibrary("bin/zTemplate.dll")
		else: 
			self.lib = cdll.LoadLibrary("bin/zTemplate.so")
		self.lib.render.restype = c_char_p
		self.lib.render.argtype = [c_char_p, PARAM_P]

		self.lib.render_text.restype = c_char_p
		self.lib.render.argtype = [c_char_p, PARAM_P]

	def handle_type(self, value):
		v = Value()
		if type(value) == list:
			v.type = 4
			rev = value[:]
			rev.reverse()
			prev_item = None
			for item in rev:
				lv = ListValue()
				self.Values.append(lv)
				lv.value = VALUE_P(self.handle_type(item))
				if prev_item != None:
					lv.next = LIST_P(prev_item)
				prev_item = lv

			v.val = cast(byref(lv), c_void_p)

		elif type(value) == dict:
			pass
		elif type(value) == str:
			sv = StringValue()
			sv.value = value.encode("UTF-8")
			self.Values.append(sv)
			v.type = 1
			v.val = cast(byref(sv), c_void_p)
		elif type(value) == bool:
			bv = BoolValue()
			bv.value = value
			self.Values.append(bv)
			v.type = 2
			v.val = cast(byref(bv), c_void_p)
		elif type(value) == int:
			nv = NumberValue()
			nv.value = value
			self.Values.append(nv)
			v.type = 3
			v.val = cast(byref(nv), c_void_p)
		else:
			print("Unhandled type %s" % type(value))

		return v

	def render(self, file, params = {}):
		root = self.construct_params(params)
		return self.lib.render(file.encode("UTF-8"), byref(root))

	def render_text(self, text, params = {}):
		root = self.construct_params(params)
		return self.lib.render_text(text.encode("UTF-8"), byref(root))


	def construct_params(self, params):
		root = Param()
		cursor = root
		self.Values = [] #Just to keep our value structs not destroyed
		for key, value in params.items():
			if type(value) == dict:
				for name, member in value.items():
					p = Param()
					p.key = ("%s->%s" % (key, name)).encode("UTF-8")
					v = self.handle_type(member)
					p.value = VALUE_P(v)
					cursor.next = PARAM_P(p)
					cursor = p
			else:
				p = Param()
				p.key = key.encode("UTF-8")
				v = self.handle_type(value)
				p.value = VALUE_P(v)
				cursor.next = PARAM_P(p)
				cursor = p

		return root