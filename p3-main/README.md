Riteshwar Singh Brar's 
## Unix Shell

In this project, you’ll build a simple Unix shell. The shell is the heart of the command-line interface, and thus is central to the Unix/C programming environment. Mastering use of the shell is necessary to become proficient in this world; knowing how the shell itself is built is the focus of this project.

There are three specific objectives to this assignment:

* To further familiarize yourself with the Linux programming environment.
* To learn how processes are created, destroyed, and managed.
* To gain exposure to the necessary functionality in shells.

## Overview

In this assignment, you will implement a *command line interpreter (CLI)* or, as it is more commonly known, a *shell*. The shell should operate in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then prompts for more user input when it has finished.

The shells you implement will be similar to, but simpler than, the one you run every day in Unix. If you don't know what shell you are running, it’s probably `bash` or `zsh`. One thing you should do on your own time is to learn more about your shell, by reading the man pages or other online materials.

## Program Specifications

### Basic Shell: `wsh`

Your basic shell, called `wsh` (short for Wisconsin Shell, naturally), is basically an interactive loop: it repeatedly prints a prompt `wsh> ` (note the space after the greater-than sign), parses the input, executes the command specified on that line of input, and waits for the command to finish. This is repeated until the user types `exit`. The name of your final executable should be `wsh`.

The shell can be invoked with either no arguments or a single argument; anything else is an error. Here is the no-argument way:

```sh
prompt> ./wsh
wsh> 
```

At this point, `wsh` is running, and ready to accept commands. Type away!

The mode above is called *interactive* mode, and allows the user to type commands directly. The shell also supports a *batch* mode, which instead reads input from a batch file and executes commands from therein. Here is how you run the shell with a batch file named `script.wsh`:

```sh
prompt> ./wsh script.wsh
```

One difference between batch and interactive modes: in interactive mode, a prompt is printed (`wsh> `). In batch mode, no prompt should be printed.

You should structure your shell such that it creates a process for each new command (the exception are `built-in` commands, discussed below). Your basic shell should be able to parse a command and run the program corresponding to the command. For example, if the user types `ls -la /tmp`, your shell should run the program `/bin/ls` with the given arguments `-la` and `/tmp` (how does the shell know to run `/bin/ls`? It’s something called the shell **path**; more on this below).

## Structure

### Basic Shell

The shell is very simple (conceptually): it runs in a while loop, repeatedly asking for input to tell it what command to execute. It then executes that command. The loop continues indefinitely, until the user types the built-in command `exit`, at which point it exits. That’s it!

For reading lines of input, you should use `getline()`. This allows you to obtain arbitrarily long input lines with ease. Generally, the shell will be run in *interactive mode*, where the user types a command (one at a time) and the shell acts on it. However, your shell will also support *batch mode*, in which the shell is given an input file of commands; in this case, the shell should not read user input (from `stdin`) but rather from this file to get the commands to execute.

In either mode, if you hit the end-of-file marker (EOF), you should call `exit(0)` and exit gracefully. EOF can be generated by pressing `Ctrl-D`.

To parse the input line into constituent pieces, you might want to use `strsep()`. Read the man page (carefully) for more details.

To execute commands, look into `fork()`, `exec()`, and `wait()/waitpid()`. See the man pages for these functions, and also read the relevant book chapter for a brief overview.

You will note that there are a variety of commands in the `exec` family; for this project, you must use `execvp`. You should **not** use the `system()` library function call to run a command. Remember that if `execvp()` is successful, it will not return; if it does return, there was an error (e.g., the command does not exist). The most challenging part is getting the arguments correctly specified.

### Paths

In our example above, the user typed `ls` but the shell knew to execute the program `/bin/ls`. How does your shell know this?

It turns out that the user must specify a **path** variable to describe the set of directories to search for executables; the set of directories that comprise the path are sometimes called the search path of the shell. The path variable contains the list of all directories to search, in order, when the user types a command.

**Important:** Note that the shell itself does not implement `ls` or other commands (except built-ins). All it does is find those executables in one of the directories specified by path and create a new process to run them.

To check if a particular file exists in a directory and is executable, consider the `access()` system call. For example, when the user types `ls`, and path is set to include both `/usr/bin` and `/bin` (assuming empty path list at first, `/bin` is added, then `/usr/bin` is added), try `access("/usr/bin/ls", X_OK)`. If that fails, try `/bin/ls`. If that fails too, it is an error.

Your initial shell path should contain one directory: `/bin`

Note: Most shells allow you to specify a binary specifically without using a search path, using either **absolute paths** or **relative paths**. For example, a user could type the absolute path `/bin/ls` and execute the `ls` binary without a search path being needed. A user could also specify a relative path which starts with the current working directory and specifies the executable directly, e.g., `./main`. In this project, you do not have to worry about these features.

### Built-in Commands

