/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [aabounding.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <math.h>

#include "aabounding.h"
#include "common.h"


EXPORT inline bool AabCollisionRectRect(struct AabRectangle a, struct AabRectangle b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y)
		return false;

	return true;
}


EXPORT inline bool AabCollisionRectCircle(struct AabRectangle rect, struct Circle c)
{

	if (c.origin.x < (rect.min.x - c.radius) || c.origin.x > (rect.max.x + c.radius) ||
	    c.origin.y < (rect.min.y - c.radius) || c.origin.y > (rect.max.y + c.radius))
		return false;

	return true;
}


EXPORT inline bool AabCollisionBoxBox(struct AabBox a, struct AabBox b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y || a.max.z < b.min.z ||
	    a.min.z > b.max.z)
		return false;

	return true;
}


EXPORT inline bool AabCollisionBoxSphere(struct AabBox box, struct Sphere s)
{
	if (s.origin.x < (box.min.x - s.radius) || s.origin.x > (box.max.x + s.radius) ||
	    s.origin.y < (box.min.y - s.radius) || s.origin.y > (box.max.y + s.radius) ||
	    s.origin.z < (box.min.z - s.radius) || s.origin.z > (box.max.z + s.radius))
		return false;

	return true;
}


EXPORT inline struct AabRectangle AabToRectangle(struct AabBox box)
{
	return (struct AabRectangle){.min = {box.min.x, box.min.y}, .max = {box.max.x, box.max.y}};
}


EXPORT inline struct AabBox AabToBox(struct AabRectangle rect, float min_z, float max_z)
{
	return (struct AabBox){.min = {rect.min.x, rect.min.y, min_z}, .max = {rect.max.x, rect.max.y, max_z}};
}


EXPORT inline struct Vector2 AabMiddleRect(struct AabRectangle rect)
{
	return (struct Vector2){.x = (rect.min.x + (rect.max.x - rect.min.x) / 2.0f),
	                        .y = (rect.min.y + (rect.max.y - rect.min.y) / 2.0f)};
}


EXPORT inline struct Vector3 AabMiddleBox(struct AabBox box)
{
	return (struct Vector3){.x = (box.min.x + (box.max.x - box.min.x) / 2.0f),
	                        .y = (box.min.y + (box.max.y - box.min.y) / 2.0f),
	                        .z = (box.min.z + (box.max.z - box.min.z) / 2.0f)};
}
