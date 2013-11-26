#include "sim.hh"

FILE *out = stdout;
bool out_to_close = false;
double STime = 0.0;

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

void Print(const char *str, ...){
	va_list pars;
	va_start(pars, str);
	vfprintf(out, str, pars);
	va_end(pars);
}

/** IMPLEMENTATIONS OF CLASSES METHODS **/

/** Input method **/
double Input::Val() const{
    return this->bp->Val();
}

/** Var methods **/
double Var::Val(){
    return this->i.Val();
}

void Var::Set(Input in){
    this->i = in;
}

/** Const method **/
double Const::Val(){
    return this->val;
}

/** Integrator methods */
void Integrator::Init(){
    this->output_val = init_val;
}

void Integrator::Set(Input in, double in_val){
    this->i = in;
    this->init_val = in_val;
}

void Integrator::Set(Integrator &integrator, double in_val){
    this->i = integrator;
    this->init_val = in_val;
}

double Integrator::Get(){
    return this->input_val;
}

double Integrator::Val(){
    return this->output_val;
}

double Integrator::IVal(){
    return this->i.Val();
}

void Integrator::EVal(){
    this->input_val = this->i.Val();
}

void Integrator::SetVal(double val){
    this->output_val = val;
}

/** Operation blocks (OpBlock1/2) methods **/
double OpBlock1::iVal(){
    return this->in.Val();
}

double OpBlock2::iVal1(){
    return this->in1.Val();
}

double OpBlock2::iVal2(){
    return this->in2.Val();
}

/** Arithmetic blocks methods **/
double UMin::Val(){
    return -this->iVal();
}

double Add::Val(){
    return this->iVal1() + this->iVal2();
}

double Sub::Val(){
    return this->iVal1() - this->iVal2();
}

double Mul::Val(){
    return this->iVal1() * this->iVal2();
}

double Div::Val(){
    return this->iVal1() / this->iVal2();
}

/** Simulator methods **/
void Sim::Method(IntegrationMethod *im){
    this->IM = im;
}

void Sim::Start(double value){
    this->TStart = value;
}

double Sim::GetStart(){
    return this->TStart;
}

void Sim::End(double value){
    this->TEnd = value;
}

double Sim::GetEnd(){
    return this->TEnd;
}

void Sim::Step(double value){
    this->TStep = value;
}

double Sim::GetStep(){
    return this->TStep;
}

void Sim::Insert(Integrator *i){
    this->IntegratorList->push_back(i);
}

void Sim::InitAll(){
    Print("%f ", STime);
    for(iIterator = IntegratorList->begin(); iIterator != IntegratorList->end(); iIterator++ ){
       (*iIterator)->Init();
       Print("%f ", (*iIterator)->Val());
    }
};

void Sim::EvaluateAll(){
    for(this->iIterator = this->IntegratorList->begin(); this->iIterator != this->IntegratorList->end(); this->iIterator++ ){
       (*this->iIterator)->EVal();
    }
}

void Sim::StartSim(){
    STime = this->GetStart();
    this->InitAll();
    STime = this->GetStart() + this->GetStep();
    Print("\n");
    for(double t = STime; t < this->GetEnd(); t += this->GetStep()){
        this->IM->Integrate();
        Print("\n");
        STime += this->GetStep();
    }

    // close file if the output file is set
    if(out_to_close == true){
        fclose(out);
        out_to_close = false; //unset the 'file-opened' flag
    }
}

/** Methods for numerical integration **/

/** one-step Euler **/
void Euler::Integrate(){
    this->s.EvaluateAll();
    Print("%f ",STime);
    for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++){
        (*this->s.iIterator)->SetVal( (*this->s.iIterator)->Val() + ( s.GetStep() * (*this->s.iIterator)->Get() ) );
        Print("%f ",(*this->s.iIterator)->Val());
    }
}

/** one-step RK4 **/
void Runge_Kutta_4::Integrate(){
    //todo
}

/** multi-step Adams-Bashforth **/
void Adams_Bashforth::Integrate(){
    //todo
}

/** Methods of Aritmetical blocks **/
Input operator - (Input x){
    return new UMin(x);
}

Input operator + (Input x, Input y){
    return new Add(x,y);
}

Input operator - (Input x, Input y){
    return new Sub(x,y);
}

Input operator * (Input x, Input y){
    return new Mul(x,y);
}

Input operator / (Input x, Input y){
    return new Div(x,y);
}
