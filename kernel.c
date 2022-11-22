// COMP350-001
// ProjectD
// 11/11/22
// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


// Test comment, please ignore
// Another test

void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void terminate();
void handleInterrupt21(int,int,int,int);
void writeSector(char*, int);
void deleteFile(char*);


void main()
{
    // Setting up for readString
    char line[80];

    // Setting up readSector
    char sectorBuffer[512];

    // Setting up readFile
    char fileBuffer [13312];
    char* fileName = "messag";
    int sectorsRead;

    // These two functions are for steps 4-6
    makeInterrupt21();

    handleInterrupt21(4, "shell", 0, 0);



    // The following code block was for testing functions for steps 1-3

    // Testing readFile as a function
    //readFile(fileName, fileBuffer, &sectorsRead); // Just a test of the function

    // Testing readFIle as an interrupt
//    handleInterrupt21(3, fileName, fileBuffer, &sectorsRead);
//
//    if (sectorsRead > 0)
//    {
//        printString(fileBuffer);
//    }
//    else
//    {
//        printString("Error: File not found\n\r");
//    }

    // Testing executeProgram as an interrupt
    //handleInterrupt21(4, "tstp", 0, 0);

    // Testing terminate by executing tstpr2
    //handleInterrupt21(4, "tstpr2", 0, 0);



    while(1);
}

void printChar(char c)
{
    interrupt(0x10, 0xe*256+c,0,0,0);
}

void printString(char* chars)
{
    int increment = 0;
    while(chars[increment] != 0x0)
    {
        interrupt(0x10, 0xe*256+chars[increment],0,0,0);
        increment++;
    }
}

void readString(char* chars)
{
    int currIndex = 0;
    chars[currIndex] = interrupt(0x16, 0,0,0,0);
    printChar(chars[currIndex]);

    while(chars[currIndex] != 0xd && currIndex < 80)
    {
        char input = interrupt(0x16, 0,0,0,0);
        if(input == 0x8)
        {
            if(currIndex >= 0)
            {
                currIndex--;
                printChar(0x8);
                printChar(' ');
                printChar(0x8);
            }
        }
        else
        {
            currIndex ++;
            chars[currIndex] = input;
            printChar(input);
        }
    }
    printChar(0xa);

    chars[currIndex+1] = 0xa;
    chars[currIndex+2] = 0x0;
}

void readSector(char* buffer, int sector)
{
    interrupt(0x13, 2*256+1, buffer, sector+1, 0x80);
}

void writeSector(char* buffer, int sector)
{
    interrupt(0x13, 3*256+1, buffer, sector+1, 0x80);
}

//void deleteFile(char* fileName)
//{
//    char dir[512];
//    char map[512];
//    int correctCharIndex;
//    int correctChars;
//    int dirEntry;
//    int fileMapLoc[26]; // 26 possible map locations (dir entry size:32, 32 - 6(for file name) = 26
//    int dirIndex;
//    int mapIndex;
//
//    readSector(dir, 2);
//    readSector(map, 3);
//
//    for (dirEntry = 0; dirEntry < 512; dirEntry += 32)
//    {
//        correctChars = 0;
//
//        for (correctCharIndex = 0; correctCharIndex < 6; correctCharIndex++)
//        {
//            if (fileName[correctCharIndex] == dir[dirEntry + correctCharIndex] && dir[dirEntry] != 0)
//            {
//                correctChars++;
//            }
//
//            else if (correctChars == 6)
//            {
//                printChar('F');
//                printChar('i');
//                printChar('l');
//                printChar('e');
//                printChar(' ');
//                printChar('f');
//                printChar('o');
//                printChar('u');
//                printChar('n');
//                printChar('d');
//                printChar('\r');
//                printChar('\n');
//
//                dir[dirEntry + 1] = '\0';   // sets first byte of file name to 0
//
//                // steps through sector locations of dir entry, changing them to 0
//                // breaks when it finds a 0
//                for (dirIndex = 6; dirIndex < 32; ++dirIndex)
//                {
//                    if(dir[dirEntry + 6] != 0)
//                    {
//                        fileMapLoc[dirIndex - 6] = dir[dirEntry + 6];
//                        dir[dirEntry + 6] = '\0';
//                    } else break;
//                }
//
//                // steps through map, changing marked locations to 0
//                // breaks when it finds a 0 in the file's map location (fileMapLoc)
//                for (mapIndex = 0; mapIndex < 26; ++mapIndex)
//                {
//                    if (fileMapLoc[mapIndex] != 0) map[fileMapLoc[mapIndex]] = '\0';
//                    else break;
//                }
//
//                terminate();
//            }
//        }
//    }
//
//    // writes dir and map char arrays back into their appropriate sectors
//    writeSector(dir, 2);
//    writeSector(map, 3);
//}

