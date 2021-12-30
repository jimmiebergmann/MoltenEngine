/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/


namespace Molten
{

    // Triangle 2
    template<typename T>
    Triangle<2, T>::Triangle() :
        p0{},
        p1{},
        p2{}
    {}

    template<typename T>
    Triangle<2, T>::Triangle(
        Vector<2, T> p0,
        Vector<2, T> p1,
        Vector<2, T> p2
    ):
        p0(p0),
        p1(p1),
        p2(p2)
    {}

    template<typename T>
    bool Triangle<2, T>::Intersects(const Vector<2, T>& point) const
    {
        const auto s = (p0.x - p2.x) * (point.y - p2.y) - (p0.y - p2.y) * (point.x - p2.x);
        const auto t = (p1.x - p0.x) * (point.y - p0.y) - (p1.y - p0.y) * (point.x - p0.x);

        if ((s < 0) != (t < 0) && s != 0 && t != 0)
        {
            return false;
        }

        const auto  d = (p2.x - p1.x) * (point.y - p1.y) - (p2.y - p1.y) * (point.x - p1.x);
        return d == 0 || (d < 0) == (s + t <= 0);
    }

}