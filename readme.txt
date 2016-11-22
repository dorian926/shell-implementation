Regarding the ____=NULL, before the free(_____), and not a lot of fclose(______), I was getting a lot of segmentation
errors on classroom when having those set up, yet it worked fine without them. Will be debugging to figure this out.
I apologize if this makes the code seem not neat/ "What is this student doing?"

exercise 1 :  Setting up the baseline. The way I implemented it I separated the known commands and the builtins into two 
separate methods. I feel as this way is overcomplicating it and I plan to go back and rework it. If it is a known command,
it takes the commands directory and the input to a method to parse it. If it is a builtin, it goes to another method, 
which will figure out which one it is and then have its own code to implement it.
exercise 2 : Getting cd to word and change the env was implemented using chdir. I found it easy to implement "cd" or
"cd ~" which retrieve the home path from my linked list implementation. I was unable to get "cd -" working. I have a
multiple methods included which I thought would change the directory. The command works on my personal shell, however I
was not able to get it working on classroom.
exercise 3 : I strcat a prompt together with the current path retrieved from the environment variables list using getcwd.
This command is ran right when the shell starts and anytime the path is changed.
exercise 4 : My appraoch to debug was to create a variable debug status and if argv[1] was "-d" then to turn on debug for
the current run of the program. My first approach used strncmp(argv[1], "-d",2), hwoever after I implemented scripting, a
bug was allowing this to turn on even when not present. As such I modified it to check each character at a time using 
arv[1] and argv[1][1].
exercise 5 : Variable and echo support was implemented with getenv, and setenv. Near program initialization, before being 
prompted I added '$?' to the environment variables and it is set up to always be assigned to the return of a command. When
using getenv, before I made the call I designed the program to erase the '$' so the envroonment variable could be found. 
Changing around some of the variables, then typing printenv, it reflected the changes.
exercise 6 : I will be upfront in saying that my piping was not implemented in this version does not work. I had attempted
piping, however I did not clearly understand what I was doing and did not know how to debug what I was writting, so I 
rolled back to the version before piping was implemented. I will need a lot of help understanding this part.The '<' 
and '>' do work though. using various commands such as 'ls' (including varients), 'cat', 'wc'. My first version included
popen and pclose, however I changed that after not knowing if that would be okay since it runs the pipe for me rather
than implementing it myself. 
exercise 7 : My approach for scripting was to do another check for the argv[1] like debugging. If it saw a script, it 
would set int script to 1 stating that there is one. After the path was made, the program detected if script was 1 and 
if so sent the script to its processing method. My first approach was to try to buffer the file into an char *, however 
once i started testing I realized that the shell files do not end lines with '\n' so I had to scratch this approach. After 
research my next approach was getline. This worked however after it made an output, I noticed a '?' at the end of the file,
and realized that is how it ended lines. Rather than go back to my first approach, I used the length - 1 of the current 
line and set it to '\0'. Using both 'thsh foo.sh' and the './foo.sh' worked. The check for the '#' was simple in the fact
that if it was seen as the first character in the line then the processing block was skipped over.
exercise 8 : Did not make it to job control. Will be stopping by many office hours to discuss it with you all and finish 
implementing my shell.
exercise 9 : Needs to be viewed with a full-window!

Resources used:
- man pages on shell
- googled "man ______" in the cases where I could not get the grasp of how the shell explained it and needed a further
break down
- http://www.cplusplus.com/reference/clibrary/