void readFile(char* fileName, char* buffer, int* sectorsRead)
{
    int correctCharIndex; // Index used for comparing how many characters in fileName match with directory[fileEntry]
    int correctChars; // The number of matching characters when comparing fileName and directory[fileEntry]

    int sectorIndex; // Index used for reading what sectors a file is stored on

    // The following variables are used for padding out characters in fileName
    int i;
    int pad;

    int fileEntry; // fileEntry acts as an index for accessing the data inside the directory
    char directory[512];

    readSector(directory,2); // Directory is at sector 2

    // Padding out fileName with 0's
    pad = 0; // Pad is false
    for (i = 0; i < 6; ++i)
    {
        if (fileName[i] == '\r' || fileName[i] == '\n')
        {
            pad = 1;
        }
        if (pad == 1)
            fileName[i] = '\0';
    }


    for (fileEntry = 0; fileEntry < 512; fileEntry += 32)
    {
        correctChars = 0;

        // fileName has to match identically with the first 6 entries of file stored in the directory
        for (correctCharIndex = 0; correctCharIndex < 6; correctCharIndex++)
        {
            // The second clause is important here, since if the file in directory starts with a 0, that means it's been marked for delete
            if (fileName[correctCharIndex] == directory[fileEntry + correctCharIndex] && directory[fileEntry] != 0)
            {
                correctChars++;
            }

            // Seeing if all 6 chars in fileName match with what is in the directory
            if (correctChars == 6)
            {
                // Looks messy but this is a better alternative for testing purposes.
                // This way we can see this print out when testing the shell
                printChar('F');
                printChar('i');
                printChar('l');
                printChar('e');
                printChar(' ');
                printChar('f');
                printChar('o');
                printChar('u');
                printChar('n');
                printChar('d');
                printChar('\r');
                printChar('\n');


                // Now that we've found the file, we need to find what sectors the file is on
                // Starting the index at 6 since the sectors that the file are stored on also start at index 6
                for (sectorIndex = 6; sectorIndex < 32; sectorIndex++)
                {
                    if (directory[fileEntry + sectorIndex] == 0x0)
                    {
                        // This was a test for readFile in kernel.c but this won't be helpful for shell
                        printString("All sectors found. \n\r");
                        break;
                    }
                    else
                    {
                        readSector(buffer, directory[fileEntry + sectorIndex]);
                        buffer += 512;
                        *sectorsRead = *sectorsRead + 1;
                    }
                }
            }
        }
    }
}


