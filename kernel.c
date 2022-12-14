// COMP350-001
// ProjectD
// 11/11/22
// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


// Test coment, please ignore
// Another test

void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*, int*);
void writeFile(char*, char*, int);
void deleteFile(char*);
void createTextFile(char*);
void executeProgram(char*);
void terminate();
void handleInterrupt21(int,int,int,int);

void main()
{
    // TJ writeFile() test
    char* buffer = "TJ test text";
    char* filename = "tjtest";
    int numberOfSectors = 1;
    writeFile(buffer, filename, numberOfSectors);
    deleteFile(filename);

    // These two functions are for steps 4-6
    makeInterrupt21();
    
    handleInterrupt21(4, "shell", 0, 0);

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

    while(chars[currIndex] != 0xd && currIndex < 64)
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

void deleteFile(char* filename)
{
    char directory[512];
    char map[512];

    int i;
    int dirEntry;

    readSector(map,1);
    readSector(directory,2);
    
    for(dirEntry = 3; dirEntry < 16; dirEntry++)
    {
        if(directory[dirEntry*32] == filename[0] &&
            directory[(dirEntry*32)+1] == filename[1] &&
            directory[(dirEntry*32)+2] == filename[2] &&
            directory[(dirEntry*32)+3] == filename[3] &&
            directory[(dirEntry*32)+4] == filename[4] &&
            directory[(dirEntry*32)+5] == filename[5])
        {
            break;
        }
    }

    directory[dirEntry*32] = '\0';

    for(i = 6; i < 32; i++)
    {
        if(directory[(dirEntry*32)+i] == '\0')
        {
            break;
        }
        else
        {
            map[(int)directory[(dirEntry*32+1)+i]] = 0x00;
            directory[(dirEntry*32+1)+i] = '\0';
        }
    }

    //Write the directory and map back to the disk
    writeSector(map, 1);
    writeSector(directory, 2);
}

void writeFile(char* buffer, char* filename, int numberOfSectors)
{
    char directory[512];
    char map[512];
    
    int dirEntry;
    int sectorNumber;
    int i;

    readSector(map,1);
    readSector(directory,2);

    //Find an empty directory entry, store the entry number in dirEntry
    for(dirEntry = 3; dirEntry < 16; dirEntry++)
    {
        if(directory[dirEntry*32] == '\0')
        {
            break;
        }
    }

    //Set the file name in the directory
    //Need to account for file names less than length 6. Will come back to it.
    for(i = 0; i < 6; i++)
    {
        directory[(dirEntry*32)+i] = filename[i];
    }

    //Run this loop once for each sector the file will take up. Each time it will find an empty
    //  sector on the map, insert it's sector number into the directory, write 512 bytes of the file
    //  to that sector, and then mark the sector as used on the map.
    for(i = 0; i < numberOfSectors; i++)
    {
        //Find an empty sector on the map
        for(sectorNumber = 3; sectorNumber < 512; sectorNumber++)
        {
            if(map[sectorNumber] == 0x00)
            {
                break;
            }    
        }
        
        //Insert that sector number into the directory
        directory[(dirEntry*32)+6+i] = (char)sectorNumber;
        
        //Write 512 bits of the file to that sector
        writeSector(buffer, sectorNumber);
        
        //Mark the space as occupied on the map
        map[sectorNumber] = 0xFF;
        
        //Write the directory and map back to the disk
        writeSector(map, 1);
        writeSector(directory, 2);
    }
    
}

void createTextFile(char* filename)
{
    char lineBuffer[64];
    char fileBuffer[512];
    int numSectors = 0;
    int fileSize = 0;
    int i;
    int j;

    while(1)
    {
        readString(lineBuffer);
        if(lineBuffer[0] == '\r')
        {
            break;        
        }
        else
        {
            for(j = 0; j < 64; j++)
            {
                fileBuffer[fileSize] = lineBuffer[j];
                fileSize++;
                if(lineBuffer[j] == '\n')
                {
                    break;
                }

            }
        }
    }
    
    while(fileSize>0)
    {
        fileSize = fileSize - 512;
        numSectors += 1;
    }

    writeFile(fileBuffer, filename, numSectors);
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
        case 7:
            deleteFile(bx);
            break;
        case 8:
            writeFile(bx, cx, dx);
            break;
        case 9:
            printChar(bx);
            break;
        case 10:
            createTextFile(bx);
            break;
        default:
            printString("No interrupt function correlated with AX number");

    }
}
