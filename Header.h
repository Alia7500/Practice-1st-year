#define _CRT_SECURE_NO_WARNINGS

#ifndef header_h
#define header_h


#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <io.h>
#include <conio.h>

using namespace std;

ofstream fout;
ifstream fin;
fstream binf;

static int n = 0;
static int m = 0;
const int Count = 5;
char filename[10] = "test.txt";
char binfile[15] = "binfile.txt";
char delfile[11] = "def.txt";

struct Cities {
    char mark[15];
    char color[50];
    double price;
    int place;
    unsigned int spid;
}st;

void sorted(Cities* cities, int n);

void filtr(char* str) {
    int x;
    int xl = 0,
        xr = strlen(str) - 1;
    while (str[xl] == ' ') xl++;
    while (str[xr] == ' ') xr--;
    for (x = xl; x <= xr; x++)
        str[x - xl] = str[x];
    str[x - xl] = '\0';
}

ostream& operator << (ostream& out, const Cities& Cities) {
    out.setf(ios::left);
    out << setw(15) << st.mark << setw(13) << st.color << setw(23) << st.price << setw(24) << st.place << setw(22) << st.spid;
    out.unsetf(ios::left);
    return out;
}

istream& operator >> (istream& in, Cities& Cities) {
    char T[255];
    in.getline(st.mark, 13);
    filtr(st.mark);
    in.clear();
    in.getline(st.color, 16);
    filtr(st.color);
    in.clear();
    in >> st.price;
    in >> st.spid;
    in >> st.place;
    in.getline(T, 255);	// запись структуры целиком из ОП  в бин. файл
    return in;
}

void createFile(char* file) {
    fout.open(file);
m: cout << "Введите все поля данные для одной структуры:\n";
    cin >> st;
    fout << st;
    cout << "Для завершения нажмите '0'\nЕсли хотите продолжить, то введите любой другой симвойл:\n";
    char c;
    c = _getch();
    if (c == '0') {
        fout.close();
        return;
    }
    else {
        fout << endl;
        goto m;
    }
    fout.close();
}

void createBin(char* data, char* binfile) {

    fin.open(data);
    if (!fin.is_open()) {
        cout << "Такого файла нет!\n";
        system("pause");
        exit(0);
    }
    fout.open(binfile, ios::binary);
    while (!fin.eof()) {
        fin >> st;
        fout.write((char*)&st, sizeof(Cities));
        n++;
    }
    fout.close();
    fin.close();
}

void RemoveLine(char* filedel, const char* binfile) {
    int z = sizeof(Cities);
    int ch = 0;
    cout << "Удаление из бинарного файла" << endl;
    fin.open(filedel);
    if (!fin) {
        cout << "Error(def.txt)";
        exit(0);
    }
    binf.open(binfile, ios::in | ios::out | ios::binary);
    if (!binf) {
        cout << "Error(binf)";
        exit(0);
    }
    long size_f = 0;
    while (binf.peek() != EOF) {
        binf.write((char*)&st, z);
        size_f += z;
    }
    string data;
    vector<string> cache;
    while (!fin.eof()) {
        getline(fin, data);
        if (data.empty())  cout << "\nБыла найдена пустая строка\n";
        else cache.push_back(data);
    }
    for (auto it = cache.begin(); it != cache.end(); it++) {
        binf.clear();
        binf.seekg(0, ios::beg);
        while (binf.peek() != EOF) {
            binf.read((char*)&st, z);
            if (st.mark == *it) {
                ch++;
                while (binf.peek() != EOF) {
                    binf.read((char*)&st, z);
                    binf.seekp(-2 * z, ios::cur); //смещаем указатель файла на одну структуру назад
                    binf.write((char*)&st, z); //запись в файл откорректированной структуры
                    binf.seekp(+z, ios::cur);
                };
                int handle = _open(binfile, ios::in | ios::binary);
                size_f -= z;
                _chsize(handle, size_f);
                _close(handle);
            }
        }
        if (ch == 0) {
            cout << "\nне была удалена структура с параметром: " << *it << "\n";
        }
        else {
            ch = 0;
            cout << "\nбыла удалена структура с параметром: " << *it << "\n";
            m++;
        }
    }
    cout << "\n";
    fin.close();
    binf.close();

};