void deleteFile(char* fileName)
{
    char dir[512];
    char map[512];
    int correctCharIndex;
    int correctChars;
    int dirEntry;
    int fileMapLoc[26]; // 26 possible map locations (dir entry size:32, 32 - 6(for file name) = 26
    int dirIndex;
    int mapIndex;


    printChar('T');
    printChar('r');
    printChar('y');
    printChar('i');
    printChar('n');
    printChar('g');
    printChar(' ');
    printChar('t');
    printChar('o');
    printChar(' ');
    printChar('d');
    printChar('e');
    printChar('l');
    printChar('e');
    printChar('t');
    printChar('e');
    printChar(' ');
    printChar('f');
    printChar('i');
    printChar('l');
    printChar('e');
    printChar('\r');
    printChar('\n');


    readSector(dir, 2); // Directory is at sector 2
    readSector(map, 1); // Map is at sector 1

    for (dirEntry = 0; dirEntry < 512; dirEntry += 32)
    {
        correctChars = 0;

        for (correctCharIndex = 0; correctCharIndex < 6; correctCharIndex++)
        {
            if (fileName[correctCharIndex] == dir[dirEntry + correctCharIndex] && dir[dirEntry] != 0)
            {
                correctChars++;
            }

            if (correctChars == 6)
            {
                printChar('F');
                printChar('i');
                printChar('l');
                printChar('e');
                printChar(' ');
                printChar('f');
                printChar('o');
                printChar('u');
                printChar('n');
                printChar('d');
                printChar('\r');
                printChar('\n');

                dir[dirEntry] = '\0';   // sets first byte of file name to 0

                // steps through sector locations of dir entry, changing them to 0
                // breaks when it finds a 0
                for (dirIndex = 6; dirIndex < 32; ++dirIndex)
                {
                    if (dir[dirEntry + dirIndex] == '\0')
                    {
                        printChar('B');
                        printChar('r');
                        printChar('e');
                        printChar('a');
                        printChar('k');
                        printChar('i');
                        printChar('n');
                        printChar('g');
                        printChar('\r');
                        printChar('\n');

                        break;
                    }
                    else
                    {
                        fileMapLoc[dirIndex - 6] = dir[dirEntry + dirIndex];
                        dir[dirEntry + dirIndex] = '\0';

                    }

                }

                // steps through map, changing marked locations to 0
                // breaks when it finds a 0 in the file's map location (fileMapLoc)
                for (mapIndex = 0; mapIndex < 26; ++mapIndex)
                {
                    if (fileMapLoc[mapIndex] != 0) map[fileMapLoc[mapIndex]] = '\0';
                    else break;
                }

                terminate();
            }
        }
    }

    // writes dir and map char arrays back into their appropriate sectors
    writeSector(dir, 2);
    writeSector(map, 1);
}


void executeProgram(char* name)
{
    int index = 0;
    int numSectorsRead = 0;
    char buffer[13312];

    printChar('e');
    printChar('x');
    printChar('e');
    printChar('c');
    printChar('\r');
    printChar('\n');

    readFile(name, buffer, &numSectorsRead);

    if (numSectorsRead > 0)
    {

        for (index = 0; index < 13312; index++) {
            putInMemory(0x2000, index, buffer[index]);
        }
        launchProgram(0x2000);
    }
    else
    {
        printChar('B');
        printChar('a');
        printChar('d');
        printChar(' ');
        printChar('a');
        printChar('r');
        printChar('g');
        printChar('u');
        printChar('m');
        printChar('e');
        printChar('n');
        printChar('t');
        printChar('\r');
        printChar('\n');

        terminate();
    }
}

void terminate()
{
    char shellName[6];

    shellName[0] = 's';
    shellName[1] = 'h';
    shellName[2] = 'e';
    shellName[3] = 'l';
    shellName[4] = 'l';
    shellName[5] = '\0';

    executeProgram(shellName);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
    switch(ax)
    {
        case 0:
            printString(bx);
            break;
        case 1:
            readString(bx);
            break;
        case 2:
            readSector(bx, cx);
            break;
        case 3:
            readFile(bx, cx, dx);
            break;
        case 4:
            executeProgram(bx);
            break;
        case 5:
            terminate();
            break;
        case 6:
            writeSector(bx, cx);
            break;
        case 9:
            printChar(bx);
            break;
        case 10:
            deleteFile(bx);
            break;
        default:
            printString("No interrupt function correlated with AX number");


    }


}
