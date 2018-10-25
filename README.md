# MTND
## College Project: The purpose of the project is to develop a NON DETERMINISTIC Touring Machine Simulator that needs to run in as little time as possible and that needs to use as little memory as possible.
### Open "main.c" to view
<br/>
My program reads the first part of the input file once (which is provided on the standard input) and creates a linked list to store all the different transitions, while keeping track of other parameters such as the biggest state, the amount of different chars that appear in the transitions and a few more things.
It then creates a matrix structure that holds i x j cells, where i is the amount of different states and j is the amount of different chars. This structure is helpful because knowing that a certain process is on state A and is reading the char B, the list of all the possible transitions can be found at <A; B><br/>
For the executions, it creates an initial process called 'init' which is provided with different variables to keep track of the current status of the machine. It's also able to use an initial tape that holds 1024 chars filled with the input string and blank _ chars. <br/>
Every time it encounters a non deterministic move, a new process is forked from the parent.<br/>
The forking process copies every variable from the parent but sets the tape on 'shared' mode.<br/>
When a process whose tape is shared tries to write on the tape, the whole tape is copied and the number of sharers is decremented.

Each process is able to execute only one move at a time before passing the token to the next process.
A process is killed if it finds itself on a termial state (returns 1) o if it doesn't have any outward transitions from the current state (returns 0 for that branch).
If a process has exceded the amount of possible moves it is also killed (returns U for that branch).


The input file looks like this:<br/>
tr<br/>
0 a _ R 1<br/>
0 a _ R 5<br/>
0 a _ R 9<br/>
1 b _ R 2<br/>
2 c _ R 3<br/>
3 _ x R 4<br/>
3 a _ R 1<br/>
5 b _ R 6<br/>
6 b _ R 7<br/>
7 _ x R 8<br/>
7 a _ R 5<br/>
9 c _ R 10<br/>
10 c _ R 11<br/>
11 _ x R 12<br/>
11 a _ R 9<br/>
acc<br/>
4<br/>
8<br/>
12<br/>
max<br/>
25<br/>
run<br/>
abcabcabc<br/>
