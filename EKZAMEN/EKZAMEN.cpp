#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

#define MIX_NUMBER 15 /* количество перемешиваний начальной расстановки */

class game15 {
public:
    string code;
    game15() {
        createBoard();
    }
    void play() {
        bool p = true;
        string a;
        while (p) {
            while (!isDone()) {
                drawBoard();
                p = getMove();
                if (!p) break;
            }
            drawBoard();
            cout << endl << endl << "Конец игры!";
            p = false;
        }
    }
    void drawBoard() {
        int r; cout << endl << endl;
        for (int y = 0; y < 4; y++) {
            cout << "+----+----+----+----+" << endl;
            for (int x = 0; x < 4; x++) {
                r = brd[x + y * 4];
                cout << "| ";
                if (r < 10) cout << " ";
                if (!r) cout << "  ";
                else cout << dec << r << " ";
            }
            cout << "|" << endl;
        }
        cout << "+----+----+----+----+" << endl;
    }
    static void clearInput() { cin.clear(); cin.ignore(10000, '\n'); }
private:
    int brd[16], x, y;
    void createBoard() {
        vector <int> v; int i;
        for (i = 1; i < 16; i++) { brd[i - 1] = i; }
        brd[15] = 0;
        x = y = 3;
        //такое вот перемешивание из начальной позиции, чтоб решалось
        for (i = 0; i < MIX_NUMBER; i++) {
            getCandidates(v);
            move(v[rand() % v.size()]);
            v.clear();
        }
        //вычисляем код игры для решалки
        this->code = "0x";
        string hex = "0123456789abcdef";
        for (int i = 0; i < 16; i++) code += hex[this->brd[i]];
    }
    void move(int d) {
        int t = x + y * 4;
        switch (d) {
        case 1: y--; break;
        case 2: x++; break;
        case 4: y++; break;
        case 8: x--; break;
        }
        brd[t] = brd[x + y * 4];
        brd[x + y * 4] = 0;
    }
    void getCandidates(vector<int>& v) {
        if (x < 3) v.push_back(2); if (x > 0) v.push_back(8);
        if (y < 3) v.push_back(4); if (y > 0) v.push_back(1);
    }
    bool getMove() {
        vector <int> v; getCandidates(v);
        vector <int> p; getTiles(p, v); unsigned int i;
        while (true) {
            cout << endl << "Возможные ходы (или 0 для закрытия): ";
            for (i = 0; i < p.size(); i++) cout << p[i] << " ";
            int z;
            try {
                cin >> z;
                if (cin.fail()) throw invalid_argument("You need a number here, please repeat");
            }
            catch (invalid_argument& error) {
                game15::clearInput();
                cerr << endl << error.what() << endl;
            }
            if (z == 0) return false;
            for (i = 0; i < p.size(); i++) {
                if (z == p[i]) { move(v[i]); return true; }
            }
        }
    }
    void getTiles(vector<int>& p, vector<int>& v) {
        for (unsigned int t = 0; t < v.size(); t++) {
            int xx = x, yy = y;
            switch (v[t]) {
            case 1: yy--; break;
            case 2: xx++; break;
            case 4: yy++; break;
            case 8: xx--;
            }
            p.push_back(brd[xx + yy * 4]);
        }
    }
    bool isDone() {
        for (int i = 0; i < 15; i++) if (brd[i] != i + 1) return false;
        return true;
    }
};

#define BUFSIZE 128 /*размеры буферов для решалки */
class solver15 {
    const int
        Nr[16]{ 3,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3 },
        Nc[16]{ 3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2 };
    int n{}, _n{}, N0[BUFSIZE]{}, N3[BUFSIZE]{}, N4[BUFSIZE]{};
    unsigned long long N2[BUFSIZE]{};
    const bool fY() {
        if (N4[n] < _n) return fN();
        if (N2[n] == 0x123456789abcdef0) {
            cout << "Решение найдено в " << dec << n << " движется :" << endl;
            for (int g{ 1 }; g <= n; ++g) cout << (char)N3[g]; cout << endl; return true;
        }
        if (N4[n] == _n) return fN();
        else return false;
    }
    const bool fN() {
        if (N3[n] != 'u' && N0[n] / 4 < 3) { fI(); ++n; if (fY()) return true; --n; }
        if (N3[n] != 'd' && N0[n] / 4 > 0) { fG(); ++n; if (fY()) return true; --n; }
        if (N3[n] != 'l' && N0[n] % 4 < 3) { fE(); ++n; if (fY()) return true; --n; }
        if (N3[n] != 'r' && N0[n] % 4 > 0) { fL(); ++n; if (fY()) return true; --n; }
        return false;
    }
    void fI() {
        const int g = (11 - N0[n]) * 4;
        const unsigned long long a = N2[n] & ((unsigned long long)15 << g);
        N0[n + 1] = N0[n] + 4;
        N2[n + 1] = N2[n] - a + (a << 16);
        N3[n + 1] = 'd';
        N4[n + 1] = N4[n] + (Nr[a >> g] <= N0[n] / 4 ? 0 : 1);
    }
    void fG() {
        const int g = (19 - N0[n]) * 4;
        const unsigned long long a = N2[n] & ((unsigned long long)15 << g);
        N0[n + 1] = N0[n] - 4;
        N2[n + 1] = N2[n] - a + (a >> 16);
        N3[n + 1] = 'u';
        N4[n + 1] = N4[n] + (Nr[a >> g] >= N0[n] / 4 ? 0 : 1);
    }
    void fE() {
        const int g = (14 - N0[n]) * 4;
        const unsigned long long a = N2[n] & ((unsigned long long)15 << g);
        N0[n + 1] = N0[n] + 1;
        N2[n + 1] = N2[n] - a + (a << 4);
        N3[n + 1] = 'r';
        N4[n + 1] = N4[n] + (Nc[a >> g] <= N0[n] % 4 ? 0 : 1);
    }
    void fL() {
        const int g = (16 - N0[n]) * 4;
        const unsigned long long a = N2[n] & ((unsigned long long)15 << g);
        N0[n + 1] = N0[n] - 1;
        N2[n + 1] = N2[n] - a + (a >> 4);
        N3[n + 1] = 'l';
        N4[n + 1] = N4[n] + (Nc[a >> g] >= N0[n] % 4 ? 0 : 1);
    }
public:
    solver15(string gs) {
        int n = gs.substr(2).find(string("0"));
        char* e;
        unsigned long long g = strtoull(gs.c_str(), &e, 16);
        N0[0] = n;
        N2[0] = g;
    }
    void solve() { for (; not fY(); ++_n); }
};

int main() {
    setlocale(0, "");
   
    srand((unsigned)time(0));

    int menu;
    do {
        game15 p;
        cout << endl << "Новый игровой код " << hex << p.code << endl;
        p.drawBoard();
        try {
            cout << endl << "Введите 1 для воспроизведения вручную, 2 для решения, 0 для выхода: ";
            cin >> menu;
            if (cin.fail() || menu < 0 || menu>2) throw invalid_argument("Неверный ввод, пожалуйста, повторите");
            if (menu == 0) break;
            else if (menu == 1) {
                p.play(); //играем сами
            }
            else {
                solver15 s(p.code);
                s.solve(); //решает автоматически  
            }
        }
        catch (invalid_argument& error) {
            game15::clearInput();
            cerr << endl << error.what() << endl;
        }
    } while (1);

    return 0;
}