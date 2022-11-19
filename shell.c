// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


void takeCommand(char* command);    // will this even be needed?

void main()
{
    while (1)
    {
        char commandInput[80];
        char fileBuffer[13312];
        int bufferIndex;
        int sectorsRead;

        char fileName[6];
        int commandIndex;

        char directory[512];
        int directoryIndex;

        char* testMessage;

        // Initializing variables for repeated use of type commands
        sectorsRead = 0;
        testMessage = "Test message for write sector. ";


        // Testing writeSector
        syscall(6, testMessage, 26, 0);
        //syscall(6, "Another test of writeSector", 25);

        // Clearing the fileBuffer
        for (bufferIndex = 0; bufferIndex < 13312; bufferIndex++)
        {
            fileBuffer[bufferIndex] = '\0';
        }

        // calls handleInterrupt21() case 0: printString()
        syscall(0, "SHELL> ",0,0);
        // calls handleInterrupt21() case 1: readString()
        syscall(1, commandInput,0,0);

        // commandInput[5] through commandInput[10] is the fileName
        for (commandIndex = 0; commandIndex < 6; commandIndex++)
        {
            fileName[commandIndex] = commandInput[commandIndex + 5];
        }

        if (commandInput[0] == 't' &&
            commandInput[1] == 'e' &&
            commandInput[2] == 's' &&
            commandInput[3] == 't')
        {
            syscall(6, " If you can read this, then the test shell command worked.", 27, 0);
        }

        // checking if the user wants to use the type command
        // this if statement is a little scuffed, but it'll work
        if (commandInput[0] == 't' &&
            commandInput[1] == 'y' &&
            commandInput[2] == 'p' &&
            commandInput[3] == 'e' &&
            commandInput[4] == ' ')
        {
            syscall(3, fileName, fileBuffer, &sectorsRead);
            if (sectorsRead > 0) //if 0, file was not found and no sectors were read
            {
                // calls handleInterrupt21() case 0: printString()
                syscall(0, fileBuffer, 0, 0);
            }
            else
            {
                // calls handleInterrupt21() case 0: printString()
                syscall(0, "Error: File not found", 0, 0);
            }

        }

        // checking if the user wants to execute a program
        // Again, it's a bit scuffed here
        else if (commandInput[0] == 'e' &&
                 commandInput[1] == 'x' &&
                 commandInput[2] == 'e' &&
                 commandInput[3] == 'c' &&
                 commandInput[4] == ' ')
        {
            // calls handleInterrupt21() case 4: executeProgram()
            syscall(4, fileName, 0, 0);
        }

        // checking if user wants to list the contents of the directory
        // This is a pretty simple version of dir, later on we can make it print out the sectors/size of the files too
        else if (commandInput[0] == 'd' &&
                 commandInput[1] == 'i' &&
                 commandInput[2] == 'r' &&
                 commandInput[3] == ' ')    // added a space at the end - Jesse 11/16 23:35
        {
            syscall(2, directory, 2);

            for (directoryIndex = 0; directoryIndex < 512; directoryIndex += 32)
            {
                if (directory[directoryIndex] != 0)
                {
                    // syscall(9) just calls printChar as an interrupt, not part of the project but is incredibly helpful here
                    syscall(9, directory[directoryIndex]);
                    syscall(9, directory[directoryIndex + 1]);
                    syscall(9, directory[directoryIndex + 2]);
                    syscall(9, directory[directoryIndex + 3]);
                    syscall(9, directory[directoryIndex + 4]);
                    syscall(9, directory[directoryIndex + 5]);
                    syscall(9, '\r');
                    syscall(9, '\n');
                }
            }
        }

        // delete file command
        else if (commandInput[0] == 'd' &&
                 commandInput[1] == 'e' &&
                 commandInput[2] == 'l' &&
                 commandInput[3] == ' ')
        {
            // calls interrupt 10 deleteFile(char* fileName)
            syscall(10, fileName);
        }


        else
        {
            // calls handleInterrupt21() case 0: printString()
            syscall(0, "Error: not a valid command.");
        }

        //Choose command to run

        // calls handleInterrupt21() case 0: printString()
        syscall(0, "\r\n");
    }
}
