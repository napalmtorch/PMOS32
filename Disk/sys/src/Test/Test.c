
const char test[] = "Hello world\n";

typedef struct
{
    const char*  Name;
    unsigned int Address;
} __attribute__((packed)) MethodEntry;

void print(const char* str);
void lib_init();

int _start(void* arg)
{
    print("Started test program\n");

    lib_init();

    return 1234;
}

void lib_init()
{
    MethodEntry* table;

    asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xFF), "d"((unsigned int)&table));
    print("Grabbed method table\n");

    for (int i = 0; i < 256; i++)
    {
        if (table[i].Address > 0)
        {
            print("METHOD: ");
            print(table[i].Name);
            print("\n");
        }
    }
}

void print(const char* str)
{
    asm volatile("int $0x80": :"a"(0), "b"((unsigned int)str), "c"(0x01), "d"(0));
}