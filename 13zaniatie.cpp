#include <iostream>
#include <cmath>
//  ЧАСТЬ A: АТД «Множество» (Set) для целых чисел
//  Хранение: динамический массив без STL
class Set {
private:
    int* data;
    int  sz;
    int  cap;
    // Удваиваем буфер при нехватке места
    void grow() {
        cap *= 2;
        int* tmp = new int[cap];
        for (int i = 0; i < sz; i++) tmp[i] = data[i];
        delete[] data;
        data = tmp;
    }
public:
    Set() : data(new int[4]), sz(0), cap(4) {}
    Set(const Set& o) : data(new int[o.cap]), sz(o.sz), cap(o.cap) {
        for (int i = 0; i < sz; i++) data[i] = o.data[i];
    }
    Set& operator=(const Set& o) {
        if (this != &o) {
            delete[] data;
            cap = o.cap; sz = o.sz;
            data = new int[cap];
            for (int i = 0; i < sz; i++) data[i] = o.data[i];
        }
        return *this;
    }
    ~Set() { delete[] data; }
    // Проверка вхождения
    bool contains(int v) const {
        for (int i = 0; i < sz; i++) if (data[i] == v) return true;
        return false;
    }
    // Добавление элемента (дубли игнорируются — свойство множества)
    void add(int v) {
        if (contains(v)) return;
        if (sz == cap) grow();
        data[sz++] = v;
    }
    // Удаление: ставим последний на место удалённого — O(1)
    void remove(int v) {
        for (int i = 0; i < sz; i++)
            if (data[i] == v) { data[i] = data[--sz]; return; }
    }
    int  size() const { return sz; }
    int  operator[](int i)    const { return data[i]; }
    // Перегруженные операции
    // (>) Проверка принадлежности: (set > elem) ≡ elem ∈ set
    bool operator>(int v) const { return contains(v); }
    // (*) Пересечение двух множеств
    Set operator*(const Set& o) const {
        Set res;
        for (int i = 0; i < sz; i++)
            if (o.contains(data[i])) res.add(data[i]);
        return res;
    }
    // (<) Проверка подмножества: (a < b) ≡ a нестрогое включение b
    bool operator<(const Set& o) const {
        for (int i = 0; i < sz; i++)
            if (!o.contains(data[i])) return false;
        return true;
    }
    friend std::ostream& operator<<(std::ostream& os, const Set& s) {
        os << "{ ";
        for (int i = 0; i < s.sz; i++) os << s.data[i] << ' ';
        return os << '}';
    }
};
// Глобальная перегрузка: (elem > set) ≡ elem Е set
bool operator>(int v, const Set& s) { return s > v; }
//  ЧАСТЬ B: Иерархия матричных классов
//  Matrix (базовый, абстрактный стиль)
//  SquareMatrix(квадратная n×n)
//  SymmetricMatrix(симметричная: A[i][j]==A[j][i])
//  RectMatrix(прямоугольная m×n)
//  Статический член totalElements считает сумму элементов всех "живых" объектов (alloc +, dealloc -)
//  Виртуальные функции: print(), norm(), typeName()
class Matrix {
protected:
    int      rows, cols;
    double** data;
    static int totalElements; // суммарное кол-во элементов
    void alloc(int r, int c) {
        rows = r; cols = c;
        data = new double* [r];
        for (int i = 0; i < r; i++) data[i] = new double[c](); // инициализация 0
        totalElements += r * c;
    }
    void dealloc() {
        totalElements -= rows * cols;
        for (int i = 0; i < rows; i++) delete[] data[i];
        delete[] data;
    }
    void copyData(const Matrix& o) {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                data[i][j] = o.data[i][j];
    }
public:
    Matrix(int r, int c) { alloc(r, c); }

    Matrix(const Matrix& o) { alloc(o.rows, o.cols); copyData(o); }

