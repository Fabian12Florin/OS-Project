# File Information

Project for Operating System Lab

The primary objective of this project is to introduce fundamental programming concepts, focusing on working with files and interprocess communication using pipes.

In brief, the program takes three arguments:

1. The first argument represents the directory where all the files to be analyzed are stored. The program examines BMP files, link files, directories, and regular files within this specified directory.

2. The second argument denotes the output directory, where statistics about the analyzed files are stored. Additionally, for each BMP file in the input directory, a corresponding grayscale duplicate is created in the output directory.

3. The third argument is dedicated to the script called within main.c (checkLine.sh). This argument is a character.

Student name: Dogaru Fabian-Florin


## How to run the program 🏃‍♂️

Download the zip file for this project and open it in any IDE.

Open a new a terminal inside the IDE and run this command

```bash
gcc -Wall -o fis1 fis1.c -lm
./fis1 Directory OutputDirectory
```
## Don't be hungry!
![Spaghetti photo not loaded :(](ImagesReadMe/Spaghetti.jpeg)
I know this is one of the most spaghetti-code ever created, have fun!
=======
