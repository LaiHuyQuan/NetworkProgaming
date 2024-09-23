#include <stdio.h>

typedef struct student {
    char name[20];
    int eng;  
    int math; 
    int phys; 
} STUDENT;

int main() {
    STUDENT data[] = {
        {"Tuan", 82, 72, 58},
        {"Nam", 77, 82, 79},
        {"Khanh", 52, 62, 39},
        {"Phuong", 61, 82, 88}
    };
    
    STUDENT *p = data;
    
    int n = sizeof(data) / sizeof(data[0]);

    for (int i = 0; i < n; i++) {
        printf("Name: %s, English: %d, Math: %d, Physics: %d\n", 
               (p + i)->name, (p + i)->eng, (p + i)->math, (p + i)->phys);
    }

    return 0;
}
