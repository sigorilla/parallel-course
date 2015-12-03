#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <omp.h>

using namespace std;

class BVP {
    int N;
    double epsilon;
    double h;
    double a_param;
    int threads;
    double *y;
    double *dy;
    double *exact;

    double *f;
    double *g;
    double *a;
    double *b;
    double *c;

    void calculate_iter(void);
    void reduction_parallel(double *a1, double *b1, double *c1, double *g1);
    void shuttle_parallel(double *a, double *b, double *c, double *g, int power);
    void visualise(void);
    void save(void);
    void print_array(double *x);
  public:
    BVP(double epsilon, int N, double a_param, double* y0, int num_threads);
    void calculate(void);
};

BVP::BVP(double epsilon, int N, double a_param, double* y0, int num_threads) {
    y = new double[N];
    dy = new double[N];

    f = new double[N];
    g = new double[N];
    a = new double[N];
    b = new double[N];
    c = new double[N];

    for (int i = 0; i < N; i++) {
        y[i] = y0[i];
    }

    dy[0] = 0;
    dy[N-1] = 0;

    this->epsilon = epsilon;
    this->N = N;
    this->a_param = a_param;
    this->threads = num_threads;

    h = 20.0 / (N - 1);
    
    omp_set_dynamic(0);
    omp_set_num_threads(this->threads);
};

void BVP::calculate() {
    bool exit = 0;
    int iteration = 1;

    while (!exit) {
        // cout << iteration << " ";

        calculate_iter();

        for (int i = 0; i < N; i++) {
            y[i] += dy[i];
        }

        exit = 1;
        for (int i = 0; i < N; i++) {
            if (fabs(dy[i]) > epsilon) {
                exit = 0;
            }
        }
        iteration += 1;
    }

    cout << "BVP iterations: " << iteration << endl;

    this->visualise();
    this->save();
};

/**
 * Calculate each iteration
 */
void BVP::calculate_iter() {
    for (int i = 0; i < N; i++) {
        f[i] = a_param * (y[i] - y[i] * y[i] * y[i]);
    }

    #pragma omp parallel for
    for (int i = 1; i < N-1; i++) {
        g[i] = (f[i] + (f[i+1] - 2 * f[i] + f[i-1])/12) * h * h - y[i+1] + 2 * y[i] - y[i-1];
        a[i] = 1 - h * h * a_param * (1 - y[i-1] * y[i-1]);
        b[i] = -2 - 10 * h * h * a_param * (1 - y[i] * y[i]) / 12;
        c[i] = 1 - h * h * a_param * (1 - y[i+1] * y[i+1]);
    }

    g[0] = 0;
    g[N-1] = 0;
    b[0] = 1;
    b[N-1] = 1;
    a[N-1] = 0;
    c[0] = 0;

    reduction_parallel(a, b, c, g);

    for (int i = 0; i < N; i++) {
        if (dy[i] != dy[i]) {
            cout << "NaN appeared" << endl;
            exit(1);
        }

        if (fabs(dy[i]) > 1e50) {
            cout << "The solution diverged" << endl;
            exit(1);
        }
    }
};

