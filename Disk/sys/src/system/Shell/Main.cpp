#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Time.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/gfx/Image.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>

int _start(void* arg)
{
    clistate(false);
    lib_init();
    printf("Initialized shell process\n");
    
    uint32_t fbw = pmlib::GetFrameBufferWidth();
    uint32_t fbh = pmlib::GetFrameBufferHeight();
    uint32_t* fb = pmlib::GetFrameBuffer();

    pmlib::Image image;
    image.Create(fbw, fbh);

    pmlib::DateTime time;
    uint32_t sec, last_sec, fps, frames;
    char fps_string[32];
    memset(fps_string, 0, 32);

    while (true)
    {
        frames++;
        time = pmlib::GetCurrentTime();

        sec = time.Second;
        if (sec != last_sec)
        {
            fps = frames;
            ltoa(fps, fps_string, 10);
            frames = 0;
            last_sec = sec;
        }

        image.Clear(0xFF007F7F);

        image.DrawFilledRect(pmlib::GetMouseX(), pmlib::GetMouseY(), 8, 8, 0xFFFFFFFF);
        image.DrawString(0, 0, (char*)"FPS: ", 0xFFFFFFFF, pmlib::Fonts::Serif8x16);
        image.DrawString(40, 0, fps_string, 0xFFFFFFFF, pmlib::Fonts::Serif8x16);

        if (pmlib::KeyboardDown(pmlib::Key::End)) { break; }

        pmlib::Render(0, 0, fbw, fbh, image.GetData());
        yield();
    }

    kfree(image.GetData());
    clistate(true);
    yield();
    return 0;
}