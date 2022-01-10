#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/api/Library.h>

namespace pmlib
{
    static inline bool SendProcMsg(char* name, ProcessMessage msg) { return ((MTYPE_SEND_MSG)MTABLE_ENTRIES[17].addr)(name, msg); }
    static inline ProcessMessage ReceiveProcMsg() { return ((MTYPE_RECV_MSG)MTABLE_ENTRIES[18].addr)(); }
    static inline bool IsProcMsgReady() { return ((MTYPE_MSG_READY)MTABLE_ENTRIES[19].addr)(); }

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
            bool Intersects(Rectangle& r) { return Intersects(r.X, r.Y); }
            bool Intersects(Point& p)     { return Intersects(p.X, p.Y); }
            bool Intersects(int x, int y) { return (x >= X && y >= Y && x <= X + Width && y <= Y + Height); }
            bool Equals(Rectangle& r)     { return X == r.X && Y == r.Y && Width == r.Width && Height == r.Height; }
    };
}