void BVP::reduction_parallel(double *a1, double *b1, double *c1, double *g1) {
    double *a2, *b2, *c2, *g2;
    double *tempa, *tempb, *tempc, *tempg;
    a2 = new double[N];
    b2 = new double[N];
    c2 = new double[N];
    g2 = new double[N];
    tempa = new double[N];
    tempb = new double[N];
    tempc = new double[N];
    tempg = new double[N];

    int power;

    for (int i = 0; i < N; i++) {
        a2[i] = a1[i];
        b2[i] = b1[i];
        c2[i] = c1[i];
        g2[i] = g1[i];
    }

    for (power = 2; power < N-1; power = power*2) {
        int delta = power/2;

        #pragma omp parallel for
        for (int i = power; i < N; i += power) {
            tempa[i] = -a2[i-delta]/b2[i-delta]*a2[i];
            tempb[i] = b2[i] - c2[i-delta]/b2[i-delta]*a2[i] - a2[i+delta]/b2[i+delta]*c2[i];
            tempc[i] = -c2[i+delta]/b2[i+delta]*c2[i];
            tempg[i] = g2[i] - g2[i-delta]/b2[i-delta]*a2[i] - g2[i+delta]/b2[i+delta]*c2[i];
        }

        #pragma omp parallel for
        for (int i = power; i < N; i += power) {
            a2[i] = tempa[i];
            b2[i] = tempb[i];
            c2[i] = tempc[i];
            g2[i] = tempg[i];
        }

        if (power == 8) {
            shuttle_parallel(a2, b2, c2, g2, power);
            break;
        }
    }

    for (power = 8; power > 1; power = power / 2) {
        int delta = power/2;

        #pragma omp parallel for
        for (int i = delta; i < N; i+= power) {
            dy[i] = (g2[i] - a2[i] * dy[i-delta] - c2[i] * dy[i+delta]) / b2[i];
        }
    }
};

void BVP::shuttle_parallel(double *a2, double *b2, double *c2, double *g2, int power) {
    double m;

    g2[0] = g2[N-1] = 0;
    b2[0] = b2[N-1] = 1;
    a2[N-1] = 0;
    c2[0] = 0;

    for (int i = power; i < N; i += power) {
        m = a2[i]/b2[i-power];
        b2[i] = b2[i] - m * c2[i-power];
        g2[i] = g2[i] - m * g2[i-power];
    }

    dy[N-1] = g2[N-1] / b2[N-1];

    for (int i = N-1-power; i >= 0; i -= power) {
        dy[i] = (g2[i]-c2[i]*dy[i+power])/b2[i];
    }
};

void BVP::print_array(double *x) {
    for (int i = 0; i < N; i++) {
        cout << setprecision(5) << x[i] << '\t';
    }
    cout << endl;
};

void BVP::visualise() {
    int skip = N / 10;

    cout << "x: ";
    for (int i = 0; i < N/skip; i++) {
        cout << fixed << setw(7) << setprecision(3) << -10 + i*skip*h << '\t';
    }
    cout << setw(7) << setprecision(3) << 10 << endl;

    cout << "y: ";
    for (int i = 0; i < N/skip; i++) {
        cout << fixed << setw(7) << setprecision(3) << y[i*skip] << '\t';
    }
    cout << fixed << setw(7) << setprecision(3) << y[N-1] << endl;
}

void BVP::save() {
    double x = -10;
    ofstream myfile;

    myfile.open("results.txt");
    if (myfile.is_open()) {
        myfile << "x: ";
        for (int i = 0; i < N; i++) {
            myfile << setw(4) << x << '\t';
            x += h;
        }
        myfile << endl << "y: ";
        for (int i = 0; i < N; i++) {
            myfile << setw(4) << y[i] << '\t';
        }
    } else {
        cout << "Failed to open the file" << endl;
        return;
    }
}

int main(int argc, char *argv[]) {
    string arg1, arg2, arg3, arg4;
    double epsilon;
    int N, a, N_TR;

    switch (argc) {
      case 5:
        arg1 = argv[1];
        arg2 = argv[2];
        arg3 = argv[3];
        arg4 = argv[4];

        epsilon = atof(arg1.c_str());
        N = atoi(arg2.c_str());
        a = atoi(arg3.c_str());
        N_TR = atoi(arg4.c_str());
        break;
      default:
        cout << "Default:" << endl;
        epsilon = 0.001;
        N = 4097;
        N_TR = 4;
        a = 100;
    }

    cout << "eps = " << epsilon << "\t";
    cout << "N = " << N << "\t";
    cout << "a = " << a << "\t";
    cout << "R_TR = " << N_TR << endl;

    double *y0;
    y0 = new double[N];
    for (int i = 0; i < N; i++) {
        y0[i] = sqrt(2);
    }

    BVP instance(epsilon, N, a, y0, N_TR);
    instance.calculate();

    return 0;
}
