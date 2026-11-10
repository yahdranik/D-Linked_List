
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE* script = fopen("histogram.gnuplot", "w");

    if (script == NULL)
    {
        printf("Cannot create file\n");
        return 1;
    }

    fprintf(script,
        "set terminal pngcairo size 1200,700\n"
        "set grid\n"
        "set xlabel 'Number of operations'\n"
        "set ylabel 'Time'\n"
        "set xrange [0:50000]\n"
        "set yrange [0:1000]\n"
        "set boxwidth 80 absolute\n"
        "set style fill solid 0.5 border\n"
        "set title 'Quick list'\n"
        "set output 'QUICK_LIST/quick_histogram.png'\n"
        "plot 'QUICK_LIST/quick_list.txt' using ($0*100):1 with boxes notitle\n"
        "set title 'Slowly list'\n"
        "set output 'SLOWLY_LIST/slowly_histogram.png'\n"
        "plot 'SLOWLY_LIST/slowly_list.txt' using ($0*100):1 with boxes notitle\n");

    fclose(script);

    if (system("gnuplot histogram.gnuplot") != 0)
    {
        printf("Gnuplot failed\n");
        return 1;
    }
}
