#pragma once
#include <Lib/Types.h>
#include <Lib/Stream.h>
#include <Kernel/HAL/Drivers/Key.h>

namespace HAL
{
    typedef struct
    {
        char Uppercase[60];
        char Lowercase[60];
    } PACKED KeyboardLayout;


    namespace Drivers
    {
        static const KeyboardLayout KBLayoutUS
        {
            "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? \0",
            "??1234567890-=??qwertyuiop[]??asdfghjkl;'`?\\zxcvbnm,./??? \0",
        };

        //static const char* KBLayoutUS = "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>????";

        class PS2Keyboard
        {
            private:
                bool          Keymap[256];
                uint8_t       CurrentKey;
                uint8_t       PreviousKey;
                Stream*       CurrentStream;

            private:
                bool LShiftDown, RShiftDown;
                bool CapsDown;
                bool EnterHandled;

            public:
                void (*OnEnterPressed)(Stream* stream);
                bool TerminalOutput;

            public:
                void Init();
                void Disable();
                void Handle(uint8_t key);
                void SetStream(Stream* stream);

            public:
                bool IsKeyDown(Key key);
                bool IsKeyUp(Key key);

            public:
                Stream* GetStream();

            public:
                void ClearKeymap();
                void UpdateKeymap(uint8_t key);
        };
    }
}