#pragma once
#include <Lib/Types.h>
#include <Lib/Stream.h>

namespace System
{
    typedef void (*CommandCallback)(char* input, char** argv, int argc);

    typedef struct
    {
        char* Name;
        char* Help;
        char* Usage;
        CommandCallback Callback;
    } PACKED Command;

    class CommandLine
    {
        public:
            Command** Commands;
            uint32_t  Count;
            uint32_t  CountMax;
            int32_t   ArgumentCount;
            char**    Arguments;
            char*     CurrentPath;
            bool      Enabled;

        public:
            Stream    KBStream;

        private:
            bool EnterDown;

        public:
            void Init();
            void Enable();
            void Disable();
            void PrintCaret();
            bool Register(Command* cmd);
            bool Unregister(Command* cmd);
            void Execute(char* input);
            void Monitor();

        public:
            char* GetPath(char* path);

        private:
            int GetFreeIndex();
    };

    namespace CommandMethods
    {
        void CLS(char* input, char** argv, int argc);
        void HELP(char* input, char** argv, int argc);
        void HEAP(char* input, char** argv, int argc);
        void PMM(char* input, char** argv, int argc);
        void VMM(char* input, char** argv, int argc);
        void PROC(char* input, char** argv, int argc);
        void FSINFO(char* input, char** argv, int argc);
        void EXEC(char* input, char** argv, int argc);
        void PKILL(char* input, char** argv, int argc);

        void LS(char* input, char** argv, int argc);
        void CD(char* input, char** argv, int argc);
    }

    namespace Commands
    {
        static const Command CLS        = { "CLS",    "Clear the screen", "cls", CommandMethods::CLS };
        static const Command HELP       = { "HELP",   "Show list of commands", "help", CommandMethods::HELP };
        static const Command HEAP       = { "HEAP",   "Show list of heap allocations", "heap", CommandMethods::HEAP };
        static const Command PMM        = { "PMM",    "Show physical memory information", "pmm", CommandMethods::PMM };
        static const Command VMM        = { "VMM",    "Show virtual memory information", "pmm", CommandMethods::VMM };
        static const Command PROC       = { "PROC",   "Show list of processes", "proc", CommandMethods::PROC };
        static const Command FSINFO     = { "FSINFO", "Show file system information", "fsinfo [b : block, f : files, p : file properties", CommandMethods::FSINFO };
        static const Command EXEC       = { "EXEC", "Executable a specified binary file", "exec [path] [args]", CommandMethods::EXEC };
        static const Command PKILL      = { "PKILL", "Terminate a specified process", "pkill [-i : index, -n : name, -na : all with name]", CommandMethods::PKILL };

        static const Command LS         = { "LS", "List contents of specified directory", "ls [path]", CommandMethods::LS };
        static const Command CD         = { "CD", "Set the current directory", "cd [path]", CommandMethods::CD };
    }
}