Whenever your shell accepts a command, it should check whether the command is a **built-in command** or not. If it is, it should not be executed like other programs. Instead, your shell will invoke your implementation of the built-in command. For example, to implement the `exit` built-in command, you simply call `exit(0);` in your wsh source code, which then will exit the shell.

In this project, you should implement `exit`, `cd`, `jobs`, `fg` and `bg` as built-in commands.

* `exit`: When the user types exit, your shell should simply call the `exit` system call with 0 as a parameter. It is an error to pass any arguments to `exit`.

* `cd`: `cd` always take one argument (0 or >1 args should be signaled as an error). To change directories, use the `chdir()` system call with the argument supplied by the user; if `chdir` fails, that is also an error.

* `jobs`: `jobs` command prints jobs in the background in the following format `<id>: <program name> <arg1> <arg2> … <argN> [&]`. `<id>` is an index number for the command that was run in the background. If the command was initiated as a background job (as explained below), an ampersand (`&`) is appended after `<argN>`. In cases where the command line includes a pipe (as detailed below), the format should be `<id>: <program 1 name> <arg1> … <argN> | <program 2 name> <arg1> … <argN> | … [&]`. 

* `fg` and `bg`: `fg` and `bg` commands are used for moving processes into foreground/background. This operation changes association with the controlling terminal. These commands can be run with 0 or 1 argument. If the argument is specified, it is the `<id>` of the job to be moved to foreground/background. If there is no argument, the largest job `<id>` is used.  You can read about these commands in the Linux manual (e.g. `man fg`).  You will also want to read about getting and setting the terminal foreground process in the `man tcgetpgrp` and `man tcsetpgrp`.

* For each new job, the job `<id>` for it should be the smallest positive integer not in use. For instance, if `1`, `2` and `4` are used for job `<id>`s, `<id>` for the next job is `3`. 

### Controlling Terminal

`Ctrl-C` sends a `SIGINT` and `Ctrl-Z` sends a `SIGTSTP` signal. The terminal is a controlling terminal for those processes receiving signals. When a process runs in the background, it should not receive these signals. The signals can also be sent with `kill` or `pkill`.

Note that with terminal job control support, you need to properly set foreground process group always, when the foreground process is changed. Please read `man tcsetpgrp` for more information.

### Background Job

Your shell will also allow the user to launch background commands. This is accomplished with the ampersand operator as follows:

```sh
wsh> <cmd> <args…> &
```

In this case, instead of running `cmd` and then waiting for it to finish, your shell should run `cmd` in the background and allow the user to run another command.

### Pipes

Pipe is one of the basic UNIX concepts and `wsh` supports it. It allows composition of multiple simple programs together to create a command with a complex behavior. This is so powerful that it is basically ubiquitous in shell scripts because you can express very complex behavior with very few lines of code. Pipe, denoted as `|`, redirects standard output of the program on the left side to the input of the program on the right side.

```sh
<program1> <arglist1> | <program2> <arglist2> | ... | <programN> <arglistN> [&]
```

The example below shows a command, which compresses a file `f.txt`, decompress it and prints the last 10 lines of the decompressed file.

```sh
cat f.txt | gzip -c | gunzip -c | tail -n 10
```

The processes in this pipe stream should all have the same *process group ID*.  You can read more about processes and process group IDs [here](https://www.win.tue.nl/~aeb/linux/lk/lk-10.html#:~:text=By%20convention%2C%20the%20process%20group,equivalently%2C%20getpgid(0)%20.).

*We don't have to make pipes work with builtins. It can be safely assumed that if a command line contains pipes, it only contains user programs and not shell builtins.*

### Miscellaneous Hints

Remember to get the basic functionality of your shell working before worrying about all of the error conditions and end cases. For example, first get a single command running (probably first a command with no arguments, such as `ls`).

Next, add built-in commands. Then, try working on pipes and terminal control. Each of these requires a little more effort on parsing, but each should not be too hard to implement. It is recommended that you separate the process of parsing and execution - parse first, look for syntax errors (if any), and then finally execute the commands.

We simplify the parsing by having a single space ` ` as the only allowed delimiter. It means that any token on the command line will be delimited by a single space ` ` in our tests.

Check the return codes of all system calls from the very beginning of your work. This will often catch errors in how you are invoking these new system calls. It’s also just good programming sense.

Beat up your own code! You are the best (and in this case, the only) tester of this code. Throw lots of different inputs at it and make sure the shell behaves well. Good code comes through testing; you must run many different tests to make sure things work as desired. Don't be gentle – other users certainly won't be.

Finally, keep versions of your code. More advanced programmers will use a source control system such as `git`. Minimally, when you get a piece of functionality working, make a copy of your `.c` file (perhaps a subdirectory with a version number, such as `v1`, `v2`, etc.). By keeping older, working versions around, you can comfortably work on adding new functionality, safe in the knowledge you can always go back to an older, working version if need be.

Error conditions should result in `wsh` terminating with an exit code of -1.  Non-error conditions should result in an exit code of 0.
