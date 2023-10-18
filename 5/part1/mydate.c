#include <stdio.h>
#include <time.h>

int main() {
    time_t t;
    struct tm *timeInfo;
    char MY_TIME[50];
    
    time(&t);
    timeInfo = localtime(&t);
    strftime(MY_TIME, sizeof(MY_TIME), "%b %d(%a), %Y %I:%M %p", timeInfo);
    printf("%s\n", MY_TIME);

    return 0;
}
