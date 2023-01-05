#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <algorithm>
#include <cctype>
#include <locale>
#define MOVE_CURSOR_LEFT "\033[999D"
using namespace std;
#define clear_line (cout << "\033[2K")

#define clear_screen (printf("\033[H\033[J"))
#define TURNRESET "\033[0m"
#define MAGENTA "\033[1m\033[31m"
#define BLUE "\033[1m\033[32m"
void setcursor(int, int);
void prompt();
void promptInit();
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)
//  variable declaration part(dont change here)

int lastRow, firstColumn;
struct winsize windowSize;
bool statusFlag=true;

string promptInput;

// env variables
string promptString = "";
string hostname, username, cwd, home;
bool promptStringChanged = false;
int k = 10;

pid_t shellId, childId = -1;
struct termios orig_termios_settings;

string currJob;
string alarmMessage;
int alarmSeconds;
vector<string> historylist;
int no, background_n;
int histIndex;
struct termios term;
vector<string> commandList = {"ls", "echo", "touch", "mkdir", "grep", "pwd", "cd", "cdddd", "cdxyg", "cdfghk", "cat", "head", "tail", "exit", "history", "clear", "cp"};

unordered_map<string, string> aliasList;
unordered_map<string, string> exportMP;
int exitStatus = 0;

string his_sav_File = "history.txt";
string alarm_list = "Alarm.txt";

unordered_map<string, string> exports;

vector<pthread_t> threads;

unordered_map<int, string> bgCommands;

void executeCommand(string pipedCommand);
vector<string> parseCommand2(string command);
void initAlarm();

// sumonto
string trim(string str)
{
    int l = 0, r = str.size() - 1;
    while (l < str.size() && str[l] == ' ')
        l++;
    while (r >= 0 && str[r] == ' ')
        r--;
    return str.substr(l, r - l + 1);
}

// soumadeep
class trie
{
public:
    bool last = false;
    class trie *child[256] = {NULL};
    int dp[112] = {0};
    void res()
    {
        for (int i = 0; i < 212; i++)
            dp[i] = 0;
    }
    void insert(trie *root, string s)
    {
        trie *ptr = root;
        int len = s.size();
        for (int j = 0; j < len; j++)
        {
            int u = int(s[j]);
            if (ptr->child[u] == NULL)
            {
                ptr->child[u] = new trie();
            }
            ptr = ptr->child[u];
        }
        ptr->last = true;
    }
    int spell_check(trie *root, string s)
    {
        trie *ptr = root;
        int len = s.size();
        for (int j = 0; j < len; j++)
        {
            int u = int(s[j]);
            if (ptr->child[u] == NULL)
            {
                return 0;
            }
            ptr = ptr->child[u];
        }
        if (ptr->last == false)
            return 0;
        return 1;
    }

    void traverse(trie *root, string s1, vector<string> &v)
    {
        if (root->last == true)
            v.push_back(s1);

        for (int i = 0; i < 256; i++)
        {
            if (root->child[i] != NULL)
            {
                s1 = s1 + char(i);
                traverse(root->child[i], s1, v);
                s1.pop_back();
            }
        }
    }

    void traverse2(trie *root, int n, string s1, vector<string> &v)
    {

        if (root->last == true)
        {

            if (root->dp[n] < 4)
            {
                v.push_back(s1);
            }
        }
        for (int i = 0; i < 256; i++)
        {
            if (root->child[i] != NULL)
            {
                s1 = s1 + char(i);
                traverse2(root->child[i], n, s1, v);
                s1.pop_back();
            }
        }
    }

    vector<string> autocomplete(trie *root, string s)
    {
        trie *ptr = root;
        int len = s.size();
        vector<string> v;
        for (int j = 0; j < len; j++)
        {
            int u = int(s[j]);
            if (ptr->child[u] == NULL)
            {
                return v;
            }
            ptr = ptr->child[u];
        }
        if (ptr == NULL)
            return v;
        string s1 = "";
        traverse(ptr, s1, v);
        return v;
    }
    void top_down(trie *root, int cx, string s, trie *par, int level)
    {
        char ch = char(cx);
        int sizex = s.size();
        for (int i = 0; i <= sizex; i++)
        {
            if (i == 0)
                root->dp[i] = level;
            else if (s[i - 1] == ch)
                root->dp[i] = par->dp[i - 1];
            else if (root->dp[i - 1] >= par->dp[i] && par->dp[i - 1] >= par->dp[i])
                root->dp[i] = par->dp[i] + 1;
            else if (root->dp[i - 1] <= par->dp[i] && root->dp[i - 1] <= par->dp[i - 1])
                root->dp[i] = root->dp[i - 1] + 1;
            else
                root->dp[i] = par->dp[i - 1] + 1;
        }

        for (int i = 0; i < 256; i++)
        {
            if (root->child[i] != NULL)
            {
                top_down(root->child[i], i, s, root, 1 + level);
            }
        }
    }

    vector<string> autocorrect(trie *root, string s)
    {
        vector<string> ans;

        int sizex = s.size();
        for (int i = 0; i <= sizex; i++)
            root->dp[i] = i;
        for (int i = 0; i < 256; i++)
        {
            if (root->child[i] != NULL)
            {
                top_down(root->child[i], i, s, root, 1);
            }
        }
        string sans = "";

        traverse2(root, sizex, sans, ans);
        return ans;
    }
};

trie *commandTrie = new trie();

// sumonto
// misc utility functions
void clrs()
{
    cout << "\033[H\033[J" << flush;
}
vector<string> splitOnDelimiter(string cmd, char delimiter)
{ // splits string based on delimiter.
    vector<string> v;
    string temp = "";
    for (int i = 0; i < cmd.size(); i++)
    {
        if (cmd[i] != delimiter)
            temp += cmd[i];
        if (cmd[i] == delimiter || i == cmd.size() - 1)
        {
            if (temp.size())
            {
                v.push_back(temp);
                temp = "";
            }
        }
    }
    return v;
}

// sumonto
// raw mode functions

