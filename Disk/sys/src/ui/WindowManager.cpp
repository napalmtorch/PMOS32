#include <sys/lib/stdio.h>
#include <sys/lib/ui/WindowManager.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>

namespace pmgui
{
    void WindowManager::Init()
    {
        Count        = 0;
        CountMax     = 256;
        ActiveWindow = nullptr;
        ActiveIndex  = -1;
        Windows      = (Window**)kmalloc(CountMax * sizeof(Window*), true, MEMTYPE_PTRARRAY);   
        printf("Initialized window manager\n");
    }

    void WindowManager::Dispose()
    {
        if (Windows != nullptr) { kfree(Windows); }
    }

    void WindowManager::Update()
    {
        if (Windows == nullptr) { return; }
        if (Count == 0) { return; }

        uint32_t hover = 0, moving = 0, resizing = 0;
        int top_hover_index = 0, last_hover = -1, active = 0;
        
        for (uint32_t i = 0; i < Count; i++)
        {
            if (Windows[i] == nullptr) { continue; }

            if (Windows[i]->Bounds.Intersects(pmlib::GetMouseX(), pmlib::GetMouseY())) 
            {
                hover++;
                if ((int)i > top_hover_index) { top_hover_index = i; } 
            }

            if (Windows[i]->Moving) { moving++; }
            if (Windows[i] == ActiveWindow) { active = i; }
            Windows[i]->State.Focused = (Windows[i] == ActiveWindow);
            if ((int)i > last_hover && i < Count - 1) { last_hover = i; }

            printf("UPDATING WIN 0x%8x, %s\n", Windows[i], Windows[i]->Name);
            Windows[i]->Update();
            printf("WIN UPDATEED\n");
        }

        ActiveIndex = active;

        if (hover + moving + resizing == 0 && pmlib::GetMouseLeftBtn()) { ActiveWindow = nullptr; ActiveIndex = -1; }

        if (moving == 0 && resizing == 0)
        {
            for (uint32_t i = 0; i < Count; i++)
            {
                if (Windows[i] == nullptr) { continue; }

                if (ActiveWindow == nullptr)
                {
                    if (Windows[i]->Bounds.Intersects(pmlib::GetMouseX(), pmlib::GetMouseY()))
                    {
                        if (pmlib::GetMouseLeftBtn() && !MouseDown) { SetActiveWindow(Windows[i]); MouseDown = true; break; }
                    }
                }
                else
                {
                    if (Windows[i]->Bounds.Intersects(pmlib::GetMouseX(), pmlib::GetMouseY()))
                    {
                        if (pmlib::GetMouseLeftBtn())
                        {
                            if (ActiveIndex != (int)i && ActiveWindow != Windows[i] && !ActiveWindow->Bounds.Intersects(pmlib::GetMouseX(), pmlib::GetMouseY()) && !MouseDown)
                            { SetActiveWindow(Windows[i]); MouseDown = true; break; }
                        }
                    }
                }
            }
        }

        if (!pmlib::GetMouseLeftBtn()) { MouseDown = false; }
    }
    
    void WindowManager::Render()
    {
        if (Windows == nullptr) { return; }

        for (uint32_t i = 0; i < Count; i++)
        {
            if (Windows[i] == nullptr) { continue; }
            Windows[i]->Render();
        }
    }

    bool WindowManager::Add(Window* window)
    {
        if (window == nullptr) { return false; }
        if (Windows == nullptr) { return false; }
        if (Count >= CountMax) { printf("Maximum amount of windows reached\n"); return false; }

        Windows[Count] = window;
        Count++;
        ActiveWindow = window;
        ActiveIndex  = Count - 1;
        window->State.Focused = true;
        printf("Added window - '%s'\n", window->Name);
        return Windows[Count - 1];
    }

    bool WindowManager::Remove(Window* window, bool free)
    {
        return false;
    }

    Window* WindowManager::SetActiveWindow(Window* win)
    {
        if (win == nullptr) { return nullptr; }
        if (Windows == nullptr) { return nullptr; }
        if (Count == 0) { return nullptr; }
        
        int index = -1;
        for (uint32_t i = 0; i < Count; i++) { if (Windows[i] == win) { index = i; break; } }
        if (index < 0 || index >= (int)Count) { printf("Invalid index while setting active window\n"); return nullptr; }

        uint8_t* start = (uint8_t*)((uint32_t)Windows + (sizeof(Window*) * index));
        uint8_t* end   = (uint8_t*)((uint32_t)Windows + (sizeof(Window*) * CountMax));

        for (uint32_t i = (uint32_t)start; i < (uint32_t)end; i += sizeof(Window*))
        {
            if (i + sizeof(Window*) < (uint32_t)end) { memcpy((void*)i, (void*)(i + sizeof(Window*)), sizeof(Window*)); }
        }
        
        ActiveWindow = win;
        Windows[Count - 1] = win;
        ActiveIndex = index;
        printf("SET ACTIVE WINDOW\n");
        return ActiveWindow;
    }
}