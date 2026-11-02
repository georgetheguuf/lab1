#include <iostream>
#include <fstream>
#include <string>

using namespace std;
void skipLine() {
    cin.clear();
    cin.ignore(100000, '\n');
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
    while (!(cin >> pipe.length) || pipe.length <= 0) {
        cout << "Длина должна быть положительна. Повторите: ";
        skipLine();
    }

    cout << "Диаметр (мм): ";
    while (!(cin >> pipe.diametr) || pipe.diametr <= 0) {
        cout << "Диаметр должен быть положительным. Повторите: ";
        skipLine();
    }

    pipe.isRepair = false;
}
void displayPipe(const Pipe& pipe) {
    cout << "--- Труба ---\n";
    cout << "Название: " << pipe.Name << "\n";
    cout << "Длина:    " << pipe.length << " км\n";
    cout << "Диаметр:  " << pipe.diametr << " мм\n";
    cout << "Статус:   " << (pipe.isRepair ? "в ремонте" : "в работе") << "\n";
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
    while (!(cin >> kc.amountFactories) || kc.amountFactories <= 0) {
        cout << "Кол-во должно быть положительным. Повторите: ";
        skipLine();
    }

    cout << "Кол-во цехов в работе (0.." << kc.amountFactories << "): ";
    while (!(cin >> kc.workingFactories) ||
           kc.workingFactories < 0 ||
           kc.workingFactories > kc.amountFactories) {
        cout << "Значение должно быть от 0 до " << kc.amountFactories
             << ". Повторите: ";
        skipLine();
    }

    cout << "Класс станции: ";
    while (!(cin >> kc.levelFactory) || kc.levelFactory <= 0) {
        cout << "Класс станции должен быть положительным. Повторите: ";
        skipLine();
    }
}

void displayKC(const KC& kc) {
    cout << "--- КС ---\n";
    cout << "Название:        " << kc.Name << "\n";
    cout << "Всего цехов:     " << kc.amountFactories << "\n";
    cout << "Цехов в работе:  " << kc.workingFactories
         << " / " << kc.amountFactories << "\n";
    cout << "Класс станции:   " << kc.levelFactory << "\n";
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
    KC   kc;
    bool pipeExists = false;
    bool kcExists   = false;
};
void savePipe(ofstream& out, const Pipe& pipe) {
    out << "PIPE\n"
        << pipe.Name << '\n'
        << pipe.length << '\n'
        << pipe.diametr << '\n'
        << pipe.isRepair << '\n';
}

void saveKC(ofstream& out, const KC& kc) {
    out << "KC\n"
        << kc.Name << '\n'
        << kc.amountFactories << '\n'
        << kc.workingFactories << '\n'
        << kc.levelFactory << '\n';
}

bool saveAll(const AppState& state, const string& filename) {
    ofstream out(filename);
    if (!out) {
        cout << "Ошибка открытия файла для записи.\n";
        return false;
    }
    cout << "[debug] saving to: " << filename << "\n";
    if (state.pipeExists) savePipe(out, state.pipe);
    if (state.kcExists)   saveKC(out, state.kc);

    if (!state.pipeExists && !state.kcExists) {
        cout << "Нечего сохранять (оба объекта пусты).\n";
        return false;
    }

    cout << "Данные сохранены в " << filename << "\n";
    return true;
}
int loadPipe(ifstream& in, Pipe& pipe) {
    string tag;
    if (!(in >> tag)) return 0;
    if (tag != "PIPE") return 0;

    Pipe temp;
    in >> ws;
    getline(in, temp.Name);
    if (!(in >> temp.length >> temp.diametr >> temp.isRepair)) return -1;

    if (temp.Name.empty()) {
        cout << "Ошибка: имя трубы пустое.\n";
        return -1;
    }
    if (temp.length <= 0 || temp.diametr <= 0) {
        cout << "Ошибка: некорректные данные трубы (длина/диаметр <= 0).\n";
        return -1;
    }

    pipe = temp;
    return 1;
}

