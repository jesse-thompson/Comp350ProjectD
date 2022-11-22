// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


void takeCommand(char* command);

void main()
{
    while (1)
    {
        char commandInput[80];
        char fileBuffer[13312];
        int bufferIndex;
        int sectorsRead;
	int sectorsReadTwo;

        char fileName[6];
	char fileNameTwo[6];
        int commandIndex;
	int currentIndex;

        char directory[512];
        int directoryIndex;

        // Initializing variables for repeated use of type commands
        sectorsRead = 0;

        // Testing writeSector
       

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


        // checking if the user wants to use the type command
        // this if statement is a little scuffed, but it'll work
        if (commandInput[0] == 't' &&
            commandInput[1] == 'y' &&
            commandInput[2] == 'p' &&
            commandInput[3] == 'e' &&
            commandInput[4] == ' ')
        {
            syscall(3, fileName, fileBuffer, &sectorsRead);
	    syscall(0,fileName,0,0);
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
                 commandInput[2] == 'r')
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

	// Checking if copy command is being called
        else if (commandInput[0] == 'c' &&
                 commandInput[1] == 'o' &&
                 commandInput[2] == 'p' &&
                 commandInput[3] == 'y' &&
                 commandInput[4] == ' ')
        {
                // Reset buffer index
                for (bufferIndex = 0; bufferIndex < 13312; bufferIndex++)
                {
                    fileBuffer[bufferIndex] = '\0';
                }

                // Getting name of file to be created
                for (currentIndex = 0; currentIndex < 6; currentIndex++)
                {//copy messag essag2
                        fileNameTwo[currentIndex] = commandInput[currentIndex + 12];
                }
                syscall(0,fileNameTwo,0,0);
                // Reading original file

                syscall(3, fileName, fileBuffer, &sectorsReadTwo);
                syscall(0,fileName,0,0);
                syscall(0,"Printing out buffer",0,0);
                syscall(0,fileBuffer,0,0);
                // If original file name found, write file contents to new file
                if (sectorsReadTwo > 0)
                {
                        syscall(8, fileBuffer, fileNameTwo, sectorsReadTwo);
                        syscall(0, "File found and writing contents", 0, 0);

                }
                else
                {
                        syscall(0, "Error! File not found!", 0, 0);
                }
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
