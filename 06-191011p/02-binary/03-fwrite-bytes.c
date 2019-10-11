#include <stdio.h>
#include <assert.h>

typedef struct point {
    int x;
    int y;
} point;

int main() {
    point pts[2];
    pts[0].x = 10;
    pts[0].y = 20;
    pts[1].x = 30;
    pts[1].y = 40;
    FILE *f = fopen("03-fwrite-demo.bin", "wb");
    assert(f);
    char data[] = {
         pts[0].x & 0xFF,
         (pts[0].x >> 8) & 0xFF,
         (pts[0].x >> 16) & 0xFF,
         (pts[0].x >> 24) & 0xFF,
         pts[0].y & 0xFF,
         (pts[0].y >> 8) & 0xFF,
         (pts[0].y >> 16) & 0xFF,
         (pts[0].y >> 24) & 0xFF,
    };
    assert(fwrite(data, sizeof(char), 8, f) == 8);
    fclose(f);
}

