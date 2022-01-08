#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

namespace pmlib
{
    class Point
    {
        public:
            int X, Y;

        public:
            Point()             { X = 0; Y = 0; }
            Point(int x, int y) { X = x; Y = y; }

        public:
            bool Equals(Point& p) { return X == p.X && Y == p.Y; }
    };

    class Rectangle
    {
        public:
            int X, Y, Width, Height;

        public:
            Rectangle()                           { X = 0; Y = 0; Width = 0; Height = 0; }
            Rectangle(int x, int y, int w, int h) { X = x; Y = y; Width = w; Height = h; }

        public:
            void SetPosition(int x, int y) { X = x; Y = y; }
            void SetSize(int w, int h)     { Width = w; Height = h; }
        
        public:
            bool Intersects(Rectangle& r) { return false; }
            bool Intersects(Point& p)     { return false; }
            bool Intersects(int x, int y) { return false; }
            bool Equals(Rectangle& r)     { return X == r.X && Y == r.Y && Width == r.Width && Height == r.Height; }
    };
}