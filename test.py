from zRender import *

r = zRender()

print(r.render("test3.html", {"bye": "Goodbye, cruel world...", "hello": "Hello world!", "hello2": "Hello world2!"}))