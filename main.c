#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

// проверка на простое число
int is_prime(unsigned long n) {
    if (n == 2) return 1;
    for (unsigned long div = 2; div <= sqrt(n) + 1; div++) {
        if (n % div == 0) return 0;
    }
    return 1;
}

// проверка на взаимную простоту
int is_relatively_prime(unsigned long a, unsigned long b) {
    while ((a != 0) && (b != 0)) {
        if (a > b) a %= b;
        else b %= a;
    }
    if (a + b == 1) return 1;
    else return 0;
}

// ЛКГ
unsigned long lkg(unsigned long x) {
    unsigned long m = 4294967296;
    unsigned long a = 53949677;
    unsigned long c = 47971;
    return (a * x + c) % m;
}

// определение мощности ЛКГ
int find_power(unsigned long a, unsigned long m){
    unsigned long long n = 1;
    unsigned long b = a - 1;
    int i = 0;
    for (int s = 1; s < 10000; s++) {
        i++;
        n *= b;
        if (n % m == 0) {
            printf("Количество итераций: %d", i);
            return s;
        }
    }
    return -1;
}

// определение периода ЛКГ
unsigned long find_period(unsigned long seed) {
    unsigned long u1 = seed;
    unsigned long u2 = lkg(seed);
    while (u1 != u2) {
        u1 = lkg(u1);
        u2 = lkg(lkg(u2));
    }
    u1 = lkg(u1);
    unsigned long period = 1;
    //printf("U2 = %u\n", u2);
    //printf("U1 = %u\n", u1);
    while (u1 != u2) {u1 = lkg(u1); period++;}
    return period;
}

// проверка ЛКГ по критерию хи-квадрат
double chi_square_criterion(unsigned long m, unsigned long seed) {
    unsigned long x = seed;
    int y[21], k = 21;
    for (int i = 0; i < k; i++) y[i] = 0;
    double d = m / k + 1;
    int n = 10000;
    for (int i = 0; i < n; i++){
        x = lkg(x);
        y[x / (int) d] += 1;
    }
    double p = 1.0 / k; // вероятность попадания в каждый интервал одинакова и равна 1/t!
    // применим критерий хи-квадрат
    double V = 0;
    for (int s = 0; s < k; s++) V += (y[s] * y[s]) / p;
    V = V / n - n;
    return V;
}

// проверка ЛКГ по критерию перестановок
double permutation_criterion(unsigned long seed) {
    unsigned long u[10000][4], x, buf;
    x = seed;
    int n = 10000, t = 4;
    // заполним n = 30 групп по t = 5 элементов
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < t; j++) {
            x = lkg(x);
            u[i][j] = x;
        }
    }
    // вычислим для каждой группы значение f
    int f[10000], r, max_j;
    for (int i = 0; i < n; i++) {
        r = t;
        f[i] = 0;
        while (r > 1) {
            // ищем индекс максимума u[i][0:r]
            max_j = 1;
            for (int j = 1; j < r; j++) {
                if (u[i][j] > u[i][max_j - 1]) {
                    max_j = j + 1;
                }
            }
            // изменяем f
            f[i] = r * f[i] + max_j - 1;
            // меняем местами u[i][r] и u[i][max_j]
            buf = u[i][max_j - 1];
            u[i][max_j - 1] = u[i][r - 1];
            u[i][r - 1] = buf;

            r -= 1;
        }
    }
    int y[24]; // массив длиной t!, содержащий количество различных комбинаций
    for (int i = 0; i < 24; i++) y[i] = 0;
    for (int i = 0; i < n; i++) y[f[i]] += 1;
    double p = 1.0 / 24; // вероятность каждой комбинации одинакова и равна 1/t!
    // применим критерий хи-квадрат
    double V = 0;
    for (int s = 0; s < 24; s++) V += (y[s] * y[s]) / p;
    V = V / n - n;
    return V;
}

int main() {
    // Size of int is 32 bits
    // Size of long is 64 bits
    // Size of long-long is 64 bits

    printf("----------------------- Выбор параметров -----------------------\n");
    // генерация начального члена
    unsigned long x0;
    srand(time(NULL));
    x0 = time(NULL);
    printf("Начальный член: %lu\n", x0);


    // выбор параметров ЛКГ
    unsigned long m = 4294967296;
    unsigned long a = 53949677;
    unsigned long c = 47971;
    printf("модуль m: %lu, m простое: %d\n", m, is_prime(m));
    printf("приращение c: %lu\n", c);
    printf("множитель a: %lu\n", a);

    printf("a mod 8 == %lu\n", a % 8);

    // Теорема: 1) c и m - взаимно простые числа
    // 2) (a - 1) кратно любому простому p, на которое делится m
    // 3) (a - 1) кратно 4, если m кратно 4

    if (is_relatively_prime(c, m)) printf("c и m - взаимно простые\n");
    else printf("c и m - не взаимно простые\n");
    if (is_relatively_prime(a, m)) printf("a и m - взаимно простые\n\n");
    else printf("a и m - не взаимно простые\n\n");

    // определение мощности
    printf("----------------------- Определение мощности -----------------------\n");
    printf("Мощность ЛКГ = %d\n\n", find_power(a, m));

    // определение периода
    printf("----------------------- Определение периода -----------------------\n");
    //printf("Период ЛКГ = %lu\n\n", find_period(x0));


    // проверка по хи-квадрат

    int bad1 = 0; // недостаточно случайные
    int susp1 = 0; // подозрительные
    int almost_susp1 = 0; // почти подозрительные
    int ok1 = 0; //  удовлетворительные
    int bad2 = 0;
    int susp2 = 0;
    int almost_susp2 = 0;
    int ok2 = 0;
    double V1, V2;
    for (int i = 0; i < 100; i++) {
        x0 = rand();
        V1 = chi_square_criterion(m, x0);
        V2 = permutation_criterion(x0);

        if (V1 <= 12.44 || V1 >= 28.41) {
            if (V1 <= 10.85 || V1 >= 31.41) {
                if (V1 <= 8.26 || V1 >= 37.57) bad1 += 1;
                else susp1 += 1;
            } else almost_susp1 += 1;
        }
        else ok1 += 1;

        if (V2 <= 14.85 || V2 >= 32.01) {
            if (V2 <= 13.09 || V2 >= 35.17) {
                if (V2 <= 10.19 || V2 >= 41.63) bad2 += 1;
                else susp2 += 1;
            } else almost_susp2 += 1;
        }
        else ok2 += 1;

    }
    printf("----------------------- Критерий хи-квадрат -----------------------");
    printf("\nПроверка разброса по табличным значениям хи-квадрат: (100 независимых наблюдений)\n"
           "Отброшенные как недостаточно случайные значения (<1%% или >99%%): %d знач.\n"
           "Подозрительные значения: %d знач.\n"
           "Почти подозрительные значения: %d знач.\n"
           "Удовлетворительные значения: %d знач.\n\n", bad1, susp1, almost_susp1, ok1);
    printf("----------------------- Критерий перестановок -----------------------");
    printf("\nПроверка перестановок чисел по табличным значениям хи-квадрат: (100 независимых наблюдений)\n"
           "Отброшенные как недостаточно случайные значения (<1%% или >99%%): %d знач.\n"
           "Подозрительные значения: %d знач.\n"
           "Почти подозрительные значения: %d знач.\n"
           "Удовлетворительные значения: %d знач.\n", bad2, susp2, almost_susp2, ok2);

    return 0;
}