void di_raw()
{ // function to disable raw mode.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}
void en_raw()
{ // function to enable raw mode.
    tcgetattr(STDIN_FILENO, &term);
    atexit(di_raw);
    struct termios tp = term;
    // tp.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
    tp.c_oflag &= ~(OPOST);
    tp.c_lflag &= ~(ECHO | ICANON);
    tp.c_cc[VTIME] = 1;
    tp.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp);
}
// sumonto
int term_size(int *rows, int *cols)
{ // function to find the dimension of the terminal.
    struct winsize ts;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ts) == -1 || ts.ws_col == 0)
    {
        return -1;
    }
    else
    {
        *cols = ts.ws_col;
        *rows = ts.ws_row;

        return 0;
    }
}
// sumonto
void move_to(std::ostream &os, int row, int col)
{ // move cursor to (row, col).
    if (row != -1)
        os << "\033[" << row << ";" << col << "H";
    else
        os << "\033["
           << ";" << col << "H";
}

// sumonto
// char sense()
// {
//     int cread;
//     char c;
//     while ((cread = read(STDIN_FILENO, &c, 1)) != 1)
//     {
//         if (cread == -1 && errno != EAGAIN)
//             cout << "ERROR";
//     }
//     if (c == '\x1b')
//     {
//         char seq[3];
//         if (read(STDIN_FILENO, &seq[0], 1) != 1)
//             return '\x1b';
//         if (read(STDIN_FILENO, &seq[1], 1) != 1)
//             return '\x1b';
//         return '\x1b';
//     }
//     else
//     {
//         return c;
//     }
// }

