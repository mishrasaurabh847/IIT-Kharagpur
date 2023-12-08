# Shell Project Readme

This readme provides an overview of a multi-part shell project that utilizes various system calls, libraries, and threading concepts. The project is divided into several parts, each building upon the previous one. Additionally, for a more detailed explanation, you can refer to "Computing Laboratory I (CS69011) Assignment 7.pdf."

The C file includes all the parts mentioned below. 

## Part A: Basic Shell with System Calls - 1

In this part, a basic shell is implemented. The parent process provides a shell prompt (`shell>`) to the user and waits for a command. It uses fork and exec system calls to execute various commands. The shell supports commands such as `pwd`, `cd`, `mkdir`, `ls`, `exit`, and `help`. It also handles background execution using the `&` operator.

## Part B: I/O Redirection with System Calls - 2

Building on Part A, this part extends the shell to support I/O redirection. It enables piping one command into another using the `|` operator. System calls like `dup` and `dup2` are used to achieve I/O redirection.

## Part C: Readline Library Integration

The shell is further extended to incorporate readline library functionalities. It now supports multi-line commands using the backslash (`\`) character, command history using the up arrow, and command editing by pressing Ctrl+A to move the cursor to the start of the line.

## Part D: Ncurses Library Integration

The shell is enhanced with the ncurses library, introducing a 'vi' command for a text editor. Users can perform various operations such as navigating with arrow keys, deleting characters, inserting characters, saving the file (Ctrl+S), and exiting the editor (Ctrl+X). After exiting, the shell prompt displays the number of lines, words, and characters written/modified.

## Part F: Threading Concept Integration

The final part involves threading concepts to perform vector operations. The pthread library is used to create threads and distribute jobs for commands like `addvec`, `subvec`, and `dotprod`. These commands perform vector addition, subtraction, and dot product, respectively, with an option to specify the number of threads.

## Reference

For a more in-depth explanation and details about the project, please refer to "Computing Laboratory I (CS69011) Assignment 7.pdf."

Feel free to explore each part of the project and experiment with the shell's capabilities!
