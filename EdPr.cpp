#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <locale.h>
#include "plant.h"

using namespace std;
const int NUM_CONTROL_CHANNELS_MAX = 100;

/*
    num_control_channels - k – число используемых каналов управления;
    num_inter_sensors - F – число датчиков, опрашиваемых на объекте;
    num_sens_measure - H - номер датчика (канала опроса), по которому будет измеряться отклик (реакция объекта)
    на поданные управляющие воздействия;
    num_control_cycles - N - число тактов управления (продолжительность управления)
    control_chan_nums - L1, L2, …, Lk – номера каналов управления;
    sensors_nums - M1, M2, …., MF – номера датчиков (каналов опроса);
    coefs_resp_pred_eq - А0, А1, А2,…, Аk+F – коэффициенты уравнения прогноза отклика;

    values_control_actions - U1, U2, …, Uk - значения управляющих воздействий;
    X1, X2,…,XF - соответствующие измеренные значения после опроса каналов M1, M2,…., MF;
    Y1pr - прогнозируемое значение отклика;

*/

void input(int& num_control_channels, int& num_inter_sensors, int& num_sens_measure, int& num_control_cycles, int& S,
    vector <int>& control_chan_nums, vector <int>& sensors_nums,
    vector <double>& coefs_resp_pred_eq)
{
    cout << "Введите число используемых каналов управления(k): ";
    cin >> num_control_channels;
    control_chan_nums.reserve(num_control_channels);    
    for (int i = 0; i < num_control_channels; i++)
    {
        int r;
        cout << "Введите номера каналов управления(L" << i + 1 << "): ";
        cin >> r;
        control_chan_nums.push_back(r);
    }
    cout << "Введите число датчиков, опрашиваемых на объекте(F): ";
    cin >> num_inter_sensors;
    sensors_nums.reserve(num_inter_sensors);
    S = 1 + num_control_channels + num_inter_sensors;
    coefs_resp_pred_eq.reserve(S);
    for (int i = 0; i < num_inter_sensors; i++)
    {
        int r;
        cout << "Введите номера датчиков(M" << i + 1 << "): ";
        cin >> r;
        sensors_nums.push_back(r);
    }
    cout << "Введите номер датчика, по которому будет измеряться отклик на поданные управляющие воздействия(H): ";
    cin >> num_sens_measure;
    cout << "Введите число тактов управления(N): ";
    cin >> num_control_cycles;

    for (int i = 0; i < S; i++)
    {
        double r;
        cout << "Введите коэффициенты уравнения прогноза отклика(A" << i << "):";
        cin >> r;
        coefs_resp_pred_eq.push_back(r);
    }
}


//Операция 5
void calc_resp_forecast(vector<double>& coefs_resp_pred_eq, int& num_control_channels, vector <double>& values_control_actions,
                        vector<double>& X, int& num_control_cycles, int& S, vector<double>& Ypr)
{
    for (int t = 0; t < num_control_cycles; t++)
    {
        Ypr[t] = coefs_resp_pred_eq[0];
        for (int i = 1; i < S; i++)
        {
            if (i <= num_control_channels)
            {
                Ypr[t] += coefs_resp_pred_eq[i] * values_control_actions[i - 1];
            }
            else
            {
                Ypr[t] += coefs_resp_pred_eq[i] * X[i - num_control_channels - 1];
            }
        }
    }
}


void research(int& num_control_cycles, int& S, int& num_control_channels, int& num_sens_measure, int& num_inter_sensors,
             vector <double>& values_control_actions, vector<double>& coefs_resp_pred_eq, vector<double>& X,
             vector<double>& Ypr, vector<double>& Y, vector<double>& C, int& A, int& counter, double &Amin, double &Amax, double &dA)
{
    
    cout << "От какого коэффициента исследовать зависимость, А0 или А1? ";
    cin >> A;
    cout << "Введите Amin: ";
    cin >> Amin;
    cout << "Введите Amax: ";
    cin >> Amax;
    cout << "Введите шаг dA: ";
    cin >> dA;
    counter = 0;
    
        switch (A)
        {
        case 0:
        {
            for (double i = Amin; i <= Amax; i += dA)
            {
                coefs_resp_pred_eq[0] = i;
                calc_resp_forecast(coefs_resp_pred_eq, num_control_channels, values_control_actions, X, num_control_cycles, S, Ypr);
                counter += 1;
            }

            break;
        }
        case 1:
        {
            for (double i = Amin; i <= Amax; i += dA)
            {
                coefs_resp_pred_eq[1] = i;
                calc_resp_forecast(coefs_resp_pred_eq, num_control_channels, values_control_actions, X, num_control_cycles, S, Ypr);
                counter += 1;
            }
            break;

        }
        }
}


