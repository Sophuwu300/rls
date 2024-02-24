
# rls

## Description
This command allows most of the features of ls but with a rainbow color output. 

## Features
- Rainbow color output.
- `rls -al` shows the same information as `ls -Al`.
- It can also count the number of files in a directory.

## Installation
Just download the .deb file and run `dpkg --install` to install it.

### From Source
```
git clone https://github.com/Sophuwu300/rls.git
cd rls
make
sudo make install
```

## Usage
`rls [OPTIONS] [PATHS]`
<br>
**Options:**
<br>`-h` `--help` display this help message
<br>`-c` `--nocolor` disable color output
<br>`-l` `--list` more info on files
<br>`-a` `--all` list dot files
<br>`-n` `--number` count files

### Examples

List all files in the current directory with more information.

```
rls -al
```
List all files in /home and /etc.

```
rls -a /home /etc
```