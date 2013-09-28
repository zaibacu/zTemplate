import sys
import unittest
import imp
lib = imp.load_source("lib.zTemplate", "lib/zTemplate.py")

class TestRendering(unittest.TestCase):
	def setUp(self):
		self.template = lib.zTemplate()

	def test_render_basic(self):
		answer = ""
		with open("tests/test_tmpl1_result.html") as f:
			answer = f.read().encode("UTF-8")
		self.assertEqual(self.template.render("../tests/test_tmpl1.html", { "word": "world", "greeting": "Hello"}), answer)

	def test_show_block(self):
		result = self.template.render("../tests/test_tmpl3.html", { "show_me": True, "hide_me": False}).decode("UTF-8")
		self.assertTrue(result.find("Hidden") == -1 and result.find("Visible") >= 0)

if __name__ == '__main__':
    unittest.main() 
