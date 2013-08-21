from zRender import *

r = zRender()

print(r.render("test3.html", {	"bye": "Goodbye, cruel world...", 
								"hello": "Hello world!", 
								"say_goodbye": True,
								"some_numbers": [ 5, 4, "three", "two", "one!" ]
								}))