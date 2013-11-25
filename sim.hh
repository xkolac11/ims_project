#ifndef SIM_HH
#define SIM_HH

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <iostream>


using namespace std;

/** global varibles and functions **/

extern FILE *out;
extern bool out_to_close;

void SetOut(const char *file);
void Print(const char *str);

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
        OpBlock1(Input i);
        double iVal();
};

/** block representating operations with two operands **/
class OpBlock2 : public Block{
    Input in1, in2;
    public:
        OpBlock2(Input i1, Input i2);
        double iVal1();
        double iVal2();
};


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

/** overloading of operators, so they can be used with ooprands like
    instance of Input class **/
Input operator + (Input a, input b);
Input operator - (Input a, input b);
Input operator * (Input a, input b);
Input operator / (Input a, input b);
Input operator - (Input a);

/** block integrator, represents numeric integration **/
class Integrator : public Block{
    public:
        double input_val;
        double output_val;
    protected:
        Input i;
        double init_val;
    private:
        Integrator(): i(0), init_val(0.0) {}
        void Init();
        void Set(Input in, double in_val = 0);
        void Set(Integrator &integrator, double in_val = 0);
        double Get();
        double Val();
        double IVal();
        void EVal();
        void SetVal(double val);
};

#endif
