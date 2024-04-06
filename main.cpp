#include <iostream>
#include <fstream>
#include <deque>
#include <string>


//1. Разработать шаблонный (с возможными типами float, double, int, short) 
//класс фильтра низких частот (ФНЧ) на основе скользящего среднего:
template<typename T>
class LowPassFilter {
    static_assert(std::is_same<T, float>::value ||
                  std::is_same<T, double>::value ||
                  std::is_same<T, int>::value ||
                  std::is_same<T, short>::value,
                  "Тип должен быть float, double, int или short");
private:
    int windowWidth;
    std::deque<T> window;
    T sum;

public:

//1.2 Ширина окна скользящего среднего задается в конструкторе;
    LowPassFilter(int width) : windowWidth(width), sum(0) {}

    int getWindowWidth() const {
        return windowWidth;
    }

    const std::deque<T>& getWindow() const {
        return window;
    }

    T getSum() const {
        return sum;
    }

    // Определение заполненности списка
    bool is_full() const {
        return (window.size() == windowWidth);
    }

//1.3 Данные (с шаблонным типом) на вход ФНЧ поступают по одному отсчету. 
//В качестве метода для приема данных использовать перегрузку оператора "<<";
    void operator<<(T value) {
        window.push_back(value);
        sum += value;
        if (window.size() > windowWidth) {
            sum -= window.front();
            window.pop_front();
        }
    }

//1.4 На выходе ФНЧ на каждый входной отсчет 
//должно выдаваться отфильтрованное значение
    T getFilteredValue() const {
        return sum / static_cast<T>(windowWidth);
    }

};


//2. Разработать класс фильтра высоких частот (ФВЧ) 
//как наследника от класа ФНЧ
template<typename T>
class HighPassFilter : public LowPassFilter<T> {
public:

//2.1 Интерфейс для создания и приема и выдачи результатов данных тот-же
    HighPassFilter(int width) : LowPassFilter<T>(width) {}

    bool is_full() const {
        return (LowPassFilter<T>::is_full());
    }

    void operator<<(T value) {
        // Вызываем оператор ввода базового класса
        LowPassFilter<T>::operator<<(value);
    }

    // Получение отфильтрованного значения для ФВЧ с учетом разницы входного и ФНЧ сигналов
    T getFilteredValue() const {
        // Из исходного сигнала вычитаем результат после ФНЧ
        return LowPassFilter<T>::getFilteredValue();
    }

};



int main() {
    std::ifstream inputFile("signal_100Hz.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::ofstream lbfFile("LBF.txt");
    if (!lbfFile.is_open()) {
        std::cerr << "Failed to create LBF.txt.\n";
        return 1;
    }

    std::ofstream hbfFile("HBF.txt");
    if (!hbfFile.is_open()) {
        std::cerr << "Failed to create HBF.txt.\n";
        return 1;
    }

    //Задать параметры ширины окна
    int windowHPF = 80;
    int windowLPF = 50;

    float value;
    LowPassFilter<float> lpf(windowLPF);
    HighPassFilter<float> hpf(windowHPF);


    
    while (inputFile >> value) {
        lpf << value;
        hpf << value;
        
        // Проверка LowPassFilter на заполненность, чтобы записать в LBF.txt
        if (lpf.is_full()) {
            lbfFile << lpf.getFilteredValue() << '\n';
        }

        // Проверка HighPassFilter на заполненность, чтобы записать в HBF.txt
        if (hpf.is_full()) {
            hbfFile << hpf.getFilteredValue() << '\n';
        }
        
    }

    inputFile.close();
    lbfFile.close();
    hbfFile.close();

    std::cout << "Filtering complete. Results saved to LBF.txt and HBF.txt.\n";

    return 0;

}
