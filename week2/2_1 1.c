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

void printData(struct student p){
    printf("Name: %s, Eng: %d, Math: %d, Phys: %d, Mean: %.2f \n",
            p.name, p.eng,p.math, p.phys, p.mean);
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
printData(data[i]);
}

return 0;

}
