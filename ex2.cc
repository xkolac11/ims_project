#include "sim.hh"

int main(){
    SetOut("ex2.txt");
    //SetOut("out.txt");
    //SetOut("out_x.txt");

    Sim s;
    s.Start(0.0);
    s.End(10.0);
    s.Step(0.01);

    s.Method(new Adams_Bashforth(s));

    Const a(3.0);
	Const b(5.0);
	Const c(8.0);
	Const d(2.0);
	_Time t(&STime);
	Var y1, z2;
	Integrator y, z1, z;

	y1.Set((a*y - b*z + t)/c);
	z2.Set((-z + b*y1)/d);

	y.Set((-a*y - b*z + t)/c, 1.0);
	z1.Set((-z - b*y1)/d, 2.0);
	z.Set(z1, 3.0);

	s.Insert(&y);
	s.Insert(&z1);
	s.Insert(&z);


	s.StartSim();
}