// Sumonto/Subhadeep
const int upArrowKey = 256;
const int downArrowKey = 257;
const int leftArrowKey = 258;
const int rightArrowKey = 259;
const int deleteKey = 260;
int sense()
{
    int cread;
    char c;
    while ((cread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (cread == -1 && errno != EAGAIN)
            cout << "ERROR";
    }
    switch(c) {
        case '\x1b':
            if(read(STDIN_FILENO, &c, 1) == 1) {
                switch(c) {
                    case '[':
                        if(read(STDIN_FILENO, &c, 1) == 1) {
                            switch(c) {
                                case 'A':
                                    return upArrowKey;
                                    break;
                                case 'B':
                                    return downArrowKey;
                                    break;
                                case 'C':
                                    return rightArrowKey;
                                    break;
                                case 'D':
                                    return leftArrowKey;
                                    break;
                                case '3':
                                    if(read(STDIN_FILENO, &c, 1) == 1) {
                                        switch(c) {
                                            case '~':
                                                return deleteKey;
                                                break;
                                        }
                                    } else {
                                        // return "\x1b[3";
                                    }
                                    break;
                            }
                        } else {
                            // return "\x1b[";
                        }
                        break;
                }
            } else return '\x1b';
            break;
        default:
            return c;
    }
    return -1;
}

// // sumonto/soumadeep
// string fetch_command()
// {
//     int c = sense();
//     string command = "";

//     while (c != '\n')
//     {
//         if ((int)c == 127 && promptInput.size() > 0)
//         {
//             promptInput.pop_back();
//             clear_line;
//             // setcursor(lastRow - 1, 0);
//             prompt();
//             cout << promptInput << flush;
//         }
//         else if ((int)c == 127)
//         {

//             clear_line;
//             //    setcursor(lastRow - 1, 0);
//             prompt();
//             cout << promptInput << flush;
//         }
//         else if (c == 10)
//         {

//             command = promptInput;
//             promptInput = "";
//             break;
//         }
//         else if (c == '\x1b')
//         {
//             clear_line;
//             // setcursor(lastRow - 1, 0);
//             prompt();
//         }
//         else if (c == '\t')
//         {

//             vector<string> v1 = splitOnDelimiter(promptInput, ' ');
//             int n = v1.size();

//             string last = v1[n - 1];

//             vector<string> result = commandTrie->autocomplete(commandTrie, last);
//             if (result.size() == 1)
//             {
//                 //  prompt();
//                 promptInput = promptInput + result[0];
//                 cout << result[0] << flush;
//             }
//             else
//             {
//                 cout << "\n";
//                 cout << MOVE_CURSOR_LEFT;
//                 for (auto i : result)
//                     cout << last << i << " ";

//                 cout << "\n";
//                 // clear_line;
//                 // setcursor(lastRow - 1, 0);
//                 prompt();
//                 cout << promptInput << flush;
//             }
//         }
//         else
//         {
//             promptInput += c;
//             cout << c << flush;
//         }
//         c = sense();
//     }
//     cout << c << MOVE_CURSOR_LEFT << flush;
//     return promptInput;
// }

// Subhadeep
void moveCursor(int x, int y) {
    int xTemp{x}, yTemp{y}, numDigitsX{0}, numDigitsY{0};
    while(xTemp > 0) {
        ++numDigitsX;
        xTemp /= 10;
    }
    while(yTemp > 0) {
        ++numDigitsY;
        yTemp /= 10;
    }
    write(STDOUT_FILENO, (std::string("\x1b[") + std::to_string(x) + std::string(";") + std::to_string(y) + std::string("H")).data(), 4 + numDigitsX + numDigitsY);
}

std::pair<int, int> readCursorPos() {
    write(STDOUT_FILENO, "\x1b[6n", 4);
    char c;
    std::string s;
    while(read(STDIN_FILENO, &c, 1) == 1) {
        if(!iscntrl(c)) s.push_back(c);
    }
    std::string x, y;
    int i{1};
    while(s.at(i) != ';') x.push_back(s.at(i++));
    ++i;
    while(s.at(i) != 'R') y.push_back(s.at(i++));
    return {stoi(x), stoi(y)};
}

// Sumonto/Soumadeep/Subhadeep
string fetch_command()
{
    int cursorBaseX{readCursorPos().first}, cursorBaseY{readCursorPos().second}, cursorY{0};
    int indexInHistorylist = historylist.size();
    string promptInputRecent;
    int c = sense();

    while (c != '\n')
    {
        if (c == 127) {
            if(cursorY > 0) {
                promptInput = promptInput.substr(0, cursorY - 1) + promptInput.substr(cursorY);
                --cursorY;
                clear_line;
                prompt();
                cout << promptInput << flush;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if(c == leftArrowKey) {
            if(cursorY > 0) {
                --cursorY;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if(c == rightArrowKey) {
            if(cursorY < promptInput.length()) {
                ++cursorY;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if(c == deleteKey) {
            if(cursorY < promptInput.length()) {
                promptInput = promptInput.substr(0, cursorY) + promptInput.substr(cursorY + 1);
                clear_line;
                prompt();
                cout << promptInput << flush;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if(c == upArrowKey) {
            if(indexInHistorylist > 0) {
                if(indexInHistorylist == historylist.size()) promptInputRecent = promptInput;
                --indexInHistorylist;
                promptInput = historylist.at(indexInHistorylist);
                cursorY = promptInput.length();
                clear_line;
                prompt();
                cout << promptInput << flush;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if(c == downArrowKey) {
            if(indexInHistorylist < historylist.size()) {
                ++indexInHistorylist;
                if(indexInHistorylist < historylist.size()) promptInput = historylist.at(indexInHistorylist);
                else promptInput = promptInputRecent;
                cursorY = promptInput.length();
                clear_line;
                prompt();
                cout << promptInput << flush;
                moveCursor(cursorBaseX, cursorBaseY + cursorY);
            }
        } else if (c == '\t') {

            vector<string> promptInputParsed = parseCommand2(promptInput);
            if(promptInputParsed.size() == 1) {
                trie *commandTrie1 = new trie();
                string commandPart = promptInputParsed.at(0);
                if(commandPart.at(0) == '\"') commandPart = commandPart.substr(1);
                if (commandPart.at(0) != '/' && (commandPart.at(0) != '~' || (commandPart.length() > 1 && commandPart.at(1) != '/')) && (commandPart.at(0) != '.' || commandPart.length() > 1 && (commandPart.at(1) != '.' || commandPart.length() > 2 && commandPart.at(2) != '/') && commandPart.at(1) != '/')) {
                    // search in path
                    string prefix(""), suffix(commandPart);
                    if(commandPart.find('/') != string::npos) {
                        prefix = commandPart.substr(0, commandPart.rfind('/'));
                        suffix = commandPart.substr(commandPart.rfind('/') + 1);
                    }
                    vector<string> paths = splitOnDelimiter(getenv("PATH"), ':');
                    for (string &s : paths) {
                        DIR *searchLoc = opendir((s + "/" + prefix).data());
                        if(searchLoc) {
                            struct dirent *direntp;
                            while (direntp = readdir(searchLoc)) {
                                struct stat sb;
                                stat((s + "/" + prefix + "/" + direntp->d_name).data(), &sb);
                                if(S_ISDIR(sb.st_mode)) {
                                    // commandTrie1->insert(commandTrie1, s + prefix + "/" + direntp->d_name + "/");
                                    commandTrie1->insert(commandTrie1, direntp->d_name + string("/"));
                                } else {
                                    // commandTrie1->insert(commandTrie1, s + prefix + "/" + direntp->d_name);
                                    commandTrie1->insert(commandTrie1, direntp->d_name);
                                }
                                // commandTrie1->insert(commandTrie1, direntp->d_name);
                                // cout << direntp->d_name << endl << "\r";
                            }
                        }
                    }
                    vector<string> matchedSuffixes = commandTrie1->autocomplete(commandTrie1, suffix);
                    if(matchedSuffixes.size() == 1) {
                        promptInput = promptInput + matchedSuffixes.at(0);
                        cursorY = promptInput.length();
                        clear_line;
                        prompt();
                        cout << promptInput << flush;
                        moveCursor(cursorBaseX, cursorBaseY + cursorY);
                    } else if(matchedSuffixes.size() > 1) {
                        cout << "\n" << MOVE_CURSOR_LEFT << flush;
                        for(string &s : matchedSuffixes) {
                            cout << promptInput + s << " ";
                        }
                        cout << "\n" << flush;
                        clear_line;
                        prompt();
                        cout << promptInput << flush;
                        cursorBaseX = readCursorPos().first;
                        moveCursor(cursorBaseX, cursorBaseY + cursorY);
                    }
                } else {
                    if(commandPart.find('/') != string::npos) {
                        string prefix(""), suffix(commandPart);
                        prefix = commandPart.substr(0, commandPart.rfind('/'));
                        suffix = commandPart.substr(commandPart.rfind('/') + 1);
                        DIR *searchLoc = opendir(prefix.data());
                        if(searchLoc) {
                            struct dirent *direntp;
                            while (direntp = readdir(searchLoc)) {
                                struct stat sb;
                                stat((prefix + "/" + direntp->d_name).data(), &sb);
                                if(S_ISDIR(sb.st_mode)) {
                                    commandTrie1->insert(commandTrie1, direntp->d_name + string("/"));
                                } else {
                                    commandTrie1->insert(commandTrie1, direntp->d_name);
                                }
                            }
                        }
                        vector<string> matchedSuffixes = commandTrie1->autocomplete(commandTrie1, suffix);
                        if(matchedSuffixes.size() == 1) {
                            promptInput = promptInput + matchedSuffixes.at(0);
                            cursorY = promptInput.length();
                            clear_line;
                            prompt();
                            cout << promptInput << flush;
                            moveCursor(cursorBaseX, cursorBaseY + cursorY);
                        } else if(matchedSuffixes.size() > 1) {
                            cout << "\n" << MOVE_CURSOR_LEFT << flush;
                            for(string &s : matchedSuffixes) {
                                cout << promptInput + s << " ";
                            }
                            cout << "\n" << flush;
                            clear_line;
                            prompt();
                            cout << promptInput << flush;
                            cursorBaseX = readCursorPos().first;
                            moveCursor(cursorBaseX, cursorBaseY + cursorY);
                        }
                    }
                }
            } else if(promptInputParsed.size() > 1) {
                trie *argTrie = new trie();
                string argPart = promptInputParsed.back();
                if(argPart.at(0) == '\"') argPart = argPart.substr(1);
                if (argPart.at(0) != '/' && (argPart.at(0) != '~' || (argPart.length() > 1 && argPart.at(1) != '/')) && (argPart.at(0) != '.' || argPart.length() > 1 && (argPart.at(1) != '.' || argPart.length() > 2 && argPart.at(2) != '/') && argPart.at(1) != '/')) {
                    argPart = "./" + argPart;
                }
                if(argPart.find('/') != string::npos) {
                    string prefix(""), suffix(argPart);
                    prefix = argPart.substr(0, argPart.rfind('/'));
                    suffix = argPart.substr(argPart.rfind('/') + 1);
                    DIR *searchLoc = opendir(prefix.data());
                    if(searchLoc) {
                        struct dirent *direntp;
                        while (direntp = readdir(searchLoc)) {
                            struct stat sb;
                            stat((prefix + "/" + direntp->d_name).data(), &sb);
                            if(S_ISDIR(sb.st_mode)) {
                                argTrie->insert(argTrie, direntp->d_name + string("/"));
                            } else {
                                argTrie->insert(argTrie, direntp->d_name);
                            }
                        }
                    }
                    vector<string> matchedSuffixes = argTrie->autocomplete(argTrie, suffix);
                    if(matchedSuffixes.size() == 1) {
                        promptInput = promptInput + matchedSuffixes.at(0);
                        cursorY = promptInput.length();
                        clear_line;
                        prompt();
                        cout << promptInput << flush;
                        moveCursor(cursorBaseX, cursorBaseY + cursorY);
                    } else if(matchedSuffixes.size() > 1) {
                        cout << "\n" << MOVE_CURSOR_LEFT << flush;
                        for(string &s : matchedSuffixes) {
                            cout << promptInputParsed.back() + s << "\n\r" << flush;
                        }
                        clear_line;
                        prompt();
                        cout << promptInput << flush;
                        cursorBaseX = readCursorPos().first;
                        moveCursor(cursorBaseX, cursorBaseY + cursorY);
                    }
                }
            }

            cursorBaseX = readCursorPos().first;
        }
        else
        {
            promptInput = promptInput.substr(0, cursorY) + string(1, c) + promptInput.substr(cursorY);
            ++cursorY;
            indexInHistorylist = historylist.size();
            clear_line;
            prompt();
            cout << promptInput << flush;
            moveCursor(cursorBaseX, cursorBaseY + cursorY);
        }
        c = sense();
    }
    cout << static_cast<char>(c) << MOVE_CURSOR_LEFT << flush;
    return promptInput;
}

// soumadepp
void setTerminalPos()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize);
    lastRow = windowSize.ws_row;
    firstColumn = 0;
}
// soumadeep
void host_user()
{
    home = string(getenv("HOME"));
    gethostname(&hostname[0], hostname.size());
    getlogin_r(&username[0], username.size());
    pid_t p_id;
    p_id = getpid();
    shellId = p_id;
}

// soumadeepx
void checkoutHistory()
{
    fstream nfile;
    nfile.open(his_sav_File.c_str(),ios::out);  
    if(nfile.is_open()) 
    {
            for (auto i: historylist)
            {
                 nfile<<i;  
            } 
        nfile.close();    
    }
}
void printHistory(int u)
{
    int n=historylist.size();
    if(u==-1)
    u=k;
    for (int i=n-u;i<n;i++)
    {
        cout<<" - "<<historylist[i]<<"\n";
    }
}
void checkinHistory()
{
    
    string curdir= string(getenv("PWD"));
    his_sav_File =curdir+ "/history.txt";


    fstream nfile;
    nfile.open(his_sav_File,ios::in); 
    if (nfile.is_open())
    {   
        string line;
        while(getline(nfile, line)){ 
           historylist.push_back(line);
            no++;
        }
        nfile.close(); 
    }
     else
    {
        cout<<"history.txt not available"<<flush;
        exit(EXIT_FAILURE);
    }
    return;
}
// soumadeep

void insertingTrie()
{
    for (int i = 0; i < commandList.size(); i++)
    {
        commandTrie->insert(commandTrie, commandList[i]);
    }
}
// soumadeep:::::::::::::::
void setcursor(int row, int col)
{
    string str = "\033[";
    str += to_string(row);
    str += ";";
    str += to_string(col);
    str += "H";
    cout << str;

    fflush(stdout);
}
// soumadeep:::::::::::::::=====++++====)()(())
void init()
{
    //  line 167
    host_user();
    //  line 180
    checkinHistory();
    initAlarm();
    insertingTrie();

    en_raw();
    clrs();
    // cout<<"c3";

    promptInit();

    // setTerminalPos();
    // if(lastRow>=0&&firstColumn>=0)
    // setcursor(lastRow, firstColumn);
}

void writeString(const string &s)
{
    write(STDOUT_FILENO, s.data(), s.length());
}

void promptInit()
{
    string user_symbol = "#";
    int len = 3000;
    char dummy1[len], dummy2[len], dummy3[len];
    getlogin_r(dummy1, sizeof(dummy1));
    gethostname(dummy2, sizeof(dummy2));
    getcwd(dummy3, sizeof(dummy3));
    string user(dummy1), hostname(dummy2), varCwd(dummy3);
    user_symbol = (geteuid()) ? "%" : "#";
    char *p;

    p = (char *)strstr(varCwd.c_str(), home.c_str());
    string dir_Name;
    dir_Name = p ? p + home.size() : varCwd;
    promptString = (p) ? (string(MAGENTA) + user + "@" + hostname + TURNRESET + ":" + BLUE + "~" + dir_Name + user_symbol + " " + TURNRESET) : ("\n" + string(MAGENTA) + user + "@" + hostname + TURNRESET + ":" + BLUE + "~" + dir_Name + user_symbol + " " + TURNRESET);
}

// soumadeep =====}{}{}{}
void prompt()
{
    cout << MOVE_CURSOR_LEFT << promptString << flush;
    // cout << promptString << flush;
}

// Subhadeep
vector<string> parseCommand(string command)
{
    // parses a single command with quotes and replaces environment variables
    vector<string> parsedCommand;
    bool quotedArgStarted{false}, nonQuotedArgStarted{false};
    for (int i{0}; i < command.length(); ++i)
    {
        if (quotedArgStarted)
        {
            if (command.at(i) != '"')
                parsedCommand.back().push_back(command.at(i));
            else
                quotedArgStarted = false;
        }
        else if (nonQuotedArgStarted)
        {
            if (command.at(i) != ' ')
                parsedCommand.back().push_back(command.at(i));
            else
                nonQuotedArgStarted = false;
        }
        else if (command.at(i) == '"')
        {
            quotedArgStarted = true;
            parsedCommand.push_back("");
        }
        else if (command.at(i) != ' ')
        {
            if (command.at(i) == '~' && (i == command.size() - 1 || command.at(i + 1) == ' '))
            {
                parsedCommand.push_back(getenv("HOME"));
            }
            else
            {
                nonQuotedArgStarted = true;
                parsedCommand.push_back(string(1, command.at(i)));
            }
        }
    }

    for (string &s : parsedCommand)
    {
        int i{0};
        if (s.at(0) == '~' && (s.at(1) == '/' || s.at(1) == ':'))
        {
            s = home + s.substr(1);
            i = home.length();
        }
        while (i < s.length())
        {
            if (s.at(i) == '$')
            {
                if (i + 1 < s.length())
                {
                    if (s.at(i + 1) == '$')
                    {
                        s = s.substr(0, i) + to_string(shellId) + s.substr(i + 2);
                        i += to_string(shellId).length();
                    }
                    else if (s.at(i + 1) == '?')
                    {
                        s = s.substr(0, i) + to_string(exitStatus) + s.substr(i + 2);
                        i += to_string(exitStatus).length();
                    }
                    else
                    {
                        int j{i + 1};
                        while (j < s.length() && (('a' <= s.at(j) && s.at(j) <= 'z') || ('A' <= s.at(j) && s.at(j) <= 'Z') || ('0' <= s.at(j) && s.at(j) <= '9')))
                            ++j;
                        string var = s.substr(i + 1, j - i);
                        string value;
                        if(getenv(var.data())) value = getenv(var.data());
                        else value = "";
                        s = s.substr(0, i) + value + s.substr(j);
                        i += value.length();
                        // if (var == "PATH")
                        // {
                        //     s = s.substr(0, i) + getenv("PATH") + s.substr(j);
                        //     i += string(getenv("PATH")).length();
                        // }
                    }
                }
                else
                {
                    ++i;
                }
            }
            else
            {
                ++i;
            }
        }
    }

    return parsedCommand;
}

// Subhadeep
vector<string> parseCommand2(string command) {
    // parses a single command with quotes
    // does not remove quotes
    // does not replace environment variables
    vector<string> parsedCommand;
    bool quotedArgStarted{false}, nonQuotedArgStarted{false};
    for (int i{0}; i < command.length(); ++i)
    {
        if (quotedArgStarted)
        {
            parsedCommand.back().push_back(command.at(i));
            if (command.at(i) == '"') quotedArgStarted = false;
        }
        else if (nonQuotedArgStarted)
        {
            if (command.at(i) != ' ')
                parsedCommand.back().push_back(command.at(i));
            else
                nonQuotedArgStarted = false;
        }
        else if (command.at(i) == '"')
        {
            quotedArgStarted = true;
            parsedCommand.push_back("\"");
        }
        else if (command.at(i) != ' ')
        {
            nonQuotedArgStarted = true;
            parsedCommand.push_back(string(1, command.at(i)));
        }
    }

    return parsedCommand;
}


// sumonto
void pipeHandler(string command)
{
    vector<string> commands = splitOnDelimiter(command, '|');
    // for (int i = 0; i < commands.size(); i++)
    //     commands[i] = trim(commands[i]);
    // for(int i=0; i<commands.size(); i++) cout<<"#"<<commands[i]<<"#"<<endl;
    int fd[2], fileD = 0;
    int pid;
    for (int i = 0; i < commands.size(); i++)
    {
        pipe(fd);
        pid = fork();
        if(pid == 0) {
            dup2(fileD, 0);
            if (i != commands.size() - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
            }
            close(fd[0]);
            executeCommand(commands[i]);
            exit(EXIT_SUCCESS);
        }
        else if(pid>0) {
            wait(NULL);
            close(fd[1]);
            fileD=fd[0];
        }
    }
    pid=fork();
    if(!pid) {
        executeCommand("");
    }
    else{
        wait(&pid);
        return;
    }
    // if (pipe(fd) == -1)
    // {
    //     return;
    // }
    // int pid1 = fork();
    // if (pid1 < 0)
    // {
    //     return;
    // }
    // if (pid1 == 0)
    // {
    //     // left process
    //     dup2(fd[1], STDOUT_FILENO);
    //     close(fd[0]);
    //     close(fd[1]);
    //     execlp("ping", "Ping", "-c", "5", "google.com", NULL);
    // }
    // int pid2 = fork();
    // if (pid2 < 0)
    // {
    //     return;
    // }
    // if (pid2 == 0)
    // {
    //     dup2(fd[0], STDIN_FILENO);
    //     // right process
    //     execlp("grep", "grep", "rtt", NULL);
    // }
    close(fd[0]);
    close(fd[1]);
    // waitpid(pid1, NULL, 0);
    // waitpid(pid2, NULL, 0);
}
//sumonto
int timeToEpoch(string endtime) {
    //hh:mm:ss to (time in seconds)
    // cout<<"string to convert #"<<endtime<<"#"<<endl<<flush;
    vector<string> hms=splitOnDelimiter(endtime, ':');
    int h=stoi(hms[0]);
    int m=stoi(hms[1]);
    int s=stoi(hms[2]);
    time_t epoch;
    epoch = time(NULL);
    int secs=epoch+h*60*60+m*60+s;
    // cout<<"converted "<<secs<<endl<<flush;
    return secs;
}
// sumonto
void * alarmThread(void * ptr) {
    int duration = *((int *) ptr);
    // cout<<"thread "<<duration<<endl<<flush;
    sleep(duration);
    di_raw();
    executeCommand("echo \"\n\033[7m ALARM! \033[0m press Enter to continue.\"");
    en_raw();
    pthread_exit(NULL);
}
//sumonto
void setAlarm(int secs) {
    fstream alarms;
    alarms.open(alarm_list.c_str(), ios::app);
    time_t epoch;
    epoch = time(NULL);
    alarms<<secs<<endl;
    // cout<<"set Alarm "<<secs<<endl<<flush;
    alarms.close();
    // int id = fork();
    // if(!id) {
    //     sleep(secs-epoch);
    //     di_raw();
    //     executeCommand("echo \nALARM!");
    //     en_raw();
    //     exit(0);
    // }
    int *duration=(int *)malloc(sizeof(*duration));
    *duration = secs - epoch;
    // cout<<"set alarm "<<duration<<endl<<flush;
    pthread_t T;
	pthread_create(&T, NULL, alarmThread, duration);
    threads.push_back(T);
}
//sumonto
void initAlarm(){
    vector<int> list;
    fstream alarms;
    alarms.open(alarm_list.c_str(), ios::in);
    time_t epoch;
    epoch=time(NULL);
    string s;
    while(alarms>>s) {
        if(stoi(s)>epoch)
            list.push_back(stoi(s));
    }
    alarms.close();
    alarms.open(alarm_list.c_str(), ios::out | ios::trunc);
    alarms.close();
    for(auto x: list) {
        setAlarm(x);
    }
}
//sumonto
void Export(string command) {
    // expected command example.
    //export -p
    //export -n nameToDelete
    //export key = value
    vector<string> arguments=splitOnDelimiter(command, ' ');
    if(arguments[1]=="-p") {
        for(auto x: exports) {
            cout<<x.first<<" = \""<<x.second<<"\""<<endl<<flush;
        }
    }
    else if(arguments[1]=="-n") {
        exports.erase(arguments[2]);
    }
    else {
        exports[arguments[1]]=arguments[3];
    }
}
//sumonto 
void Exit() {
    for(pthread_t x: threads) {
        pthread_join(x, NULL);
    }
    exit(0);
}
//sumonto
void bg(string cmd) {
    //command expected type "name arg1 agr2 ...argn &"
    string command="";
    vector<string> temp=splitOnDelimiter(cmd, ' ');
    for(int i=0; i<temp.size()-2; i++) {
        command+=temp[i]+" ";
    }
    command+=temp[temp.size()-2];
    srand(time(0));
    int id=rand();
    id=id%1000000;
    if(id<100000) {
        id=1e7-id;
    }
    while(bgCommands.find(id)!=bgCommands.end()){
        id=rand();
        id=id%1000000;
        if(id<100000) {
            id=1e7-id;
        }
    }
    bgCommands[id]=command;
}
//sumonto
void fg(int index) {
    //command expected type "fg id"
    executeCommand(bgCommands[index]);
    bgCommands.erase(index);
}
//sumonto
void showBg() {
    cout<<"\033[7m ID     Command \n\033[0m";
    for(auto x: bgCommands) {
        cout<<x.first<<"  "<<x.second<<endl<<flush;
    }
}

// soumadeep
void runAlias(vector<string> aliasCommand)
{
    string str = aliasList[aliasCommand[0]];

    vector<string> tokens= splitOnDelimiter(str,' ');
    int i=1;
    while( i < aliasCommand.size())
    {
       tokens.push_back(aliasCommand[i]);
       i++;
    }
    string com="";
    for (auto i: tokens)
    {
        com=com+i+" ";
    }
    executeCommand(com);
        // RUN(Tokens);
}
// soumadeep
void controlAlias()
{
    string x;
    ifstream inputFile(".myrc");
    while (getline(inputFile, x))
    {
        vector<string> v = splitOnDelimiter(x, ' ');
        if (v.empty())
            continue;
        string key = v[1];
        string val = "";
        int sizeofVector = v.size();
        int i = 3;
        if (v[0] == "alias")
        {
            while (i < sizeofVector - 1)
            {
                val += v[i] + " ";
                i++;
            }
            val = val + v[i];
            aliasList[key] = val;
        }
    }
    inputFile.close();
}
// soumadeep
void insertAlias(vector <string> aliasCommand , string realcommand)
{
     if (aliasCommand.size() <=3)
        {
            cout << "incorrect command" << endl;
            statusFlag = false;
            return;
        }
        string Value = "";
        int i=3;
        while( i <aliasCommand.size())
        {
            Value += aliasCommand[i] + " ";
            i++;
        }
        // ofstream outfile;

        aliasList[aliasCommand[1]] = Value;

        

        // outfile.open(".myrc", std::ios_base::app); 
        // outfile << '\n'
        //         << realcommand;
        // outfile.close();
        //  controlAlias();
}
// soumadeep
// void insertAlias(vector <string> aliasCommand , string realcommand)
// {
//      if (aliasCommand.size() <=3)
//         {
//             cout << "incorrect command" << endl;
//             statusFlag = false;
//             return;
//         }
//         string Value = "";
//         int i=3;
//         while( i <aliasCommand.size())
//         {
//             Value += aliasCommand[i] + " ";
//             i++;
//         }
//         ofstream outfile;

//         aliasList[aliasCommand[1]] = Value;

        

//         outfile.open(".myrc", std::ios_base::app); 
//         outfile << '\n'
//                 << realcommand;
//         outfile.close();
//         // controlAlias();
// }

// Soumadeep
bool checkForIo(string command)
{
    int x1 = command.find(">");
    int flag=0;
     if (x1 != std::string::npos)
     flag=1;
    int x2 = command.find(">>");
    if (x2 != std::string::npos)
     flag=1;

    int  x3 = command.find("<");
    if (x3 != std::string::npos)
     flag++;
     if(flag==1)
      return true;
     return false;
}

// Subhadeep
void executeCommand(string singleCommand)
{
    // historylist.push_back(singleCommand);
    vector<string> commandTokens = parseCommand(singleCommand);
    if(commandTokens.size() == 0) return;

    // no pipe
    int eqPos;
    if ((eqPos = commandTokens.at(0).find('=')) != string::npos && eqPos + 1 < commandTokens.at(0).size()) {
        // command: var=value
        if (commandTokens.size() == 1) {
            bool executeEqCommand{false};
            string var = commandTokens.at(0).substr(0, eqPos);
            string value;
            if (commandTokens.at(0).at(eqPos + 1) == '"') {
                if (commandTokens.at(0).back() == '"') {
                    value = commandTokens.at(0).substr(eqPos + 2, commandTokens.at(0).length() - eqPos - 3);
                    executeEqCommand = true;
                } else {
                    executeEqCommand = false;
                    // wrong command
                }
            } else {
                value = commandTokens.at(0).substr(eqPos + 1);
                executeEqCommand = true;
            }
            if (executeEqCommand) {
                // if (var == "PATH") {
                //     setenv("PATH", value.data(), 1);
                // }
                if (var == "PS1") {
                    if(value == "DEFAULT") {
                        promptInit();
                        promptStringChanged = false;
                        setenv("PS1", promptString.data(), 1); 
                    } else {
                        promptString = value;
                        promptStringChanged = true;
                        setenv("PS1", value.data(), 1);
                    }
                } else {
                    setenv(var.data(), value.data(), 1);
                }
                historylist.push_back(singleCommand);
            }
        }
        else
        {
            // execute next command
        }
    }
    else {
        // execute non eq commands
        string commandName{commandTokens.at(0)};
        if (commandName.at(0) != '/' && (commandName.at(0) != '.' || commandName.length() > 1 && (commandName.at(1) != '.' || commandName.length() > 2 && commandName.at(2) != '/') && commandName.at(1) != '/')) {
            if(commandName == "unset") {
                for(int i{1}; i < commandTokens.size(); ++i) {
                    unsetenv(commandTokens.at(i).data());
                }
            } else if(commandName == "history") {
                if(commandTokens.size() == 1) {
                    // execute history
                    historylist.push_back(singleCommand);
                    printHistory(-1);
                } else if(commandTokens.size() == 2) {
                    // history with integer argument
                    historylist.push_back(singleCommand);
                    printHistory(stoi(commandTokens[1]));
                } else {
                    cout << "our shell: history: too many arguments" << endl;
                }
            } else if(commandName == "cd") {
                if(commandTokens.size() == 1) {
                    // go to home
                    chdir(home.data());
                    if(!promptStringChanged) promptInit();
                    historylist.push_back(singleCommand);
                } else if(commandTokens.size() == 2) {
                    if(!chdir(commandTokens.at(1).data())) {
                        if(!promptStringChanged) promptInit();
                        historylist.push_back(singleCommand);
                    } else {
                        cout << "our shell: " + commandName + ": No such file or directory" << endl;
                    }
                } else {
                    cout << "our shell: cd: too many arguments" << endl;
                }
            } else if(commandName == "alias") {
                if(commandTokens.size() == 1) {
                    // list alias
                } else {
                    vector<string> vars, vals;
                    for(int alias_i{1}; alias_i < commandTokens.size(); ++ alias_i) {
                        // cout << commandTokens.at(alias_i) << " " << endl;
                        int aliasEqPos;
                        if ((aliasEqPos = commandTokens.at(alias_i).find('=')) != string::npos && aliasEqPos + 1 < commandTokens.at(alias_i).size()) {
                            if (commandTokens.at(alias_i).at(aliasEqPos + 1) == '"') {
                                if (commandTokens.at(alias_i).back() == '"') {
                                    vars.push_back(commandTokens.at(alias_i).substr(0, aliasEqPos));
                                    vals.push_back(commandTokens.at(alias_i).substr(aliasEqPos + 2, commandTokens.at(alias_i).length() - aliasEqPos - 3));
                                } else {
                                    // wrong syntax
                                }
                            } else {
                                vars.push_back(commandTokens.at(alias_i).substr(0, aliasEqPos));
                                vals.push_back(commandTokens.at(alias_i).substr(aliasEqPos + 1));
                            }
                        } else {
                            // wrong syntax
                        }
                    }
                    for(int i{0}; i < vars.size(); ++i) {
                        insertAlias(vector<string>({"alias", vars.at(i), "=", vals.at(i)}), "alias " + vars.at(i) + "=" + vals.at(i));
                        // cout << vars.at(i) << " " << vals.at(i) << endl;
                    }

                }
                historylist.push_back(singleCommand);
            } else {
                vector<string> paths = splitOnDelimiter(getenv("PATH"), ':');
                bool found{false};
                for (string &s : paths) {
                    DIR *pathLoc = opendir(s.data());
                    struct dirent *direntp;
                    while (direntp = readdir(pathLoc)) {
                        if (direntp->d_name == commandName) {
                            commandName = s + "/" + commandName;
                            found = true;
                        }
                    }
                    if (found) break;
                }
                if (found) {
                    char **argv = new char *[commandTokens.size() + 1];
                    for (int i{0}; i < commandTokens.size(); ++i)
                    {
                        argv[i] = commandTokens.at(i).data();
                    }
                    argv[commandTokens.size()] = 0;
                    int childPid;
                    if (!(childPid = fork()))
                    {
                        execv(commandName.data(), argv);
                    }
                    waitpid(childId, &exitStatus, 0);
                    historylist.push_back(singleCommand);
                } else if(commandName == "echo") {
                        for(int i{1}; i < commandTokens.size() - 1; ++i) {
                            if(commandTokens.at(i) != "") writeString(commandTokens.at(i) + " ");
                        }
                        if(commandTokens.back() != "" && commandTokens.size() > 1) writeString(commandTokens.back());
                        writeString("\n");
                        historylist.push_back(singleCommand);
                } else {
                    // command not found
                    runAlias(commandTokens);
                }
            }
        } else {
            if(!access(commandName.data(), F_OK)) {
                // cout << "Hello" << endl;
                char **argv = new char *[commandTokens.size() + 1];
                for (int i{0}; i < commandTokens.size(); ++i) {
                    argv[i] = commandTokens.at(i).data();
                }
                argv[commandTokens.size()] = 0;
                int childPid;
                if (!(childPid = fork())) {
                    execv(commandName.data(), argv);
                    std::cout << std::flush;
                }
                waitpid(childId, &exitStatus, 0);
                std::cout << std::flush;
                historylist.push_back(singleCommand);
            } else {
                writeString("our shell: " + commandName + ": No such file or directory");
                cout << endl;
            }
        }
    };
    int len = 3000;
    char dummy[len];
    getcwd(dummy, sizeof(dummy));
    setenv("PWD", dummy, 1);
}

// soumadeep
void shell()
{

    string input;
    insertingTrie();
    // populateAliasMap();
    // controlAlias();

    for (;;)
    {
        childId = -1;
        // setcursor(lastRow-1,0);

        prompt();
        promptInput = "";
        input = fetch_command();
        if (!input.empty())
        {

            vector<string> tokens = splitOnDelimiter(input, ';');

            for (int i = 0; i < tokens.size(); i++)
            {
                // cout<<"--"<<tokens[i]<<"--";
                //    execute_given_command(tokens[i]);
                vector<string> parsedCommand = parseCommand(tokens.at(i));
                if(parsedCommand.size() >= 3 && checkForIo(tokens.at(i)) && (parsedCommand.at(parsedCommand.size() - 2) == "<" || parsedCommand.at(parsedCommand.size() - 2) == ">" || parsedCommand.at(parsedCommand.size() - 2) == ">>") ) {
                    if(parsedCommand.at(parsedCommand.size() - 2) == "<") {
                        string s("cat ");
                        s += parsedCommand.back() + " | ";
                        for(int i{0}; i < parsedCommand.size() - 2; ++i) {
                            s += "\"" + parsedCommand.at(i) + "\" ";
                        }
                        di_raw();
                        pipeHandler(s);
                        en_raw();
                        historylist.push_back(tokens.at(i));
                    } else if(parsedCommand.at(parsedCommand.size() - 2) == ">" || parsedCommand.at(parsedCommand.size() - 2) == ">>") {
                        int out_temp = open("temp_out", O_RDWR|O_CREAT, 0664);
                        int stdout_fd = dup(fileno(stdout));
                        dup2(out_temp, fileno(stdout));

                        string s("");
                        for(int i{0}; i < parsedCommand.size() - 2; ++i) {
                            s += "\"" + parsedCommand.at(i) + "\" ";
                        }
                        di_raw();
                        executeCommand(s);
                        en_raw();

                        if(parsedCommand.at(parsedCommand.size() - 2) == ">") {
                            char c{0};
                            int out_temp2 = open("temp_out", O_RDONLY, 0664);
                            remove(parsedCommand.back().data());
                            int fd_out = open(parsedCommand.back().data(), O_RDWR|O_CREAT, 0664);
                            while(read(out_temp2, &c, 1) == 1) write(fd_out, &c, 1);
                            close(out_temp2);
                            close(fd_out);
                        } else if(parsedCommand.at(parsedCommand.size() - 2) == ">>") {
                            char c{0};
                            int out_temp2 = open("temp_out", O_RDONLY, 0664);
                            int fd_out = open(parsedCommand.back().data(), O_RDWR|O_CREAT|O_APPEND, 0664);
                            while(read(out_temp2, &c, 1) == 1) write(fd_out, &c, 1);
                            close(out_temp2);
                            close(fd_out);
                        }

                        historylist.pop_back();
                        historylist.push_back(tokens.at(i));

                        fflush(stdout); close(out_temp);

                        dup2(stdout_fd, fileno(stdout));

                        close(stdout_fd);
                        remove("temp_out");
                    }
                } else {
                    di_raw();
                    if(tokens.at(i).find("|") != string::npos) pipeHandler(tokens.at(i));
                    else if(splitOnDelimiter(tokens.at(i), ' ').back()=="&") bg(tokens.at(i));
                    else if(splitOnDelimiter(tokens.at(i), ' ')[0]=="fg") 
                            fg(stoi(splitOnDelimiter(tokens.at(i), ' ')[1]));
                    else if(splitOnDelimiter(tokens.at(i), ' ')[0]=="showbg") showBg();
                    else if(splitOnDelimiter(tokens.at(i), ' ')[0]=="export") Export(tokens.at(i));
                    else if(splitOnDelimiter(tokens.at(i), ' ')[0]=="alarm") {
                        setAlarm(timeToEpoch(splitOnDelimiter(tokens.at(i), ' ')[1]));
                    }
                    else if(trim(tokens.at(i))=="exit") Exit();
                    else executeCommand(tokens.at(i));
                    en_raw();
                }
            }
            cout << flush;
        }
        cout << flush;
    }
}

int main()
{

    init();
    // signal(SIGINT, ctrl_c);
    // signal(SIGCHLD, handler);
    clear_line;
    clear_screen;
    if (lastRow-1>= 0)
    {
        // setcursor(lastRow - 1, 0);
    }

    shell();

    // en_raw();
    // clrs();
    // while(1) { // just taking strings and printing to verify.
    //     string command=fetch_command();
    //     cout<<MOVE_CURSOR_LEFT<<command<<flush;
    // }
    // pipeHandler("pingc -c 5 google.com | grep rtt");
    // pipeHandler("ping -c 10 google.com | tail -10 | grep bytes");

    return 0;
}