int loadKC(ifstream& in, KC& kc) {
    string tag;
    if (!(in >> tag)) return 0;
    if (tag != "KC") return 0;

    KC temp;
    in >> ws;
    getline(in, temp.Name);
    if (!(in >> temp.amountFactories >> temp.workingFactories >> temp.levelFactory))
        return -1;

    if (temp.Name.empty()) {
        cout << "Ошибка: имя КС пустое.\n";
        return -1;
    }
    if (temp.amountFactories <= 0 || temp.levelFactory <= 0 ||
        temp.workingFactories < 0 ||
        temp.workingFactories > temp.amountFactories) {
        cout << "Ошибка: некорректные данные КС.\n";
        return -1;
    }

    kc = temp;
    return 1;
}

bool loadAll(AppState& state, const string& filename) {
    ifstream in(filename);
    if (!in) {
        cout << "Файл не найден.\n";
        return false;
    }
    cout << "[debug] loading from: " << filename << "\n";
    bool pipeLoaded = false;
    bool kcLoaded   = false;
    string tag;
    while (in >> tag) {
        if (tag == "PIPE") {
            in.putback('\n');           // вернём перевод строки для getline
            in.seekg(-(streamoff)tag.size(), ios::cur); // вернёмся к началу тега
            int r = loadPipe(in, state.pipe);
            if (r == 1) pipeLoaded = true;
            if (r == -1) {
                cout << "Загрузка прервана: ошибка в данных трубы.\n";
                return false;
            }
        } else if (tag == "KC") {
            in.seekg(-(streamoff)tag.size(), ios::cur);
            int r = loadKC(in, state.kc);
            if (r == 1) kcLoaded = true;
            if (r == -1) {
                cout << "Загрузка прервана: ошибка в данных КС.\n";
                return false;
            }
        }
    }

    if (pipeLoaded) state.pipeExists = true;
    if (kcLoaded)   state.kcExists   = true;

    if (!pipeLoaded && !kcLoaded) {
        cout << "В файле не найдено ни трубы, ни КС.\n";
        return false;
    }

    cout << "Данные загружены";
    if (pipeLoaded && kcLoaded) cout << " (труба и КС)";
    else if (pipeLoaded)        cout << " (только труба)";
    else                        cout << " (только КС)";
    cout << ".\n";
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
    else                  cout << "Трубы нет.\n";

    if (state.kcExists) displayKC(state.kc);
    else                cout << "КС нет.\n";
    cout << "=========================\n";
}

void handleEditPipe(AppState& state) {
    if (!state.pipeExists) {
        cout << "Сначала создайте трубу.\n";
        return;
    }
    displayPipe(state.pipe);
    cout << "1. В ремонт\n";
    cout << "2. В работу\n";
    cout << "3. Переключить\n";
    cout << "0. Отмена\n";
    cout << "Выбор: ";

    int sub;
    if (!(cin >> sub)) {
        skipLine();
        cout << "Некорректный ввод.\n";
        return;
    }
    skipLine();

    switch (sub) {
        case 1: setPipeRepair(state.pipe);  break;
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
    cout << "1. Запустить цех\n";
    cout << "2. Остановить цех\n";
    cout << "0. Отмена\n";
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
                cout << "Цех запущен. В работе: "
                     << state.kc.workingFactories << "\n";
            else
                cout << "Все цехи уже в работе.\n";
            break;
        case 2:
            if (stopShop(state.kc))
                cout << "Цех остановлен. В работе: "
                     << state.kc.workingFactories << "\n";
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
        if (!(cin >> choice)) {
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
            case 1: handleAddPipe(state);          break;
            case 2: handleAddKC(state);            break;
            case 3: handleView(state);             break;
            case 4: handleEditPipe(state);         break;
            case 5: handleEditKC(state);           break;
            case 6: saveAll(state, fileName);      break;
            case 7: loadAll(state, fileName);      break;
            default:
                cout << "Нет такого пункта.\n";
                break;
        }
    }
    return 0;
}