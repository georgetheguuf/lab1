
#include <iostream>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

void skipLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

struct Pipe {
    string Name = "";
    double length = 0;
    int diametr = 0;
    bool isRepair = false;
};

void inputPipe(Pipe& pipe) {
    cout << "\nНазвание: ";
    getline(cin >> ws, pipe.Name);

    cout << "Длина (км): ";
    while (!(cin >> pipe.length) || pipe.length <= 0 || cin.peek() != '\n') {
        cout << "Длина должна быть положительна. Повторите: ";
        skipLine();
    }

    cout << "Диаметр (мм): ";
    while (!(cin >> pipe.diametr) || pipe.diametr <= 0 || cin.peek() != '\n') {
        cout << "Диаметр должен быть положительным. Повторите: ";
        skipLine();
    }

    pipe.isRepair = false;
}

void displayPipe(const Pipe& pipe) {
    cout << "--- Труба ---\n";
    cout << "Название: " << pipe.Name << "\n";
    cout << "Длина: " << pipe.length << " км\n";
    cout << "Диаметр: " << pipe.diametr << " мм\n";
    cout << "Статус: " << (pipe.isRepair ? "в ремонте" : "в работе") << "\n";
}

void setPipeRepair(Pipe& pipe) {
    pipe.isRepair = true;
    cout << "Труба переведена в ремонт.\n";
}

void setPipeWorking(Pipe& pipe) {
    pipe.isRepair = false;
    cout << "Труба введена в работу.\n";
}

void togglePipeRepair(Pipe& pipe) {
    pipe.isRepair = !pipe.isRepair;
    cout << "Новый статус: " << (pipe.isRepair ? "в ремонте" : "в работе") << "\n";
}
struct KC {
    string Name = "";
    int amountFactories = 0;
    int workingFactories = 0;
    int levelFactory = 0;
};

void inputKC(KC& kc) {
    cout << "\nНазвание КС: ";
    getline(cin >> ws, kc.Name);

    cout << "Кол-во цехов: ";
    while (!(cin >> kc.amountFactories) || kc.amountFactories <= 0 || cin.peek() != '\n') {
        cout << "Кол-во должно быть положительным. Повторите: ";
        skipLine();
    }

    cout << "Кол-во цехов в работе (0.." << kc.amountFactories << "): ";
    while (!(cin >> kc.workingFactories) ||
           kc.workingFactories < 0 ||
           kc.workingFactories > kc.amountFactories ||
           cin.peek() != '\n') {
        cout << "Значение должно быть от 0 до " << kc.amountFactories << ". Повторите: ";
        skipLine();
    }

    cout << "Класс станции: ";
    while (!(cin >> kc.levelFactory) || kc.levelFactory <= 0 || cin.peek() != '\n') {
        cout << "Класс станции должен быть положительным. Повторите: ";
        skipLine();
    }
}
void displayKC(const KC& kc) {
    cout << "--- КС ---\n";
    cout << "Название: " << kc.Name << "\n";
    cout << "Всего цехов: " << kc.amountFactories << "\n";
    cout << "Цехов в работе: " << kc.workingFactories
         << " / " << kc.amountFactories << "\n";
    cout << "Класс станции: " << kc.levelFactory << "\n";
}
bool startShop(KC& kc) {
    if (kc.workingFactories >= kc.amountFactories) return false;
    ++kc.workingFactories;
    return true;
}
bool stopShop(KC& kc) {
    if (kc.workingFactories <= 0) return false;
    --kc.workingFactories;
    return true;
}
struct AppState {
    Pipe pipe;
    KC kc;
    bool pipeExists = false;
    bool kcExists = false;
};

void savePipe(ofstream& out, const Pipe& pipe) {
    out << "PIPE\n" << pipe.Name << '\n'
        << pipe.length << '\n' << pipe.diametr << '\n'
        << pipe.isRepair << '\n';
}
void saveKC(ofstream& out, const KC& kc) {
    out << "KC\n" << kc.Name << '\n'
        << kc.amountFactories << '\n'
        << kc.workingFactories << '\n'
        << kc.levelFactory << '\n';
}
bool saveAll(const AppState& state, const string& filename) {
    if (!state.pipeExists && !state.kcExists) {
        cout << "Нечего сохранять.\n";
        return false;
    }
    ofstream out(filename);
    if (!out) {
        cout << "Ошибка открытия файла для записи.\n";
        return false;
    }
    if (state.pipeExists) savePipe(out, state.pipe);
    if (state.kcExists) saveKC(out, state.kc);

    if (!out) {
        cout << "Ошибка записи в файл.\n";
        return false;
    }

    cout << "Данные сохранены в " << filename << "\n";
    return true;
}
bool loadPipe(ifstream& in, Pipe& pipe) {
    Pipe temp;
    int repair;

    in >> ws;
    getline(in, temp.Name);

    if (!(in >> temp.length >> temp.diametr >> repair)) return false;

    if (temp.Name.empty() || temp.length <= 0 ||
        temp.diametr <= 0 || (repair != 0 && repair != 1)) {
        cout << "Некорректные данные трубы.\n";
        return false;
    }

    temp.isRepair = repair;
    pipe = temp;
    return true;
}

