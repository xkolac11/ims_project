#include "sim.hh"

FILE *out = stdout;
const char *out_name;
bool out_to_close = false;
double STime = 0.0;

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

    int size = s.IntegratorList->size();
	int index;
	int i;
	double step = s.GetStep();
	double k1[size], k2[size], k3[size], k4[size], y[size]; // we need to store coefficients and default output values
	double def_time = STime; // stores default simulation time

	Print("%.3f ", STime);

    //reset index
    index = 0;

	// save default output values
	for(s.iIterator = s.IntegratorList->begin(); s.iIterator != s.IntegratorList->end(); s.iIterator++) {
		y[index] = (*s.iIterator)->Val();
		index++;
	}
	this->s.EvaluateAll();

    //reset index
    //cout << index << endl;
    index = 0;

	// k1 = h*f(t, y(t))
	for(s.iIterator = s.IntegratorList->begin(); s.iIterator != s.IntegratorList->end(); s.iIterator++) {
		k1[index] = step * (*s.iIterator)->Get();
		(*s.iIterator)->SetVal(y[index] + k1[index] / 2);
		index++;
	}
    this->s.EvaluateAll();

    STime += step / 2;

    //reset index
    index = 0;

	// k2 = h * f(t + h/2, y(t) + k1/2)
	for(s.iIterator = s.IntegratorList->begin(); s.iIterator != s.IntegratorList->end(); s.iIterator++) {
		k2[index] = step * (*s.iIterator)->Get();
		(*s.iIterator)->SetVal(y[index] + k2[index] / 2);
		index++;
	}

    //reset index
    index = 0;

	// time remains the same
	s.EvaluateAll();
	// k3 = h * f(t + h/2, y(t) + k2/2)
	for(s.iIterator = s.IntegratorList->begin(); s.iIterator != s.IntegratorList->end(); s.iIterator++) {
		k3[index] = step * (*s.iIterator)->Get();
		(*s.iIterator)->SetVal(y[index] + k3[index]);
		index++;
	}

	//reset index
    index = 0;

	STime = def_time + step;
	s.EvaluateAll();
	// k4 = h * f(t + h, y(t) + k3)
	for(s.iIterator = s.IntegratorList->begin(); s.iIterator != s.IntegratorList->end(); s.iIterator++) {
		k4[index] = step * (*s.iIterator)->Get();
		// y(t+h) = y(t) + k1/6 + k2/3 + k3/3 + k4/6
		(*s.iIterator)->SetVal(y[index] + k1[index]/6 + k2[index]/3 + k3[index]/3 + k4[index]/6);
		Print("%.3f ", (*s.iIterator)->Val());
		index++
	}

	STime = def_time;
	Print("\n");


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
