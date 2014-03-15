#include <stdio.h>
#include <windows.h>

int main(int argc, char **argv[])
{
    HANDLE FileHandle, ProcHeap;
    DWORD FileSize, Counter;
    BYTE FileByte;
    LPVOID pFileHeap, pFileOutHeap;
    LPDWORD lpNumberOfBytesRead;


    if(argc != 3)
    {
        printf("Syntax: ASCIItoRUS <Input file name> <Output txt file name>\n");
        return 0;
    }

    FileHandle = CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    FileSize = GetFileSize(FileHandle,NULL);

    ProcHeap = GetProcessHeap();
    pFileHeap = HeapAlloc(ProcHeap,HEAP_ZERO_MEMORY,(FileSize));

    ReadFile(FileHandle,pFileHeap,FileSize,&lpNumberOfBytesRead,NULL);

    pFileOutHeap = HeapAlloc(ProcHeap,HEAP_ZERO_MEMORY,(FileSize*2+2));

    //Make the txt format UNICODE by setting a magic number
    *(WORD *)pFileOutHeap = 0xFEFF;

    for(Counter=0;Counter != FileSize;Counter++)
    {
        //Test if character is using extended ASCII
        if(*(unsigned char *)(pFileHeap+Counter) > 0x7F)
        {
            //Convert extended ASCII to Russian UNICODE character
            *(char *)(pFileHeap+Counter) = (*(char *)(pFileHeap+Counter))-0xB0;
            *(char *)(pFileOutHeap+3+Counter*2) = 0x04;
        }
        else
        {
            //Convert regular ASCII to UNICODE
            *(char *)(pFileOutHeap+3+Counter*2) = 0x00;
        }
        *(char *)(pFileOutHeap+2+Counter*2) = *(char *)(pFileHeap+Counter);
    }
    //EXAMPLE conversion: C8 -> 18 04 (first char - 0xB0) (0x04)


    FileHandle = CreateFile(argv[2],GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

    WriteFile(FileHandle,pFileOutHeap,(FileSize*2+2),&lpNumberOfBytesRead,NULL);


    return 0;
}
