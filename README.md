IMCSH is a custom Linux shell implemented in C as part of an Operating Systems course project. The shell provides basic command execution, background process handling, output redirection, and built-in commands.

## Features

- **Command Execution**: Execute commands using the `exec` keyword.
  - Example: `exec ls -l`
- **Background Execution**: Run commands in the background by appending `&`.

  - Example: `exec sleep 10 &`

- **Output Redirection**: Redirect command output to a file using `>`.

  - Example: `exec ls -l > output.txt`

- **Built-in Commands**:
  - `globalusage`: Displays the version and author information of the shell.
    - Example: `globalusage`
  - `quit`: Exits the shell. If background processes are running, the user is prompted for confirmation.

## How to Compile and Run

1.  **Clone the repository** (if applicable) or ensure you have the source code file (`imcsh.c`) and a `Makefile`.

2.  **Compile the code** using the provided `Makefile`:

    ```bash
    make
    ```

    This will generate an executable named `imcsh`.

3.  **Run the shell**:
    ```bash
    ./imcsh
    ```
4.  **Use the shell**:

        Enter commands as described in the Features section.

    To exit the shell, type quit.

    ```bash
    quit

    ```

    ```bash
    user@host> exec ls -l
    # Lists files in the current directory

    user@host> exec sleep 5 &
    # Runs 'sleep 5' in the background

    user@host> globalusage
    # Output: IMCSH Version 1.1 created by George and Mihai

    user@host> exec ls -l > output.txt
    # Redirects the output of 'ls -l' to 'output.txt'

    user@host> quit
    # Exits the shell (prompts for confirmation if background processes are running)
    ```

## Project Structure

```imcsh/
├── imcsh.c        # Main source code
├── Makefile       # Compilation script
├── README.md      # Project documentation
├── project-os.pdf # Assignment details
```

## Technical Details

- **Command Parsing**: The shell uses `strtok` to parse user input into tokens.
- **Process Management**: The shell uses `fork()` and `execvp()` to execute commands. Background processes are managed using an array of PIDs.
- **Output Redirection**: The shell uses `dup2()` to redirect standard output to a file.
- **Signal Handling**: Background processes are terminated using `SIGKILL` when the shell exits.

## Project Structure

- `imcsh.c`: The main source code file containing the shell implementation.
- `Makefile`: A simple Makefile to compile the project.
- `README.md`: This file, providing an overview of the project.

## Assessment Criteria

The project is assessed based on the following criteria:

- **Working implementation of `exec`**: 25 points.
- **Working implementation of `&` (background execution)**: 10 points.
- **Working implementation of `globalusage`**: 2 points.
- **Working implementation of `>` (output redirection)**: 5 points.
- **Working implementation of `quit`**: 3 points.
- **Clean code and documentation**: 5 points.

## Authors

- **Gheorghe Tutunaru**
- **Mihail Lupasco**
