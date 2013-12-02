#include "sim.hh"

int main(){
    SetOut("ex2.txt");
    //SetOut("out.txt");
    //SetOut("out_x.txt");

    Sim s;
    s.Start(0.0);
    s.End(30.0);
    s.Step(0.01);

    s.Method(new Runge_Kutta_4(s));

    Const sigma(10.0);
    Const lambda(24.0);
    Const b(2.0);
    Const one(1.0);

    Integrator y1, y2, y3;

    y1.Set(sigma*(y2-y1),1.0);
    y2.Set((one + lambda -y3) *y1 -y2, 1.0);
    y3.Set(y1*y2 - b*y3, 1.0);

    s.Insert(&y1);
    s.Insert(&y2);
    s.Insert(&y3);

    s.StartSim();
}
