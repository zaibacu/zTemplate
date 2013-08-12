License:
-------------------------
This project is licensed under MIT license, look at LICENSE.md file


Description:
-------------------------
Web template rendering library written in C. 
Nothing fancy, just trying to squeez some speed

Usage:
-------------------------
Common variable insert:

<html>
 <body>
  Hello, << $name >>
 </body>
</html>

Common include:

<html>
 <body>
  << include hello_world.html >>
 </body>
</html>


Python API:

from zRender import *

r = zRender()
rendered_html = r.render("index.html", {"name": "Mr. Awesome", "company": "Awesome Inc."})

Version history:
-------------------------
Version 0.1:
  * Possibility to render value
  * Possiblity to use include
  * Basic tests
