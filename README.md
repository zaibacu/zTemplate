License:
-------------------------
This project is licensed under MIT license, look at LICENSE.md file

Build Status:
-------------------------
[![Build Status](https://travis-ci.org/zaibacu/zTemplate.png?branch=master)](https://travis-ci.org/zaibacu/zTemplate)

Description:
-------------------------
Web template rendering library written in C. 
Nothing fancy, just trying to squeez some speed

Usage:
-------------------------
Common variable insert:

Hello, << $name >>

Common include:

<< include hello_world.html >>

IF block:

[[ if $some_variable ]]
 You may pass
[[end]]

Foreach block:

[[ foreach $tree in $forest ]]
 << $tree >>
[[end]]

Python API:

from zRender import *

r = zRender()

rendered_html = r.render("index.html", {"name": "Mr. Awesome", "company": "Awesome Inc."})

Version history:
-------------------------
Version 0.2:
  * Possibility to make block statement
  * Possiblity to iterate through collection
  
Version 0.1:
  * Possibility to render value
  * Possiblity to use include
  * Basic tests
