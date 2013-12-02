#include "sim.hh"

FILE *out = stdout;
const char *out_name;
bool out_to_close = false;
double STime = 0.0;
int cnt = 1;

using namespace std;

void SetOut(const char *fd){
    if(fd != NULL){
        //SetOut could be called multiple times, use the last defined output file
        out_name = fd;
    }
}

void OpenOut(){
    if((out = fopen(out_name, "w+")) != NULL){
        out_to_close = true;
    }else{
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
    return output_val;
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

    // open the output file if SetOut was called
    // at least once (opens last set file)
    if(out_name != NULL){
        OpenOut();
    }

    STime = this->GetStart();
    this->InitAll();
    STime += this->GetStep();

    Print("\n");

    for(double t = STime; t < this->GetEnd(); t += this->GetStep()){
        this->IM->Integrate();
        cnt++;
        Print("\n");
        STime += this->GetStep();
    }
    cnt = 1;

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

    //index to access array
	int index = 0;
	//array for store default output values
	double y[s.IntegratorList->size()];
    // store actual simulation time to TemporaryTime variable
	double TTime = STime;

    //two dimensional array k[i][j]
    //i --> coeficient k'i+1' - k1/k2/k3/k4
    //j --> index of integrator from list
    //[i][j] --> value of cofficient for specified integrator
    double k[3][s.IntegratorList->size()];

    Print("%f ", STime);

	// store default values
	for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++) {
		y[index] = (*this->s.iIterator)->Val();
		index++;
	}
	this->s.EvaluateAll();

    //reset index
    //cout << index << endl;
    index = 0;

	//k1 = h*f(t, y(t))
	for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++) {
		k[0][index] = this->s.GetStep() * (*this->s.iIterator)->Get();
		(*this->s.iIterator)->SetVal(y[index] + k[0][index] / 2);
		index++;
	}
    this->s.EvaluateAll();

    //set the time
    STime += this->s.GetStep() / 2;

    //reset index
    index = 0;

	//k2 = h * f(t + h/2, y(t) + k1/2)
	for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++) {
		k[1][index] = this->s.GetStep() * (*this->s.iIterator)->Get();
		(*this->s.iIterator)->SetVal(y[index] + k[1][index] / 2);
		index++;
	}
	this->s.EvaluateAll();

    //reset index
    index = 0;

	//k3 = h * f(t + h/2, y(t) + k2/2)
	for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++) {
		k[2][index] = this->s.GetStep() * (*this->s.iIterator)->Get();
		(*this->s.iIterator)->SetVal(y[index] + k[2][index]);
		index++;
	}
	this->s.EvaluateAll();

	//reset index
    index = 0;

    //set the time
    STime += this->s.GetStep() / 2;

	//k4 = h * f(t + h, y(t) + k3)
	for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++) {
		k[3][index] = this->s.GetStep() * (*this->s.iIterator)->Get();
		//y(t+h) = y(t) + k1/6 + k2/3 + k3/3 + k4/6
		(*this->s.iIterator)->SetVal(y[index] + k[0][index]/6 + k[1][index]/3 + k[2][index]/3 + k[3][index]/6);
		Print("%f ", (*this->s.iIterator)->Val());
		index++;
	}

    //time reset
	STime = TTime;
}

/*

#include <cstdlib>
#include <stdio.h>
#include <iostream>

using namespace std;

double adams_4(int step, double y, double h, double vysl_ad[]) {

    double result, divisor, steps;
    double bashforth[4];

    switch (step) {
        case 1:
            // call Euler
            break;

        case 2:
            bashforth[0] = 3.0;
            bashforth[1] = -1.0;
            divisor = 2.0;
            steps = 2;
            break;

        case 3:
            bashforth[0] = 23.0;
            bashforth[1] = -16.0;
            bashforth[2] = 5.0;
            divisor = 12.0;
            steps = 3;
            break;

        default:
            bashforth[0] = 55.0;
            bashforth[1] = -59.0;
            bashforth[2] = 37.0;
            bashforth[3] = -9.0;
            divisor = 24.0;
            steps = 4;
            break;

    }
    int i;
    int j = step;
    double tmp;

    for (i = 0; i < steps; i++, j--) {
        tmp += bashforth[i] * vysl_ad[j];
    }

    result = y + (h / divisor) * tmp;
    return result;
}

int main(int argc, char** argv) {



    double bashforth[] = {30.0, 2.0, 3.0}; // predchozi vysledky
    printf("%f \n", adams_4(4, 1.0, 1.0, bashforth));


    return 0;
}

*/

/** multi-step Adams-Bashforth **/
void Adams_Bashforth::Integrate(){
    //todo

    //double coeficients[] = {55.0, -59.0, 37.0, -9.0};
    Print("%f ",STime);

    switch(cnt){

        case 1:
            this->s.EvaluateAll();

            for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++){
                (*this->s.iIterator)->SetVal( (*this->s.iIterator)->Val() + ( s.GetStep() * (*this->s.iIterator)->Get() ) );
                (*this->s.iIterator)->results[0] = (*this->s.iIterator)->Val();
                Print("%f ",(*this->s.iIterator)->Val());
            }
            break;

        case 2:
            this->s.EvaluateAll();
            for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++){
                (*this->s.iIterator)->SetVal( (*this->s.iIterator)->Val() + ( s.GetStep() * (*this->s.iIterator)->Get() ) );
                (*this->s.iIterator)->results[1] = (*this->s.iIterator)->Val();
                Print("%f ",(*this->s.iIterator)->Val());
            }
            break;

        case 3:
            this->s.EvaluateAll();
            for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++){
                (*this->s.iIterator)->SetVal( (*this->s.iIterator)->Val() + ( s.GetStep() * (*this->s.iIterator)->Get() ) );
                (*this->s.iIterator)->results[2] = (*this->s.iIterator)->Val();
                Print("%f ",(*this->s.iIterator)->Val());
            }
            break;

        default:
            this->s.EvaluateAll();
            for(this->s.iIterator = this->s.IntegratorList->begin(); this->s.iIterator != this->s.IntegratorList->end(); this->s.iIterator++){
                 (*this->s.iIterator)->SetVal( (*this->s.iIterator)->Val() + (this->s.GetStep()/24) * ( 55*(*this->s.iIterator)->Get() - 59*(*this->s.iIterator)->results[2] + 37*(*this->s.iIterator)->results[1] - 9*(*this->s.iIterator)->results[0] ) );
                 Print("%f ",(*this->s.iIterator)->Val());
            }
    }
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
