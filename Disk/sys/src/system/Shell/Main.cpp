#include <sys/lib/Types.h>
#include <sys/lib/Library.h>
#include <sys/lib/Common.h>
#include <sys/lib/Time.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/gfx/Image.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>

int _start(void* arg)
{
    clistate(false);
    lib_init();
    printf("%s, %d, 0x%8x\n", "Hello world", 1234, 0xDEADC0DE);
    
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

        if (pmlib::KeyboardDown(pmlib::Key::End)) 
        { 
            clistate(true);
            exit(0); 
        }

        pmlib::Render(0, 0, fbw, fbh, image.GetData());
    }

    kfree(image.GetData());

    return 1234;
}