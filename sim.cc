#include "sim.hh"

FILE *out = stdout;
bool out_to_close = false;

using namespace std;

void SetOut(const char *fd){
    if(fd != NULL){
        out = fopen(fd, "w+");
        if(out == NULL){
            //failed to open file, use stdout
            out = stdout;
        }else{
            //file opened successfuly, set the flag
            out_to_close = true;
        }
    }else{
       //stdout
       out = stdout;
    }
}

void Print(const char *str){
	fprintf(out, "%s", str);
}

/** IMPLEMENTATIONS OF CLASSES METHODS **/

/** Input method **/
double Input::Val() const{
    return bp->Val();
}

/** Var methods **/
double Var::Val(){
    return i.Val();
}

void Var::Set(Input in){
    i = in;
}

/** Const method **/
double Const::Val(){
    return val;
}

/** Integrator methods */
void Integrator::Init(){
    output_val = init_val;
}

void Integrator::Set(Input in, double in_val){
    i = in;
    init_val = in_val;
}

void Integrator::Set(Integrator &integrator, double in_val){
    i = integrator;
    init_val = in_val;
}

double Integrator::Get(){
    return input_val;
}

double Integrator::Val(){
    return output_val;
}

double Integrator::IVal(){
    return i.Val();
}

void Integrator::EVal(){
    input_val = i.Val();
}

void Integrator::SetVal(double val){
    output_val = val;
}

/** Operation blocks (OpBlock1/2) methods **/
double OpBlock1::iVal(){
    return in.Val();
}

double OpBlock2::iVal1(){
    return in1.Val();
}

double OpBlock2::iVal2(){
    return in2.Val();
}

/** Arithmetic blocks methods **/
double UMin::Val(){
    return -iVal();
}

double Add::Val(){
    return iVal1() + iVal2();
}

double Sub::Val(){
    return iVal1() - iVal2();
}

double Mul::Val(){
    return iVal1() * iVal2();
}

double Div::Val(){
    return iVal1() / iVal2();
}
