#pragma once
#include <Lib/Types.h>
#include <Kernel/System/Threading.h>
#include <Kernel/HAL/Memory/VMM.h>

namespace System
{
    typedef struct
    {
        char     Message[64];
        uint32_t Size;
        uint8_t* Data;
    } ProcessMessage;

    class Process
    {
        public:
            Threading::Thread** Threads;
            uint32_t            ThreadCount;
            uint32_t            ThreadCountMax;
            uint32_t            Index;
            uint32_t            MessageCount;
            uint32_t            Virtual;
            ProcessMessage*     Messages;

        public:
            uint8_t*  ProgramData;
            uint32_t  ProgramSize;
        
        public:
            HAL::Memory::PageDirectory* PageDir;
            bool     Running;
            uint32_t Switched;
            uint32_t ID;
            char     Name[64];
        
        public:
            void Init(char* name, Threading::ThreadProtocol protocol);
            static Process* Create(char* name, Threading::ThreadProtocol protocol);
            static Process* CreateELF(char* name, uint8_t* data, uint32_t size);
            static Process* CreateKernel();

        public:
            bool IsMessageReady();
            bool PushMessage(ProcessMessage msg);
            ProcessMessage PopMessage();

        public:
            bool Start();
            bool Terminate();

        public:
            bool LoadThread(Threading::Thread* thread);
            bool UnloadThread(Threading::Thread* thread);
            bool StartThread(Threading::Thread* thread);
            bool TerminateThread(Threading::Thread* thread);

        private:
            int GetFreeIndex();
    };

    class ProcessManager
    {
        public:
            Process** Processes;
            Process*  CurrentProc;
            Process*  NextProc;
            uint32_t  Count;
            uint32_t  CountMax;
            uint32_t  Index;
            

        public:
            void Init();
            void Print();
            void PrintThreads();
            bool Load(Process* proc);
            bool Unload(Process* proc);
            bool Kill(Process* proc);
            bool Kill(char* name);
            bool Kill(int index);

        public:
            bool SendMessage(char* name, ProcessMessage msg);

        public:
            void Schedule();
            Process* GetNextProcess(bool inc);
            Threading::Thread* GetNextThread(Process* proc, bool inc);

        private:
            int GetFreeIndex();
    };
}