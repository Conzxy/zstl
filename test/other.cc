#include <stdio.h>

int main(){
    int i = 2;
    int *p = &i;
    char* pc = (char*)p;
    if(p == pc) printf("p == pc\n"); 
}