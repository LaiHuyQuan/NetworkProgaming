#include <stdio.h>
#include <string.h>

struct student
{
    char name[20];
    int eng;
    int math;
    int phys;
    double mean;
};


void CalMean(struct student *p){
    p -> mean = (p -> math + p -> phys + p -> eng) / 3.0;
}

void PrintGrade(struct student p){
    char Grade[2] = "D";
    if (p.mean < 60) {
        strcpy(Grade, "D");
    }
    else if (p.mean >= 60 && p.mean < 70) {
        strcpy(Grade, "C");
    }
    else if (p.mean >= 70 && p.mean < 80) {
        strcpy(Grade, "B");
    }
    else if (p.mean >= 80 && p.mean < 90) {
        strcpy(Grade, "A");
    }
    else if (p.mean >= 90 && p.mean <= 100) {
        strcpy(Grade, "S");
    }

    printf("Name: %s, Mean: %.2f, Grade: %s\n", p.name, p.mean, Grade);
}

int main(){

static struct student data[]= {
    {"Tuan", 82, 72, 58, 0.0},
    {"Nam", 77, 82, 79, 0.0},
    {"Khanh", 52, 62, 39, 0.0},
    {"Phuong", 61, 82, 88, 0.0}  
};

int n = sizeof(data)/ sizeof(data[0]);

for (int i = 0; i < n; i++){
CalMean(&data[i]);
}

for (int i = 0; i < n; i++){
PrintGrade(data[i]);
}

return 0;

}