    Matrix& operator=(const Matrix& o) {
        if (this != &o) { dealloc(); alloc(o.rows, o.cols); copyData(o); }
        return *this;
    }
    virtual ~Matrix() { dealloc(); }
    double getElement(int i, int j) const { return data[i][j]; }
    int    getRows()  const { return rows; }
    int    getCols()  const { return cols; }
    // Виртуальная установка элемента (переопределяется в SymmetricMatrix)
    virtual void setElement(int i, int j, double v) { data[i][j] = v; }
    // Виртуальные функции
    // 1. Вывод матрицы на экран
    virtual void print() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) std::cout << data[i][j] << '\t';
            std::cout << '\n';
        }
    }
    // 2. Норма Фробениуса: sqrt( Σ a[i][j]^2 )
    virtual double norm() const {
        double s = 0;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) s += data[i][j] * data[i][j];
        return std::sqrt(s);
    }
    // 3. Название типа (для демонстрации полиморфизма)
    virtual const char* typeName() const { return "Matrix"; }
    // Статический: суммарное кол-во элементов всех матриц
    static int getTotalElements() { return totalElements; }
    bool operator==(const Matrix& o) const {
        if (rows != o.rows || cols != o.cols) return false;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (data[i][j] != o.data[i][j]) return false;
        return true;
    }
    // operator<< вызывает виртуальный print - полиморфизм работает через cout
    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        m.print(); return os;
    }
};
int Matrix::totalElements = 0;
// Квадратная матрица (n × n)
class SquareMatrix : public Matrix {
public:
    SquareMatrix(int n = 1) : Matrix(n, n) {}
    SquareMatrix(const SquareMatrix& o) : Matrix(o) {}
    const char* typeName() const override { return "SquareMatrix"; }
    void print() const override {
        std::cout << "[SquareMatrix " << rows << 'x' << cols << "]\n";
        Matrix::print();
    }
    // Уникальный метод: след матрицы (сумма главной диагонали)
    double trace() const {
        double s = 0;
        for (int i = 0; i < rows; i++) s += data[i][i];
        return s;
    }
};
// Симметричная матрица
// Ключевое свойство: при setElement(i, j, v) автоматически data[j][i] = v
class SymmetricMatrix : public SquareMatrix {
public:
    SymmetricMatrix(int n = 1) : SquareMatrix(n) {}
    SymmetricMatrix(const SymmetricMatrix& o) : SquareMatrix(o) {}
    void setElement(int i, int j, double v) override {
        data[i][j] = v;
        data[j][i] = v; // зеркальный элемент
    }
    const char* typeName() const override { return "SymmetricMatrix"; }
    void print() const override {
        std::cout << "[SymmetricMatrix " << rows << 'x' << cols << "]\n";
        Matrix::print();
    }
};
// Прямоугольная матрица (m × n)
class RectMatrix : public Matrix {
public:
    RectMatrix(int r = 1, int c = 1) : Matrix(r, c) {}
    RectMatrix(const RectMatrix& o) : Matrix(o) {}
    const char* typeName() const override { return "RectMatrix"; }
    void print() const override {
        std::cout << "[RectMatrix " << rows << 'x' << cols << "]\n";
        Matrix::print();
    }
};
//  ЧАСТЬ C: Шаблонный класс TSet<T>
//  Реализован на однонаправленном связном списке - не требует конструктора по умолчанию у T, работает с любым типом, имеющим operator==
//  Операции те же, что у Set: >, *, 
template<typename T>
class TSet {
private:
    struct Node {
        T     value;
        Node* next;
        explicit Node(const T& v) : value(v), next(nullptr) {}
    };
    Node* head;
    int   sz;
    void clear() {
        while (head) { Node* nx = head->next; delete head; head = nx; }
        sz = 0;
    }
public:
    TSet() : head(nullptr), sz(0) {}
    TSet(const TSet& o) : head(nullptr), sz(0) {
        for (Node* c = o.head; c; c = c->next) add(c->value);
    }
    TSet& operator=(const TSet& o) {
        if (this != &o) {
            clear();
            for (Node* c = o.head; c; c = c->next) add(c->value);
        }
        return *this;
    }
    ~TSet() { clear(); }
    bool contains(const T& v) const {
        for (Node* c = head; c; c = c->next)
            if (c->value == v) return true;
        return false;
    }
    // Добавление в голову списка (порядок не важен для множества)
    void add(const T& v) {
        if (!contains(v)) {
            Node* n = new Node(v);
            n->next = head;
            head = n;
            sz++;
        }
    }
    void remove(const T& v) {
        Node** pp = &head;
        while (*pp) {
            if ((*pp)->value == v) {
                Node* del = *pp;
                *pp = del->next;
                delete del;
                sz--;
                return;
            }
            pp = &(*pp)->next;
        }
    }
    int size() const { return sz; }
    // (>) Принадлежность: (set > elem) ≡ elem ∈ set
    bool operator>(const T& v) const { return contains(v); }
    // (*) Пересечение
    TSet operator*(const TSet& o) const {
        TSet res;
        for (Node* c = head; c; c = c->next)
            if (o.contains(c->value)) res.add(c->value);
        return res;
    }
    // (<) Подмножество: (this < o) ≡ this ⊆ o
    bool operator<(const TSet& o) const {
        for (Node* c = head; c; c = c->next)
            if (!o.contains(c->value)) return false;
        return true;
    }
    // print использует operator<< типа T
    // Для Matrix это вызывает виртуальный print() — полиморфизм!
    void print() const {
        std::cout << "TSet{ ";
        for (Node* c = head; c; c = c->next) {
            std::cout << c->value;
            if (c->next) std::cout << " | ";
        }
        std::cout << "}";
    }
};
// Глобальная перегрузка: (elem > set) ≡ elem ∈ set
template<typename T>
bool operator>(const T& v, const TSet<T>& s) { return s.contains(v); }
//  main: демонстрация всех трёх частей
int main() {
    // ЧАСТЬ A
    std::cout << "========== ЧАСТЬ A: Set ==========\n";
    Set A, B, sub;
    A.add(1); A.add(2); A.add(3); A.add(4);
    B.add(3); B.add(4); B.add(5); B.add(6);
    sub.add(1); sub.add(2);
    std::cout << "A   = " << A << "\n";
    std::cout << "B   = " << B << "\n";
    std::cout << "sub = " << sub << "\n\n";
    std::cout << "A > 3  (3 ∈ A):    " << (A > 3 ? "true" : "false") << "\n";
    std::cout << "A > 9  (9 ∈ A):    " << (A > 9 ? "true" : "false") << "\n";
    std::cout << "5 > B  (5 ∈ B):    " << (5 > B ? "true" : "false") << "\n\n";
    Set inter = A * B;
    std::cout << "A * B = " << inter << "  (пересечение)\n";
    std::cout << "sub < A (sub ⊆ A): " << (sub < A ? "true" : "false") << "\n";
    std::cout << "A   < B (A ⊆ B):   " << (A < B ? "true" : "false") << "\n\n";
    // ЧАСТЬ B
    std::cout << "========== ЧАСТЬ B: Матрицы ==========\n";
    SquareMatrix sq(3);
    sq.setElement(0, 0, 1); sq.setElement(0, 1, 2); sq.setElement(0, 2, 3);
    sq.setElement(1, 0, 4); sq.setElement(1, 1, 5); sq.setElement(1, 2, 6);
    sq.setElement(2, 0, 7); sq.setElement(2, 1, 8); sq.setElement(2, 2, 9);
    sq.print();
    std::cout << "typeName = " << sq.typeName()
        << "  |  trace = " << sq.trace()
        << "  |  norm = " << sq.norm() << "\n\n";

    SymmetricMatrix sym(3);
    sym.setElement(0, 0, 4.0); sym.setElement(1, 1, 5.0); sym.setElement(2, 2, 6.0);
    sym.setElement(0, 1, 1.0); // автоматически sym[1][0] = 1.0
    sym.setElement(0, 2, 2.0); // автоматически sym[2][0] = 2.0
    sym.setElement(1, 2, 3.0); // автоматически sym[2][1] = 3.0
    sym.print();
    std::cout << "typeName = " << sym.typeName()
        << "  |  trace = " << sym.trace()
        << "  |  norm = " << sym.norm() << "\n\n";

    RectMatrix rect(2, 4);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 4; j++)
            rect.setElement(i, j, double(i * 4 + j + 1));
    rect.print();
    std::cout << "typeName = " << rect.typeName()
        << "  |  norm = " << rect.norm() << "\n\n";

    // Полиморфизм: работаем с объектами через указатель на базовый класс
    std::cout << "--- Полиморфный обход ---\n";
    Matrix* mats[] = { &sq, &sym, &rect };
    for (int i = 0; i < 3; i++)
        std::cout << "  " << mats[i]->typeName()
        << "  norm=" << mats[i]->norm() << "\n";

    std::cout << "\nОбщий счётчик элементов (static): "
        << Matrix::getTotalElements() << "\n\n";
    // ЧАСТЬ C
    std::cout << "========== ЧАСТЬ C: TSet<T> ==========\n";
    // TSet<int>
    std::cout << "\n--- TSet<int> ---\n";
    TSet<int> ti1, ti2, tiSub;
    ti1.add(1); ti1.add(2); ti1.add(3); ti1.add(4);
    ti2.add(3); ti2.add(4); ti2.add(5);
    tiSub.add(1); tiSub.add(2);
    std::cout << "ti1  = "; ti1.print();   std::cout << "\n";
    std::cout << "ti2  = "; ti2.print();   std::cout << "\n";
    std::cout << "tiSub= "; tiSub.print(); std::cout << "\n";
    std::cout << "ti1 > 2  (2 ∈ ti1):      " << (ti1 > 2 ? "true" : "false") << "\n";
    std::cout << "7 > ti1  (7 ∈ ti1):      " << (7 > ti1 ? "true" : "false") << "\n";
    TSet<int> tiInter = ti1 * ti2;
    std::cout << "ti1 * ti2 = "; tiInter.print(); std::cout << "\n";
    std::cout << "tiSub < ti1 (⊆):         " << (tiSub < ti1 ? "true" : "false") << "\n";
    std::cout << "ti2   < ti1 (⊆):         " << (ti2 < ti1 ? "true" : "false") << "\n";
    // TSet<char>
    std::cout << "\n--- TSet<char> ---\n";
    TSet<char> tc1, tc2;
    tc1.add('a'); tc1.add('b'); tc1.add('c');
    tc2.add('b'); tc2.add('c'); tc2.add('d');
    std::cout << "tc1 = "; tc1.print(); std::cout << "\n";
    std::cout << "tc2 = "; tc2.print(); std::cout << "\n";
    std::cout << "tc1 > 'b':  " << (tc1 > 'b' ? "true" : "false") << "\n";
    TSet<char> tcInter = tc1 * tc2;
    std::cout << "tc1 * tc2 = "; tcInter.print(); std::cout << "\n";
    std::cout << "tc1 < tc2 (⊆): " << (tc1 < tc2 ? "true" : "false") << "\n";
    // TSet<SquareMatrix> — множество матриц различных "значений"
    std::cout << "\n--- TSet<SquareMatrix> ---\n";
    SquareMatrix m1(2), m2(2), m3(2);
    m1.setElement(0, 0, 1); m1.setElement(0, 1, 2);
    m1.setElement(1, 0, 3); m1.setElement(1, 1, 4);
    m2.setElement(0, 0, 5); m2.setElement(0, 1, 6);
    m2.setElement(1, 0, 7); m2.setElement(1, 1, 8);
    // m3 идентична m1 — для проверки дублирования
    m3.setElement(0, 0, 1); m3.setElement(0, 1, 2);
    m3.setElement(1, 0, 3); m3.setElement(1, 1, 4);
    TSet<SquareMatrix> sm1, sm2;
    sm1.add(m1); sm1.add(m2);          // sm1 = { m1, m2 }
    sm2.add(m3); sm2.add(m2);          // sm2 = { m3(≡m1), m2 }
    sm2.add(m3);                        // дубль — не добавится
    std::cout << "sm1.size=" << sm1.size()
        << "  sm2.size=" << sm2.size() << "\n";
    std::cout << "sm1 > m1 (m1 ∈ sm1): " << (sm1 > m1 ? "true" : "false") << "\n";
    std::cout << "sm1 > m3 (m3≡m1 ∈ sm1): " << (sm1 > m3 ? "true" : "false") << "\n";
    TSet<SquareMatrix> smInter = sm1 * sm2;
    std::cout << "Пересечение sm1*sm2, размер = " << smInter.size()
        << " (оба содержат m1 и m2)\n";
    std::cout << "sm2 < sm1 (⊆): " << (sm2 < sm1 ? "true" : "false") << "\n";
    std::cout << "\nСчётчик элементов матриц в конце: "
        << Matrix::getTotalElements() << "\n";
    return 0;
}