int main()
{
    setlocale(LC_ALL, "Russian");

    Plant plant;
    plant_init(plant);

    int num_control_channels, num_inter_sensors, num_sens_measure, num_control_cycles;
    int S;
    double Amin = 0, Amax = 0, dA = 0;
    vector <int> control_chan_nums, sensors_nums;
    vector <double> coefs_resp_pred_eq;
    input(num_control_channels, num_inter_sensors, num_sens_measure, num_control_cycles, S, control_chan_nums,
        sensors_nums, coefs_resp_pred_eq);

    vector<double> Ypr(num_control_cycles);
    vector<double> Y(num_control_cycles);

    int counter = 0;
    vector<double> C(counter);
    int A = 0;
    //Операция 8
    for (int j = 0; j < num_control_cycles; j++)
    {
        //Операция 2: Ввести выбранные пользователем программы k значений управляющих воздействий: U1, U2, …, Uk.
        vector<double> values_control_actions(num_control_channels);
        for (int i = 0; i < num_control_channels; i++)
        {
            cout << "Введите значения управлющих воздействий(U" << i + 1 << "): ";
            cin >> values_control_actions[i];
        }

        //Операция 3: Установить значение U1 в канале управления с номером L1, значение U2 – в канале L2, …., значение Uk – в канале Lk.
        for (int i = 0; i < num_control_channels; i++)
        {
            plant_control(control_chan_nums[i], values_control_actions[i], plant);
        }
        //Операция 4: Последовательно произвести опрос каналов M1, M2,…., MF, запомнить соответствующие измеренные значения X1, X2,…,XF.
        vector <double> X(num_inter_sensors);
        for (int i = 0; i < num_inter_sensors; i++)
        {
            X[i] = plant_measure(sensors_nums[i], plant);
        }

        //Операция 5: Вычислить прогнозируемое значение отклика по формуле Y1пр = A0+ A1*U1+ A2*U2+…+ Ak*Uk+ Ak+1*X1+ Ak+2*X2+…+ Ak+F*XF
        calc_resp_forecast(coefs_resp_pred_eq, num_control_channels, values_control_actions, X, num_control_cycles, S, Ypr);

        //Операция 6: Произвести опрос датчика с номером Н и запомнить измеренное значение отклика Y1.
        Y[j] = plant_measure(num_sens_measure, plant);


        research(num_control_cycles, S, num_control_channels, num_sens_measure, num_inter_sensors,
            values_control_actions, coefs_resp_pred_eq, X, Ypr, Y, C, A, counter, Amin, Amax, dA);


        for (int i = 0; i < counter; i++) {
            C.push_back(pow(Y[j] - Ypr[i], 2));
        }

        for (int i = 0; i < counter; i++)
        {
            C[i] = C[i] / num_control_cycles;
        }

        switch (A)
        {
        case 0:
        {

            int j = 0;
            cout << "№\t" << "A0\t" << "C" << endl;
            for (double i = Amin; i <= Amax; i += dA) {
                if (j < counter) {
                    cout << j + 1 << "\t";
                    cout << i << "\t";
                    cout << C[j] << endl;
                    j++;
                }
            }
            break;
        }
        case 1:
        {

            int j = 0;
            cout << "№\t" << "A1\t" << "C" << endl;
            for (double i = Amin; i <= Amax; i += dA) {
                if (j < counter) {
                    cout << j + 1 << "\t";
                    cout << i << "\t";
                    cout << C[j] << endl;
                    j++;
                }
            }
            break;
        }
        }
        C.clear();
    }



    
    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