void correction(const char* binfile) {
    binf.open(binfile, ios::binary | ios::in);
    fout.open("corr.txt");
    while (!binf.eof()) {
        binf.read((char*)&st, sizeof(Cities));
        fout << st << "\n";
    }
    fout.close();
    binf.close();
}

void rbinf(char* binfile) {
    cout << "\n\nЧтение файла по ПЗ #1:\n";
    binf.open(binfile, ios::binary | ios::out);
    while (!binf.eof()) {
        binf.read((char*)&st, sizeof(Cities));
        cout << st << endl;
    }
    binf.close();
}

void sortedBinary(char* binfile, const char* bnew) {
    binf.open(binfile, ios::in | ios::binary);
    if (!binf) {
        cout << "Error(binf)";
        system("pause");
        exit(0);
    }
    int size = 0;
    while (!binf.eof()) {
        binf.read((char*)&st, sizeof(Cities));
        size++;
    }
    binf.clear();
    binf.seekg(0, ios::beg);
    int i = 0;
    Cities* cities = new Cities[size];
    while (!binf.eof()) {
        binf.read((char*)&cities[i], sizeof(Cities));
        i++;
    }
    binf.clear();
    binf.seekg(0);
    sorted(cities, size);

    fout.open(bnew, ios::binary);
    cout << "Отсортированный файл\n";
    for (int j = 0; j < size - 1; j++) {
        fout.write((char*)&cities[j], sizeof(Cities));
    }
    delete[] cities;
    fout.close();
    binf.close();
}

void sorted(Cities* cities, int n) {
    int i, j, step;
    Cities tmp;
    int stmp;
    for (step = n / 2; step > 0; step /= 2)
        for (i = step; i < n; i++) {
            tmp = cities[i];
            stmp = cities[i].spid;
            for (j = i; j >= step; j -= step) {
                if (stmp < cities[j - step].spid)
                    cities[j] = cities[j - step];
                else
                    break;
            }
            cities[j] = tmp;
        }
}

void outputMenu() {
    system("cls");
    cout << "\n                    МЕНЮ: \n"
        << "\nСоздание файла------------------------------ 1"
        << "\nЧтение файла-------------------------------- 2"
        << "\nСортировка файла---------------------------- 3"
        << "\nУдаление данных из файла-------------------- 4"
        << "\nВыход--------------------------------------- 5";
    cout << "\n\nДля выполнения операции введите нажмите нужную клавишу: ";
}

void printHeaderTable() {
    string header[5] = { " Спортивные секции ",
"|===============|=============|=======================|========================|======================| ",
"|Название секции| ФИО тренера |Длительность тренеровки|Стоимость одного занятия|  Кол-во спортсменов  | ",
"|===============|=============|=======================|========================|======================| ",
"|---------------|-------------|-----------------------|------------------------|----------------------| "
    };
    for (int i = 0; i < 5; i++) cout << header[i] << "\n";
}

void printFooterTabele() {
    cout << "|===============|=============|=======================|========================|======================|" << endl;
}

void printLineTable(Cities city) {
    cout << left << '|' << setw(15) << st.mark << '|' << setw(13) << st.color << '|' << setw(23) << st.price << '|' << setw(24) << st.place << '|' << setw(22) << st.spid << '|' << endl;
}

void CheckEdit(const char* binfile) {
    printHeaderTable();
    binf.open(binfile, ios::binary | ios::in);
    for (int i = 0; i < (n - m); i++) {
        binf.read((char*)&st, sizeof(Cities));
        printLineTable(st);
    }

    binf.close();
    printFooterTabele();
}

void Create() {
    system("cls");
    createBin(filename, binfile);
    system("pause");
    outputMenu();
}

void Read() {
    system("cls");
    CheckEdit(binfile);
    system("pause");
    outputMenu();
}

void Sorted() {
    system("cls");
    sortedBinary(binfile, "binnew.txt");
    correction("binnew.txt");
    CheckEdit("binnew.txt");
    system("pause");
    outputMenu();
}

void Remove() {
    system("cls");
    RemoveLine(delfile, "binnew.txt");
    correction("binnew.txt");
    CheckEdit("binnew.txt");
    system("pause");
    outputMenu();
}


int inputNumber() {
    int a;
    (cin >> a).get();
    return a;
}
void Exit() { exit(1); }

void(*Processing[5])() = { Create,  Read, Sorted, Remove, Exit };

#endif
