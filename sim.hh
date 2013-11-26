#ifndef SIM_HH
#define SIM_HH

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <iostream>

#include <list>
#define dlist std::list

using namespace std;

/** global varibles and functions **/

extern FILE *out;
extern bool out_to_close;
extern double STime;

void SetOut(const char *file);
void Print(const char *str, ...);

/** class prototypes **/
class Block;
class Input;
class Var;
class Const;
class OpBlock1;
class OpBlock2;
class Add;
class Sub;
class Mul;
class Div;
class UMin;
class Integrator;
class IntegrationMethod;
class Euler;
class Runge_Kutta_4;
class Adams_Bashforth;
class Sim;

/** base class for all blocks
    every block has it's own output value */
class Block{
    public:
        virtual double Val() = 0;
};

/** wrapper for pointer to objects of a Block
    used for referencing of continuous blocks in expressions */
class Input{
    Block *bp;
    public:
        Input(const Input &i): bp(i.bp){}
        Input(Block &b): bp(&b){}
        Input(Block *b): bp(b){}
        double Val() const;
};

/** block representating variable
    value can be changed **/
class Var : public Block{
    Input i;
    public:
        Var() : i(0) {}
        void Set(Input in);
        virtual double Val();
};

/** block representating constant
    constant keeps always the same (defined) value **/
class Const : public Block{
    const double val;
    public:
        Const(double v) : val(v){}
        virtual double Val();
};

/** block representating operations with one operand **/
class OpBlock1 : public Block{
    Input in;
    public:
        OpBlock1(Input i) : in(i) {}
        double iVal();
};

/** block representating operations with two operands **/
class OpBlock2 : public Block{
    Input in1, in2;
    public:
        OpBlock2(Input i1, Input i2) : in1(i1), in2(i2) {}
        double iVal1();
        double iVal2();
};


/** overloading of operators, so they can be used with ooprands like
    instance of Input class **/
Input operator + (Input a, Input b);
Input operator - (Input a, Input b);
Input operator * (Input a, Input b);
Input operator / (Input a, Input b);
Input operator - (Input a);

/** operation unary minus **/
class UMin : public OpBlock1{
    public:
        UMin(Input i) : OpBlock1(i){}
        virtual double Val();
};

/** operation add **/
class Add : public OpBlock2{
    public:
        Add(Input i1, Input i2) : OpBlock2(i1, i2){}
        virtual double Val();
};

/** operation subtract **/
class Sub : public OpBlock2{
    public:
        Sub(Input i1, Input i2) : OpBlock2(i1, i2){}
        virtual double Val();
};

/** operation multiply **/
class Mul : public OpBlock2{
    public:
        Mul(Input i1, Input i2) : OpBlock2(i1, i2){}
        virtual double Val();
};

/** operation divide **/
class Div : public OpBlock2{
    public:
        Div(Input i1, Input i2) : OpBlock2(i1, i2){}
        virtual double Val();
};

/** block integrator, represents numeric integration **/
class Integrator : public Block{
    private:
        double input_val;
        double output_val;
    protected:
        Input i;
        double init_val;
    public:
        Integrator(): i(0), init_val(0.0) {}
        void Set(Input in, double in_val = 0);
        void Set(Integrator &integrator, double in_val = 0);
        double Get();
        double IVal();
        void SetVal(double val);
        void Init();
        double Val();
        void EVal();
};

/** main simulator class,
    represents integrator container
    has it's own integration method (one for all integrations in
    one simulator)
**/
class Sim{
    private:
        double TStart;
        double TEnd;
        double TStep;
        IntegrationMethod *IM;
    public:
        dlist<Integrator*> *IntegratorList;
        dlist<Integrator*>::iterator iIterator;
        Sim(){IntegratorList = new dlist<Integrator *>;}
        void Method(IntegrationMethod *im); //set the integraton method
        void Start(double value); //set start time
        double GetStart(); //get start time
        void End(double value); //set end time
        double GetEnd(); //get end time
        void Step(double value); //set step time
        double GetStep(); //get step time
        void Insert(Integrator *i); //insert integrator to list
        void InitAll(); //initialize all
        void EvaluateAll(); //evaluate all integrators
        void StartSim(); //start simulation
};

/** classes for integration methods **/

/** class IntegrationMethod, encapsulates
    all numerical methods classes for integrations
*/
class IntegrationMethod{
    public:
        virtual void Integrate(void) = 0;
};

class Euler : public IntegrationMethod{
    private:
        Sim &s;
    public:
        Euler(Sim &sim) : s(sim){}
        virtual void Integrate(void);
};

class Runge_Kutta_4 : public IntegrationMethod{
    private:
        Sim &s;
    public:
        Runge_Kutta_4(Sim &sim) : s(sim){}
        virtual void Integrate(void);
};

class Adams_Bashforth : public IntegrationMethod{
    private:
        Sim &s;
    public:
        Adams_Bashforth(Sim &sim) : s(sim){}
        virtual void Integrate(void);
};

#endif
