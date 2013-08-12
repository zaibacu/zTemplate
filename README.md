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

&lt;html&gt;
 &lt;body&gt;
  Hello, << $name >>
 &lt;/body&gt;
&lt;/html&gt;

Common include:

&lt;html&gt;
 &lt;body&gt;
  << include hello_world.html >>
 &lt;/body&gt;
&lt;/html&gt;


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
