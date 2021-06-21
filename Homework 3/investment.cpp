class Investment {
    public:
        Investment(string name, int purchasePrice);
        virtual ~Investment();
        virtual string description() const = 0;
        virtual bool fungible() const;
        string name() const;
        int purchasePrice() const;
    private:
        string m_name;
        int m_purchasePrice;
};

class Painting : public Investment {
    public:
        Painting(string name, int purchasePrice);
        ~Painting();
        string description() const; // override of description
};

class Stock : public Investment {
    public:
        Stock(string name, int purchasePrice, string ticker);
        ~Stock();
        string description() const; // override of description
        bool fungible() const;  // override of fungible
    private:
        string m_ticker;
};

class House : public Investment {
    public:
        House(string name, int purchasePrice);
        ~House();
        string description() const; // override of description
};

Investment::Investment(string name, int purchasePrice) : m_name(name), m_purchasePrice(purchasePrice) {}
Investment::~Investment() {}
bool Investment::fungible() const { return false; }
string Investment::name() const { return m_name; }
int Investment::purchasePrice() const { return m_purchasePrice; }

Painting::Painting(string name, int purchasePrice) : Investment(name, purchasePrice) {}
Painting::~Painting() { cout << "Destroying " << name() << ", a painting" << endl; }
string Painting::description() const { return "painting"; };

Stock::Stock(string name, int purchasePrice, string ticker) : Investment(name, purchasePrice), m_ticker(ticker) {}
Stock::~Stock() { cout << "Destroying " << name() << ", a stock holding" << endl; }
string Stock::description() const { return "stock trading as " + m_ticker; };
bool Stock::fungible() const { return true; }

House::House(string name, int purchasePrice) : Investment(name, purchasePrice) {}
House::~House() { cout << "Destroying the house " << name() << endl; }
string House::description() const { return "house"; }