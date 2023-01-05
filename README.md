# POSIX Shell

Round Robin Shell

## Installation



```bash
g++ RRShell.cpp -o RRShell
./RRShell
```

### C++14 or above should be used. 

### Commands
```
echo $$
```
```
path autocompletion using <tab> key.
```
```
echo HOST

```
```
PS1="=>"
```
```
PS1=DEFAULT
```
```
cd ..
```
```
cd $HOME
```

```
cd ~
```
```
alarm 00:00:05
```
```
cat a.txt | head -4 | tail -3 | grep abc
```
```
cat abc.txt > op.txt
```
```
cat abc.txt | head -4 | tail -3 | sort > op.txt
```
```
cat abc.txt  >> op.txt
```
```
./a.out < input
```
```
alias x=ls
### There should not be any space around the '='
```
```
export apple = mango
export -p
export -n key
### 2. export -p prints all the previous exports, export -n key deletes the exported key.
```
```
pwd
```
```
history
```
```
clear
```
```
open abc.mp4
```
```
sleep 50 &
```
```
showbg # shows processes in background.
```

```
fg <id>
```

```
ls, echo, touch, mkdir, grep, pwd, cd, cat, 
```
```
head, tail, chmod, exit, history, clear, cp
```
