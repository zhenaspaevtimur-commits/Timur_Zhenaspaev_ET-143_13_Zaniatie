#include <iostream>
using namespace std;
// ЗАДАНИЕ B: Иерархия классов документа
class Document {
private:
    int id;
    static int documentCount; // Статическая переменная для подсчета

public:
    Document(int id) : id(id) {
        documentCount++;
    }

    virtual ~Document() {
        documentCount--;
    }
    // Статическая функция (выводит количество документов)
    static int getDocumentCount() {
        return documentCount;
    }
    // Виртуальная функция 1
    virtual void print() const {
        cout << "Document ID: " << id;
    }
    // Виртуальная функция 2 (чисто виртуальная, делает класс абстрактным)
    virtual const char* getDocType() const = 0;
    int getId() const { return id; }
};

// Инициализация статической переменной
int Document::documentCount = 0;
// Класс-наследник 1
class Article : public Document {
private:
    const char* title;

public:
    Article(int id, const char* title) : Document(id), title(title) {}
    void print() const override {
        cout << "Article [ID: " << getId() << ", Title: " << title << "]";
    }
    const char* getDocType() const override {
        return "Article";
    }
};
// Класс-наследник 2
class Report : public Document {
private:
    int pagesCount;

public:
    Report(int id, int pagesCount) : Document(id), pagesCount(pagesCount) {}
    void print() const override {
        cout << "Report [ID: " << getId() << ", Pages: " << pagesCount << "]";
    }
    const char* getDocType() const override {
        return "Report";
    }
};

// ЗАДАНИЕ A: Шаблонный класс множества
template <typename T>
class Set {
private:
    T* elements;     // Динамический массив для хранения элементов
    int capacity;    // Вместимость массива
    int currentSize; // Текущее количество элементов
    // Скрытая функция для расширения массива при нехватке места
    void expand() {
        capacity *= 2;
        T* newElements = new T[capacity];
        for (int i = 0; i < currentSize; i++) {
            newElements[i] = elements[i];
        }
        delete[] elements;
        elements = newElements;
    }
public:
    // Базовый конструктор
    Set() : capacity(5), currentSize(0) {
        elements = new T[capacity];
    }

    // Деструктор (очистка памяти)
    ~Set() {
        delete[] elements;
    }
    // Конструктор копирования (ОБЯЗАТЕЛЕН при работе с динамической памятью)
    Set(const Set& other) : capacity(other.capacity), currentSize(other.currentSize) {
        elements = new T[capacity];
        for (int i = 0; i < currentSize; i++) {
            elements[i] = other.elements[i];
        }
    }
    // Оператор присваивания
    Set& operator=(const Set& other) {
        if (this != &other) {
            delete[] elements;
            capacity = other.capacity;
            currentSize = other.currentSize;
            elements = new T[capacity];
            for (int i = 0; i < currentSize; i++) {
                elements[i] = other.elements[i];
            }
        }
        return *this;
    }
    // Проверка наличия элемента
    bool contains(const T& item) const {
        for (int i = 0; i < currentSize; i++) {
            if (elements[i] == item) return true;
        }
        return false;
    }
    // Перегрузка () - конструктор множества (добавление с исключением дубликатов)
    Set& operator()(const T& item) {
        if (!contains(item)) {
            if (currentSize >= capacity) {
                expand();
            }
            elements[currentSize++] = item;
        }
        return *this; // Возвращаем ссылку на себя для цепочки: set(1)(2)(3)
    }
    // Перегрузка + (объединение множеств)
    Set operator+(const Set& other) const {
        Set result(*this); // Копируем текущее множество
        for (int i = 0; i < other.currentSize; i++) {
            result(other.elements[i]); // Используем перегруженный (), он сам отсеет дубликаты
        }
        return result;
    }
    // Перегрузка <= (сравнение: является ли текущее подмножеством other)
    bool operator<=(const Set& other) const {
        for (int i = 0; i < currentSize; i++) {
            if (!other.contains(elements[i])) {
                return false; // Нашли элемент, которого нет в other
            }
        }
        return true;
    }
    // Вспомогательные методы для работы извне
    int size() const { return currentSize; }
    T get(int index) const { return elements[index]; }
};
// ЗАДАНИЕ C: Демонстрация работы
int main() {
    cout << "=== Test 1: Set of Natural Numbers ===" << endl;
    Set<unsigned int> setA;
    // Используем перегруженный оператор () в стиле конструктора множества
    setA(1)(2)(3)(2)(1); // Дубликаты 2 и 1 будут проигнорированы
    cout << "Set A elements: ";
    for (int i = 0; i < setA.size(); i++) cout << setA.get(i) << " ";
    cout << endl;
    Set<unsigned int> setB;
    setB(3)(4)(5);
    Set<unsigned int> setUnion = setA + setB; // Объединение
    cout << "Set A + Set B (Union): ";
    for (int i = 0; i < setUnion.size(); i++) cout << setUnion.get(i) << " ";
    cout << endl;
    Set<unsigned int> setC;
    setC(1)(2);
    cout << "Is {1, 2} a subset of Set A? " << (setC <= setA ? "Yes" : "No") << endl;
    cout << "Is {1, 5} a subset of Set A? " << (setB <= setA ? "Yes" : "No") << endl;

    cout << "\n=== Test 2: Set of Documents ===" << endl;
    // Создаем документы
    Article doc1(101, "C++ for Beginners");
    Report doc2(102, 45);
    Article doc3(103, "Advanced Patterns");
    // Множество указателей на базовый класс (для поддержки полиморфизма)
    Set<Document*> docSet1;
    docSet1(&doc1)(&doc2);
    Set<Document*> docSet2;
    docSet2(&doc2)(&doc3); // doc2 участвует в обоих множествах
    Set<Document*> docUnion = docSet1 + docSet2;
    cout << "Union of Document Sets (duplicates excluded):" << endl;
    for (int i = 0; i < docUnion.size(); i++) {
        cout << "- ";
        // Полиморфный вызов: вызовется print() нужного наследника
        docUnion.get(i)->print();
        cout << " (Type: " << docUnion.get(i)->getDocType() << ")" << endl;
    }
    cout << "\nTotal documents created: " << Document::getDocumentCount() << endl;
}