bool loadKC(ifstream& in, KC& kc) {
    KC temp;

    in >> ws;
    getline(in, temp.Name);

    if (!(in >> temp.amountFactories >> temp.workingFactories
             >> temp.levelFactory)) return false;

    if (temp.Name.empty() || temp.amountFactories <= 0 ||
        temp.levelFactory <= 0 || temp.workingFactories < 0 ||
        temp.workingFactories > temp.amountFactories) {
        cout << "Некорректные данные КС.\n";
        return false;
    }

    kc = temp;
    return true;
}
bool loadAll(AppState& state, const string& filename) {
    ifstream in(filename);
    if (!in) {
        cout << "Файл не найден.\n";
        return false;
    }

    Pipe tempPipe;
    KC tempKC;
    bool pipeLoaded = false;
    bool kcLoaded = false;
    string tag;

    while (in >> tag) {
        if (tag == "PIPE") {
            if (pipeLoaded || !loadPipe(in, tempPipe)) {
                cout << "Ошибка загрузки трубы.\n";
                return false;
            }
            pipeLoaded = true;
        }
        else if (tag == "KC") {
            if (kcLoaded || !loadKC(in, tempKC)) {
                cout << "Ошибка загрузки КС.\n";
                return false;
            }
            kcLoaded = true;
        }
        else {
            cout << "Неизвестная запись в файле.\n";
            return false;
        }
    }

    if (in.bad() || (!pipeLoaded && !kcLoaded)) {
        cout << "Не удалось загрузить данные.\n";
        return false;
    }

    if (pipeLoaded) {
        state.pipe = tempPipe;
        state.pipeExists = true;
    }

    if (kcLoaded) {
        state.kc = tempKC;
        state.kcExists = true;
    }

    cout << "Данные загружены.\n";
    return true;
}
void displayMenu() {
    cout << "\n========== Трубопровод ==========\n";
    cout << "1. Ввод трубы\n";
    cout << "2. Ввод КС\n";
    cout << "3. Просмотр\n";
    cout << "4. Редактировать трубу\n";
    cout << "5. Редактировать КС\n";
    cout << "6. Сохранить всё в файл\n";
    cout << "7. Загрузить всё из файла\n";
    cout << "0. Выход\n";
    cout << "=================================\n";
    cout << "Выберите действие: ";
}
void handleAddPipe(AppState& state) {
    if (state.pipeExists) {
        cout << "Труба уже есть. Перезаписать? (y/n): ";
        char ans;
        cin >> ans;
        skipLine();
        if (ans != 'y' && ans != 'Y') return;
    }

    inputPipe(state.pipe);
    state.pipeExists = true;
    cout << "Труба сохранена в памяти.\n";
}
void handleAddKC(AppState& state) {
    if (state.kcExists) {
        cout << "КС уже есть. Перезаписать? (y/n): ";
        char ans;
        cin >> ans;
        skipLine();
        if (ans != 'y' && ans != 'Y') return;
    }

    inputKC(state.kc);
    state.kcExists = true;
    cout << "КС сохранена в памяти.\n";
}
void handleView(const AppState& state) {
    cout << "\n=== Текущее состояние ===\n";
    if (state.pipeExists) displayPipe(state.pipe);
    else cout << "Трубы нет.\n";

    if (state.kcExists) displayKC(state.kc);
    else cout << "КС нет.\n";
    cout << "=========================\n";
}
void handleEditPipe(AppState& state) {
    if (!state.pipeExists) {
        cout << "Сначала создайте трубу.\n";
        return;
    }

    displayPipe(state.pipe);
    cout << "1. В ремонт\n2. В работу\n3. Переключить\n0. Отмена\n";
    cout << "Выбор: ";

    int sub;
    if (!(cin >> sub)) {
        skipLine();
        cout << "Некорректный ввод.\n";
        return;
    }
    skipLine();

    switch (sub) {
        case 1: setPipeRepair(state.pipe); break;
        case 2: setPipeWorking(state.pipe); break;
        case 3: togglePipeRepair(state.pipe); break;
        case 0: break;
        default: cout << "Нет такого пункта.\n"; break;
    }
}
void handleEditKC(AppState& state) {
    if (!state.kcExists) {
        cout << "Сначала создайте КС.\n";
        return;
    }

    displayKC(state.kc);
    cout << "1. Запустить цех\n2. Остановить цех\n0. Отмена\n";
    cout << "Выбор: ";

    int sub;
    if (!(cin >> sub)) {
        skipLine();
        cout << "Некорректный ввод.\n";
        return;
    }
    skipLine();

    switch (sub) {
        case 1:
            if (startShop(state.kc))
                cout << "Цех запущен. В работе: " << state.kc.workingFactories << "\n";
            else
                cout << "Все цехи уже в работе.\n";
            break;
        case 2:
            if (stopShop(state.kc))
                cout << "Цех остановлен. В работе: " << state.kc.workingFactories << "\n";
            else
                cout << "Все цехи уже остановлены.\n";
            break;
        case 0: break;
        default: cout << "Нет такого пункта.\n"; break;
    }
}
int main() {
    AppState state;
    const string fileName = "pipeline_data.txt";

    bool running = true;

    while (running) {
        displayMenu();

        int choice;
        if (!(cin >> choice) || cin.peek() != '\n') {
            cout << "Некорректный ввод.\n";
            skipLine();
            continue;
        }
        skipLine();

        switch (choice) {
            case 0:
                cout << "Выход.\n";
                running = false;
                break;
            case 1: handleAddPipe(state); break;
            case 2: handleAddKC(state); break;
            case 3: handleView(state); break;
            case 4: handleEditPipe(state); break;
            case 5: handleEditKC(state); break;
            case 6: saveAll(state, fileName); break;
            case 7: loadAll(state, fileName); break;
            default: cout << "Нет такого пункта.\n"; break;
        }
    }

    return 0;
}
