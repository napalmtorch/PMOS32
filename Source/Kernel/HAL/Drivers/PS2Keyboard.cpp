#include <Kernel/HAL/Drivers/PS2Keyboard.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

// PS2 keyboard interrupt callback handler
void PS2KeyboardCallback(HAL::Registers32* regs)
{
    // check status
    uint8_t status = HAL::Ports::Read8(0x64);
    if ((status & 0b00000001) == 0) { return; }

    // read scancode
    uint8_t code = HAL::Ports::Read8(0x60);

    // handle keyboard input
    Core::Keyboard.Handle(code);
    Core::Keyboard.UpdateKeymap(code);

    // unused register argument
    UNUSED(regs);
}

namespace HAL
{
    namespace Drivers
    {
        // Initialize PS2 keyboard driver
        void PS2Keyboard::Init()
        {
            // register interrupt
            IDT::RegisterInterrupt(IRQ1, (InterruptCallback)PS2KeyboardCallback);

            HAL::Ports::Write8(0x64, 0xAE);

            // clear states
            ClearKeymap();

            Debug::OK("Started PS/2 keyboard driver");
        }

        // Disable PS2 keyboard driver
        void PS2Keyboard::Disable()
        {
            // unregister interrupt
           IDT::UnregisterInterrupt(IRQ1);

            // message
            Debug::Warning("Stopped PS/2 keyboard driver");
        }

        /// @brief Handle PS2 keyboard scancode @param key Scancode value
        void PS2Keyboard::Handle(uint8_t key)
        {
            CurrentKey = key;

            if (CurrentStream != nullptr)
            {
                char* out = (char*)CurrentStream->ToArray();

                if (key == (uint8_t)KeyPressed::BACKSPACE)
                {
                    // handle backspace
                    if (strlen(out) > 0)
                    {
                        if (CurrentStream->GetSize() == 0) { return; }
                        strdel(out);
                        CurrentStream->Seek(CurrentStream->GetPosition() - 1);
                        if (TerminalOutput) { Core::Terminal.Delete(); }
                    }
                }
                else if (key == (uint8_t)KeyPressed::LSHIFT) { LShiftDown = true; }
                else if (key == (uint8_t)KeyPressed::RSHIFT) { RShiftDown = true; }
                else if (key == (uint8_t)KeyReleased::LSHIFT) { LShiftDown = false; }
                else if (key == (uint8_t)KeyReleased::RSHIFT) { RShiftDown = false; }
                else if (key == (uint8_t)KeyPressed::CAPS_LOCK) { CapsDown = !CapsDown; }
                else if (key == (uint8_t)KeyPressed::LCTRL) { }
                else if (key == (uint8_t)KeyPressed::LALT) { }
                else if (key == (uint8_t)KeyPressed::ESCAPE) { }
                else if (key == (uint8_t)KeyPressed::ENTER)
                {
                    if (TerminalOutput) { Core::Terminal.NewLine(); }
                    if (OnEnterPressed != nullptr) { OnEnterPressed(CurrentStream); }
                }
                else if (key < 58)
                {
                    char ascii = 0;
                    bool caps = (((LShiftDown || RShiftDown) && !CapsDown) || ((!LShiftDown && !RShiftDown) && CapsDown));

                    if (caps) { ascii = KBLayoutUS.Uppercase[key]; }
                    else      { ascii = KBLayoutUS.Lowercase[key]; }

                    CurrentStream->Write(ascii);
                    if (TerminalOutput) { Core::Terminal.WriteChar(ascii); }
                }
            }
        }

        /// @brief Set current stream @param stream Pointer to stream
        void PS2Keyboard::SetStream(Stream* stream) { CurrentStream = stream; }

        /// @brief Get current stream @return Pointer to stream
        Stream* PS2Keyboard::GetStream() { return CurrentStream; }

        // Clear all values in keymap
        void PS2Keyboard::ClearKeymap() { for (uint16_t i = 0; i < 256; i++) { Keymap[i] = false; } }

        /// @brief Update value in keymap @param key Scancode value
        void PS2Keyboard::UpdateKeymap(uint8_t key)
        {
            if (key <= 0x58) { Keymap[(int)key] = true; }
            else if (key >= 0x81 && key < 0xD8) { Keymap[(int)key - 128] = false; }
        }

        /// @brief Check if key is pressed @param key Key value @return Key state
        bool PS2Keyboard::IsKeyDown(Key key)
        {
            return Keymap[(int)key];
        }

        /// @brief Check if key is released @param key Key value @return Key state
        bool PS2Keyboard::IsKeyUp(Key key)
        {
            return Keymap[(int)key] == false ? true : false;
        }
    }
}