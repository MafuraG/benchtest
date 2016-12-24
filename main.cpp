#include <QCoreApplication>
#include <QtConcurrent>
#include <iostream>

class TestClass{
public:
    TestClass(){}
    TestClass(double _a,double _b):a(_a), b(_b){}
    double getB() const {return b;}
    void setB(double value){this->b = value;}

    double getA() const{return a;}
    void setA(double value){this->a = value;}

private:
    double a,b;
};


//random generator generic function courtesy "stackoverflow.com/users/3807729/galik"
template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to)
{
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
}


struct mapFunction{
    mapFunction(){}
    typedef TestClass result_type;
    TestClass operator()(const TestClass &t)
    {
        TestClass t1;
        t1.setA(random<double>(1,2));
        t1.setB(random<double>(3,4));
        return t1;
    }
};

//struct reduceFunction{
//    reduceFunction(){}
//    typedef void result_type;

//};

void reduceFunction(double &result, const TestClass &term)
{
    result += term.getA() * term.getB();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVector<TestClass> testVect = QVector<TestClass>(10000000);

    int mapReduceTime = 0;
    QTime time;
    time.start();
    QFuture<double> r = QtConcurrent::mappedReduced(testVect,mapFunction(),reduceFunction);
    double result = r.result();
    mapReduceTime = time.elapsed();
    std::cout<<"Result: "<<result<<" Elapsed time: "<<mapReduceTime;

    return a.exec();
}
