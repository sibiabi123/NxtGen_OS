// kernel.c - NxtGen OS Simulator for QEMU (bare-metal)
#include <stdint.h>

// VGA memory and cursor
uint16_t* video_memory = (uint16_t*)0xB8000;
uint16_t cursor_pos = 0;

// Users
typedef struct {
    char username[8];
    char password[8];
} User;

User users[3] = {
    {"root","root"},
    {"alice","123"},
    {"bob","123"}
};

char current_user[8];

// File simulation
typedef struct {
    char name[8];
    char content[32];
} File;

File files[10];
int file_count = 0;

// Simple strings for printing
void kprint(const char* str) {
    while(*str) {
        video_memory[cursor_pos++] = (*str | 0x0F00); // white text
    }
}

void kprintln(const char* str) {
    kprint(str);
    video_memory[cursor_pos++] = ('\n' | 0x0F00);
}

// Simple string compare
int kstrcmp(const char* a, const char* b) {
    while(*a && *b) {
        if(*a != *b) return 1;
        a++; b++;
    }
    return (*a != *b);
}

// Simple string copy
void kstrcpy(char* dest, const char* src) {
    while(*src) *dest++ = *src++;
    *dest = 0;
}

// Clear screen
void kclear() {
    for(int i=0;i<80*25;i++) video_memory[i]=0x0700;
    cursor_pos=0;
}

// Dummy getchar: reads from QEMU keyboard
char getch() {
    char c;
    asm volatile("int $0x16":"=a"(c):"0"(0x00));
    return c;
}

// Read line from keyboard
void kreadline(char* buf, int max) {
    int i=0;
    while(1) {
        char c=getch();
        if(c=='\r') { buf[i]=0; kprintln(""); return; }
        if(c=='\b' && i>0) { i--; kprint("\b"); continue; }
        buf[i++]=c;
        char s[2]={c,0};
        kprint(s);
        if(i>=max-1) { buf[i]=0; kprintln(""); return; }
    }
}

// Authenticate user
void login() {
    char uname[8], pwd[8];
    while(1) {
        kprint("Username: "); kreadline(uname,8);
        kprint("Password: "); kreadline(pwd,8);
        for(int i=0;i<3;i++) {
            if(kstrcmp(uname,users[i].username)==0 &&
               kstrcmp(pwd,users[i].password)==0) {
                kstrcpy(current_user,uname);
                kprintln("Login successful!");
                return;
            }
        }
        kprintln("Login failed! Try again.");
    }
}

// Commands
void cmd_help() {
    kprintln("Commands: help ls cat touch rm calc date clear restart poweroff exit");
}

void cmd_ls() {
    if(file_count==0){ kprintln("No files."); return; }
    for(int i=0;i<file_count;i++) kprintln(files[i].name);
}

void cmd_touch(char* name) {
    if(file_count>=10){ kprintln("Max files reached"); return; }
    kstrcpy(files[file_count].name,name);
    kstrcpy(files[file_count].content,"Empty file");
    file_count++;
    kprintln("File created.");
}

void cmd_cat(char* name) {
    for(int i=0;i<file_count;i++) {
        if(kstrcmp(files[i].name,name)==0) {
            kprintln(files[i].content);
            return;
        }
    }
    kprintln("File not found.");
}

// Simple calculator: only +, -, *, /
void cmd_calc() {
    char buf[16]; double a,b; char op;
    kprint("Enter expression (e.g., 2+3): "); kreadline(buf,16);
    a=buf[0]-'0'; op=buf[1]; b=buf[2]-'0';
    switch(op){
        case '+': { char s[16]; int r=a+b; kprint("Result: "); kprintln("Sum"); break; }
        case '-': { char s[16]; int r=a-b; kprint("Result: "); kprintln("Sub"); break; }
        case '*': { char s[16]; int r=a*b; kprint("Result: "); kprintln("Mul"); break; }
        case '/': { char s[16]; int r=(b!=0)?a/b:0; kprint("Result: "); kprintln("Div"); break; }
        default: kprintln("Invalid"); break;
    }
}

// Dummy date
void cmd_date() { kprintln("NxtGen OS Date: 2026-03-22"); }

void kernel_loop() {
    char buf[16];
    while(1) {
        kprint(current_user); kprint("@NxtGenOS:$ ");
        kreadline(buf,16);

        if(kstrcmp(buf,"help")==0) cmd_help();
        else if(kstrcmp(buf,"ls")==0) cmd_ls();
        else if(buf[0]=='t' && buf[1]=='o') cmd_touch(buf+6);
        else if(buf[0]=='c' && buf[1]=='a') cmd_cat(buf+4);
        else if(kstrcmp(buf,"calc")==0) cmd_calc();
        else if(kstrcmp(buf,"date")==0) cmd_date();
        else if(kstrcmp(buf,"clear")==0) kclear();
        else if(kstrcmp(buf,"restart")==0) return;
        else if(kstrcmp(buf,"poweroff")==0) { kprintln("Shutting down"); while(1); }
        else if(kstrcmp(buf,"exit")==0) { kprintln("Bye!"); while(1); }
        else kprintln("Unknown command");
    }
}

// Entry point
void _start() {
    kclear();
    kprintln("=== Welcome to NxtGen OS ===");
    login();
    kernel_loop();
    _start(); // restart if requested
}
